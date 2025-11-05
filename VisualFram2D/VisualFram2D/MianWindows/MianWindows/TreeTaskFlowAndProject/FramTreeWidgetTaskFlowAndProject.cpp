#pragma execution_character_set("utf-8")
#include "FramTreeWidgetTaskFlowAndProject.h"
#include <QStyleFactory>
#include <QGraphicsOpacityEffect>
#include <QMessageBox>
#include <QJsonArray>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QScreen>
#include <cmath>

// ================== 新增：DPI 自适配辅助工具 ==================
namespace {
    inline qreal GetScaleFactor(QWidget* w) {
        QScreen* sc = w ? w->screen() : nullptr;
        qreal dpi = sc ? sc->logicalDotsPerInch() : qreal(96.0);
        qreal scale = dpi / 96.0;
        if (scale < 0.75) scale = 0.75;
        if (scale > 3.0)  scale = 3.0;
        return scale;
    }

    inline int ScaledPixel(QWidget* w, int basePx) {
        return int(std::round(basePx * GetScaleFactor(w)));
    }

    inline QFont MakeScaledFont(QWidget* w, int basePx, const QString& family = QStringLiteral("Microsoft YaHei")) {
        QFont f(family);
        f.setPixelSize(ScaledPixel(w, basePx));
        return f;
    }

    inline QString BuildMenuButtonStyle(QWidget* w, int baseFontPx) {
        QFont f = MakeScaledFont(w, baseFontPx);
        QFontMetrics fm(f);
        int minH = fm.height() + ScaledPixel(w, 8); // 文字高度 + 内边距
        return QString(
            "QPushButton{color:#000;border:none;text-align:left;font:%1px \"%2\";"
            "padding-left:%3px;padding-right:%3px;min-height:%4px;}"
            "QPushButton::hover{background-color:rgba(176,190,225,200);}"
            "QPushButton::selected{background-color:rgba(176,190,255,255);}"
        ).arg(f.pixelSize())
            .arg(f.family())
            .arg(ScaledPixel(w, 5))
            .arg(minH);
    }

    inline QString BuildTreeStyle(int row_height) {
        return QString(
            "QTreeWidget::branch:has-children:!has-siblings:closed,"
            "QWidget{border:1px solid #d9d9d9}"
            "QTreeWidget::item{color:#363636;height:%1px}"
            "QTreeView::item::selected{background-color:#96B5A8}"
            "QTreeWidget::branch:closed:has-children:!has-siblings{image:url(:/MianWindows/Image/Close.png);}"
            "QTreeWidget::branch:closed:has-children:has-siblings{image:url(:/MianWindows/Image/Close.png);}"
            "QTreeWidget::branch:open:has-children:!has-siblings{image:url(:/MianWindows/Image/file-open.png);}"
            "QTreeWidget::branch:open:has-children:has-siblings{image:url(:/MianWindows/Image/file-open.png);}"
            "QScrollBar:vertical{background:#FFFFF2;padding-top:20px;padding-bottom:20px;"
            "padding-left:2px;padding-right:2px;border-left:1px solid #d7d7d7;}"
            "QScrollBar::handle:vertical{background:#dbdbdb;border-radius:6px;min-height:20px;}"
            "QScrollBar::handle:vertical:hover{background:#d0d0d0;}"
            "QScrollBar::add-line:vertical{background:url(:/MianWindows/Image/Flow.png) center no-repeat;}"
            "QScrollBar::sub-line:vertical{background:url(:/MianWindows/Image/Flow.png) center no-repeat;}"
        ).arg(row_height);
    }
}


FramTreeWidgetTaskFlowAndProject::FramTreeWidgetTaskFlowAndProject(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	Initial();
}

FramTreeWidgetTaskFlowAndProject::FramTreeWidgetTaskFlowAndProject(FramTaskFlowViewWidget* fram_widget,
	QWidget* parent):QWidget(parent)
{
	ui.setupUi(this);
	framTaskFlowViewWidget = fram_widget;
	Initial();
}

FramTreeWidgetTaskFlowAndProject::~FramTreeWidgetTaskFlowAndProject()
{
	ptrThreadPools->Shutdown();
}

void FramTreeWidgetTaskFlowAndProject::Initial()
{
	InitialPushButton();
	InitialTreeWidget();
	InitialMenu();

	FrmLog* ptr_framlog = FrmLog::FrmLogInstance();
	QObject::connect(this, &FramTreeWidgetTaskFlowAndProject::SendLogInfoSynSig, ptr_framlog, &FrmLog::LogMassageShowSlot);
	ptrThreadPools = std::make_shared<ThreadPool>(20);
	ptrThreadPools->Init();
}

void FramTreeWidgetTaskFlowAndProject::InitialPushButton()
{
	ui.pushButtonAddFlow->setIconSize(QSize(35, 35));
	ui.pushButtonAddFlow->setIcon(QIcon(":/MianWindows/Image/BuildFlow.png"));
	connect(ui.pushButtonAddFlow, &QPushButton::clicked, this, &FramTreeWidgetTaskFlowAndProject::AddTaskFlowSlot);

	ui.pushButtonDelteFlow->setIconSize(QSize(35, 35));
	ui.pushButtonDelteFlow->setIcon(QIcon(":/MianWindows/Image/delete.png"));
	connect(ui.pushButtonDelteFlow, &QPushButton::clicked, this, &FramTreeWidgetTaskFlowAndProject::DeleteTaskFlowSlot);

	ui.pushButtonOnce->setIconSize(QSize(35, 35));
	ui.pushButtonOnce->setIcon(QIcon(":/MianWindows/Image/Once.png"));
	connect(ui.pushButtonOnce, &QPushButton::clicked, this, &FramTreeWidgetTaskFlowAndProject::TaskFlowRunOnce);

	ui.pushButtonNCycle->setIconSize(QSize(35, 35));
	ui.pushButtonNCycle->setIcon(QIcon(":/MianWindows/Image/NCycle.png"));
	connect(ui.pushButtonNCycle, &QPushButton::clicked, this, &FramTreeWidgetTaskFlowAndProject::TaskFlowRunNCycle);

	ui.pushButtonCycle->setIconSize(QSize(35, 35));
	ui.pushButtonCycle->setIcon(QIcon(":/MianWindows/Image/Cycle.png"));
	connect(ui.pushButtonCycle, &QPushButton::clicked, this, &FramTreeWidgetTaskFlowAndProject::TaskFlowRunCycle);

	ui.pushButtonStop->setIconSize(QSize(35, 35));
	ui.pushButtonStop->setIcon(QIcon(":/MianWindows/Image/Stop.png"));
	connect(ui.pushButtonStop, &QPushButton::clicked, this, &FramTreeWidgetTaskFlowAndProject::TaskFlowStopRun);
}

void FramTreeWidgetTaskFlowAndProject::InitialTreeWidget()
{
    // 动态字体与行高
    QFont font = MakeScaledFont(this, 16);              // 基础 14px
    QFontMetrics fm(font);
    int row_height = fm.height() + ScaledPixel(this, 6); // 行高 = 字高 + padding

    ui.treeWidgetTaskFlows->setStyleSheet(BuildTreeStyle(row_height));
    ui.treeWidgetTaskFlows->setFont(font);
    ui.treeWidgetTaskFlows->header()->setVisible(false);
    ui.treeWidgetTaskFlows->clear();
    setAcceptDrops(false);
    ui.treeWidgetTaskFlows->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui.treeWidgetTaskFlows, &QTreeWidget::customContextMenuRequested,
        this, &FramTreeWidgetTaskFlowAndProject::OnRightClickedSlot);

	itemParent = new QTreeWidgetItem(ui.treeWidgetTaskFlows);
	itemParent->setText(0, "项目流程方案");
	//itemParent->setIcon(0, QIcon(":/MianWindows/Image/project.png"));
	itemParent->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled);

	connect(ui.treeWidgetTaskFlows, &QTreeWidget::itemPressed, this, &FramTreeWidgetTaskFlowAndProject::ItemPressedSlot, Qt::AutoConnection);
}

void FramTreeWidgetTaskFlowAndProject::InitialMenu()
{
    QString button_style =
        "QPushButton{color:#000;border:none;text-align:left;font:9pt Microsoft YaHei;"
        "padding-left:5px;padding-right:5px;min-height:26px;}"
        "QPushButton::hover{background-color:rgba(176,190,225,200);}"
        "QPushButton::selected{background-color:rgba(176,190,255,255);}";

    mMenu = new QMenu(this);
    // 菜单字体
    QFont menu_font = MakeScaledFont(this, 16);
    mMenu->setFont(menu_font);
    // 菜单样式可按需再动态扩展
    QString menu_style =
        "QMenu::item{color:#000;padding:4px;}"
        "QMenu::item:hover{background-color:rgb(176,190,225);}"
        "QMenu::item:selected{background-color:rgb(176,190,225);}";

    mMenu->setStyleSheet(menu_style);
    auto addMenuButton = [&](const QString& text, const QIcon& icon, auto slot) {
        QPushButton* btn = new QPushButton(mMenu);
        btn->setStyleSheet(button_style);
        btn->setText(text);
        btn->setIconSize({ 20,20 });
        btn->setIcon(icon);
        QWidgetAction* act = new QWidgetAction(this);
        act->setDefaultWidget(btn);
        connect(btn, &QPushButton::clicked, this, slot);
        connect(btn, &QPushButton::clicked, mMenu, &QMenu::close);
        mMenu->addAction(act);
    };

    addMenuButton("使能", QIcon(":/MianWindows/Image/State.png"),
        &FramTreeWidgetTaskFlowAndProject::DisableTaskFlowSlot);
    addMenuButton("更改名称", QIcon(":/MianWindows/Image/change.png"),
        &FramTreeWidgetTaskFlowAndProject::ModefyTaskFlowNameSlot);
    addMenuButton("删除流程", QIcon(":/MianWindows/Image/Delte.png"),
        &FramTreeWidgetTaskFlowAndProject::DeleteTaskFlowSlot);
}

bool FramTreeWidgetTaskFlowAndProject::DeserializeProject(const QJsonObject& json_project)
{
    QJsonArray project_json = json_project["Project"].toArray();

	for (QJsonValueRef taskflow_object : project_json) {
		auto taskflow_json = taskflow_object.toObject();
		QString old_name = taskflow_json["TaskFlowName"].toString();
        int task_flow_id = taskflow_json["TaskFlowId"].toInt();
		bool task_flow_enable = taskflow_json["TaskFlowEnabledState"].toBool();
		QJsonObject flowgraph_model_json = taskflow_json["FlowGraphModel"].toObject();
		emit SendLogInfoSynSig("反序列化流程名称：" + old_name, LOGTYPE::INFO);

		bool res = framTaskFlowViewWidget->AddTaskTableWidget(old_name, task_flow_id);
		if (!res) {
			emit SendLogInfoSynSig("反序列化流程名称重复，重新命名。", LOGTYPE::WARN);
			bool ok = false;
			QString new_name = QInputDialog::getText(this, tr("修改流程名"),tr("输入新名称: "), QLineEdit::Normal, old_name, &ok);

			if (ok && !new_name.isEmpty()) {
				emit SendLogInfoSynSig("反序列化流程重新命名为: " + new_name, LOGTYPE::WARN);
                int index = framTaskFlowViewWidget->GetTaskFlowIndex(new_name);
                new_name = QString::number(index) + flowNameSpace + new_name;
				bool res = framTaskFlowViewWidget->AddTaskTableWidget(new_name, task_flow_id);
				old_name = new_name;
			}
			else{
				QMessageBox::critical(NULL, QString("警告"),QString("流程反序列化失败"), QMessageBox::Yes, NULL);
				emit SendLogInfoSynSig(old_name + "反序列化失败", LOGTYPE::ERRORS);
				return false;
			}
		}
		GraphicsViewTaskFlowWidget* ptr_graphics_taskflow = framTaskFlowViewWidget->DelegateGraphicsTaskFlow(old_name);
		if (ptr_graphics_taskflow == nullptr) {
            emit SendLogInfoSynSig(old_name + " 序列化失败，请检查序列化参数！ ", LOGTYPE::ERRORS);
			return false;
		}
        int index = framTaskFlowViewWidget->GetTaskFlowIndex(old_name);
        old_name = QString::number(index) + flowNameSpace + old_name;
        QTreeWidgetAddItem(old_name, task_flow_enable);
		ptr_graphics_taskflow->SetTaskFlowEnabledState(task_flow_enable);
		auto flowgraph_model = ptr_graphics_taskflow->GetDataFlowGraphModel();
		flowgraph_model->load(flowgraph_model_json);
	}

	emit SendLogInfoSynSig("项目反序列化结束", LOGTYPE::INFO);
	return true;
}

bool FramTreeWidgetTaskFlowAndProject::SerializeProject(const QString& path)
{
	QFile file(path);
	if (file.open(QIODevice::WriteOnly)) {
		file.write(QJsonDocument(framTaskFlowViewWidget->SerializeProject()).toJson());
		return true;
	}
	return false;
}

QJsonObject FramTreeWidgetTaskFlowAndProject::SerializeProject()
{
    return framTaskFlowViewWidget->SerializeProject();
}

void FramTreeWidgetTaskFlowAndProject::OnRightClickedSlot(const QPoint& point)
{
	QTreeWidget* cur_tree = qobject_cast<QTreeWidget*>(sender());
	// 根据具体的坐标可以求出当前右键点击的是哪一个Item，然后做出对应的处理
	rightCurrentPressedItem = cur_tree->itemAt(point);
	mMenu->exec(QCursor::pos());
}

void FramTreeWidgetTaskFlowAndProject::ItemPressedSlot(QTreeWidgetItem* item, int column)
{
	///左键选择
	if (qApp->mouseButtons() == Qt::LeftButton) {
		QTreeWidgetItem* parent = item->parent();
		if (NULL == parent)
			return;
		leftCurrentPressedItem = item;
		auto name = leftCurrentPressedItem->text(0);
        name = name.split(flowNameSpace)[1];
		framTaskFlowViewWidget->SetCurrentTableWidget(name);
	}
}

void FramTreeWidgetTaskFlowAndProject::AddTaskFlow()
{
	this->AddTaskFlowSlot();
}


void FramTreeWidgetTaskFlowAndProject::AddTaskFlowSlot()
{
    bool ok = false;
    QString item_name = QInputDialog::getText(this, tr("流程名"),
        tr("输入名称："), QLineEdit::Normal, "flow", &ok);
    if (!ok || item_name.isEmpty()) {
        return;
    }
	bool res = framTaskFlowViewWidget->AddTaskTableWidget(item_name);
	if (!res) {
		emit SendLogInfoSynSig(item_name + "流程名重复，请重新命名", LOGTYPE::ERRORS);
		return;
	}
    
    int index = framTaskFlowViewWidget->GetTaskFlowIndex(item_name);
    item_name = QString::number(index) + flowNameSpace + item_name;
	QTreeWidgetAddItem(item_name);
}

void FramTreeWidgetTaskFlowAndProject::QTreeWidgetAddItem(const QString& item_name, const bool state)
{
    if (!itemParent) return;
    auto* tree_item = new QTreeWidgetItem();
    tree_item->setText(0, item_name);
    tree_item->setIcon(0, QIcon(":/image/images/flow.png"));
    SetItemEnableColor(tree_item, state);
    itemParent->addChild(tree_item);
    ui.treeWidgetTaskFlows->expandAll();
}

void FramTreeWidgetTaskFlowAndProject::DeleteTaskFlowSlot()
{
	if (leftCurrentPressedItem == nullptr) {
		QMessageBox::critical(NULL, QString("提示"),
			QString("未选择有效流程"), QMessageBox::Yes, NULL);

		emit SendLogInfoSynSig("删除流程失败，未选中流程", LOGTYPE::ERRORS);
		return;
	}
	QMessageBox message(QMessageBox::NoIcon, QString("提示"),
		QString("是否删除本行数据?"),
		QMessageBox::Yes | QMessageBox::No, NULL);

	if (message.exec() != QMessageBox::Yes)
		return;

	QTreeWidgetItem* parent = leftCurrentPressedItem->parent();
	if (nullptr == parent)
		return;

	int press_index = parent->indexOfChild(leftCurrentPressedItem);
	QString flow_name = leftCurrentPressedItem->text(0);
    flow_name = flow_name.split(flowNameSpace)[1];
	framTaskFlowViewWidget->DelteTableWidget(flow_name);

	emit SendLogInfoSynSig("删除流程： " + flow_name, LOGTYPE::WARN);

	delete leftCurrentPressedItem;
	leftCurrentPressedItem = nullptr;
}

void FramTreeWidgetTaskFlowAndProject::DisableTaskFlowSlot()
{
	if (!rightCurrentPressedItem)
		return;
	QString name = rightCurrentPressedItem->text(0);
    QString tool_name = ExtractBaseName(name);

	GraphicsViewTaskFlowWidget* flow = framTaskFlowViewWidget->DelegateGraphicsTaskFlow(tool_name);
    if (!flow) {
        emit SendLogInfoSynSig("流程不存在: " + tool_name, LOGTYPE::ERRORS);
        return;
    }
    bool new_state = !flow->GetTaskFlowEnabledState();
    flow->SetTaskFlowEnabledState(new_state);
    SetItemEnableColor(rightCurrentPressedItem, new_state);
    emit SendLogInfoSynSig(QString("设置流程 %1 %2")
        .arg(tool_name).arg(new_state ? "使能" : "非使能"),
        LOGTYPE::INFO);
}

/// <summary>
/// 需要改
/// </summary>
void FramTreeWidgetTaskFlowAndProject::ModefyTaskFlowNameSlot()
{
	if (!rightCurrentPressedItem)
		return;
	QString old_name = rightCurrentPressedItem->text(0);
    old_name = old_name.split(flowNameSpace)[1];
	bool ok = false;
	QString new_name = QInputDialog::getText(this, tr("修改流程名"),
		tr("输入新名称："), QLineEdit::Normal, old_name, &ok);

	if (ok && !new_name.isEmpty()) {
		bool res = framTaskFlowViewWidget->ModifyTableWidgetName(old_name, new_name);
		if (!res) {
			emit SendLogInfoSynSig("流程--" + old_name + "重命名失败", LOGTYPE::ERRORS);
			return;
		}

        int index = framTaskFlowViewWidget->GetTaskFlowIndex(new_name);
        new_name = QString::number(index) + flowNameSpace + new_name;
		rightCurrentPressedItem->setText(0, new_name);
		emit SendLogInfoSynSig("流程--" + old_name + "改名:--" + new_name, LOGTYPE::INFO);
	}
	else {
		emit SendLogInfoSynSig("流程--" + old_name + "重命名失败", LOGTYPE::ERRORS);
	}
}


void FramTreeWidgetTaskFlowAndProject::TaskFlowRunOnce()
{
	if (leftCurrentPressedItem == nullptr) {
		QMessageBox::critical(NULL, QString("提示"),
			QString("未选择有效流程"), QMessageBox::Yes, NULL);
		return;
	}
	QString name = leftCurrentPressedItem->text(0);
    name = name.split(flowNameSpace)[1];

	auto ptr = framTaskFlowViewWidget->DelegateGraphicsTaskFlow(name);

	if (!ptr->GetTaskFlowEnabledState()) {
		QMessageBox::critical(NULL, QString("提示"),
			QString("请打开流程使能"), QMessageBox::Yes, NULL);
		emit SendLogInfoSynSig("流程--" + name + "使能未打开", LOGTYPE::WARN);
		return;
	}

	emit SendLogInfoSynSig("流程--" + name + "运行一次", LOGTYPE::INFO);

	ptrThreadPools->Submit([this, ptr]() {
		ptr->TaskFlowOnceExecutor();
		});
}

void FramTreeWidgetTaskFlowAndProject::TaskFlowRunNCycle()
{
	if (leftCurrentPressedItem == nullptr) {
		QMessageBox::critical(NULL, QString("提示"),
			QString("未选择有效流程"), QMessageBox::Yes, NULL);
		return;
	}

	QString dlgTitle = "执行次数";//对话框标题
	QString txtLabel = "设置执行次数";//对话框Label显示内容
	int defaultValue = 0;
	int minValue = 0, maxValue = 1000000, stepValue = 1; //设置整数编辑框的最小、大值，与单步改变值
	bool ok = false;
	int inputValue = QInputDialog::getInt(this, dlgTitle, txtLabel, defaultValue, minValue, maxValue, stepValue, &ok);
	if (!ok) {
		return;
	}

	QString name = leftCurrentPressedItem->text(0);
    name = name.split(flowNameSpace)[1];
	auto ptr = framTaskFlowViewWidget->DelegateGraphicsTaskFlow(name);
    if (ptr == nullptr) {
        emit SendLogInfoSynSig("流程--" + name + "不存在，请检查后，开启! ", LOGTYPE::ERRORS);
    }

	if (!ptr->GetTaskFlowEnabledState()) {
		QMessageBox::critical(NULL, QString("提示"),
			QString("请打开流程使能"), QMessageBox::Yes, NULL);
		emit SendLogInfoSynSig("流程--" + name + "使能未打开", LOGTYPE::WARN);
		return;
	}

	if (ptr->GetTaskFlowRunState()) {
		QMessageBox::critical(NULL, QString("提示"),
			QString("当前流程正在运行，请勿重复开启"), QMessageBox::Yes, NULL);
		emit SendLogInfoSynSig("流程--" + name + "已开启，请勿重复开启", LOGTYPE::ERRORS);
		return;
	}

	emit SendLogInfoSynSig("流程--" + name + "运行" + QString::number(inputValue) + "次", LOGTYPE::INFO);

	ptrThreadPools->Submit([this, ptr, inputValue]() {
		ptr->TaskFlowNCycleExecutor(inputValue);
		});
}

void FramTreeWidgetTaskFlowAndProject::TaskFlowRunCycle()
{
	if (leftCurrentPressedItem == nullptr) {
		QMessageBox::critical(NULL, QString("提示"),
			QString("未选择有效流程"), QMessageBox::Yes, NULL);
		return;
	}
	QString name = leftCurrentPressedItem->text(0);
    name = name.split(flowNameSpace)[1];
	auto ptr = framTaskFlowViewWidget->DelegateGraphicsTaskFlow(name);

	if (!ptr->GetTaskFlowEnabledState()) {
		QMessageBox::critical(NULL, QString("提示"),
			QString("请打开流程使能"), QMessageBox::Yes, NULL);
		emit SendLogInfoSynSig("流程--" + name + "使能未打开", LOGTYPE::WARN);
		return;
	}

	if (ptr->GetTaskFlowRunState()) {
		QMessageBox::critical(NULL, QString("提示"),
			QString("当前流程正在运行，请勿重复开启"), QMessageBox::Yes, NULL);
		emit SendLogInfoSynSig("流程--" + name + "已开启，请勿重复开启", LOGTYPE::ERRORS);
		return;
	}
	emit SendLogInfoSynSig("流程--" + name + "开始循环运行", LOGTYPE::INFO);

	ptrThreadPools->Submit([this, ptr]() {
		ptr->TaskFlowCycleExecutor();
	});
}

void FramTreeWidgetTaskFlowAndProject::TaskFlowStopRun()
{
	if (leftCurrentPressedItem == nullptr) {
		QMessageBox::critical(NULL, QString("提示"),
				QString("未选择有效流程"), QMessageBox::Yes, NULL);
		return;
	}
	QString name = leftCurrentPressedItem->text(0);
    name = name.split(flowNameSpace)[1];
	auto ptr = framTaskFlowViewWidget->DelegateGraphicsTaskFlow(name);
	emit SendLogInfoSynSig("流程--" + name + "停止运行", LOGTYPE::INFO);
	ptrThreadPools->Submit([this, ptr]() {
		ptr->TaskFlowStopExecutor();
	});
}

// 提取原始名称（如果不存在分隔符，返回原值）
QString FramTreeWidgetTaskFlowAndProject::ExtractBaseName(const QString& display) const {
    int pos = display.indexOf(flowNameSpace);
    if (pos < 0) return display;
    return display.mid(pos + flowNameSpace.size());
}
