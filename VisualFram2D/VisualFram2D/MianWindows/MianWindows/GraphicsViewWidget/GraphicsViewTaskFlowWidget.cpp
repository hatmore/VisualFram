#pragma execution_character_set("utf-8")
#include "GraphicsViewTaskFlowWidget.h"
#include "../StaticGlobalVariable.h"
#include <QInputDialog>
#include <QToolBar >
#define MaxThreadNum 1024

namespace {
    // 简单的限频日志辅助（可扩展为按 key）
    class WhileRateLimiter {
    public:
        bool Allow(std::chrono::milliseconds interval) {
            auto now = std::chrono::steady_clock::now();
            if (now - last >= interval) {
                last = now;
                return true;
            }
            return false;
        }
    private:
        std::chrono::steady_clock::time_point last{};
    };
}


GraphicsViewTaskFlowWidget::GraphicsViewTaskFlowWidget(QWidget* parent):QWidget(parent)
{
	Initial();
}

GraphicsViewTaskFlowWidget::~GraphicsViewTaskFlowWidget()
{
	this->TaskFlowStopExecutor();
	ptrThreadPools->Shutdown();
    auto iter = StaticGlobalVariable::ptrToolNodeDataInteract->ptrMapMapAllNodesOutData->find(this->taskFlowId);
    if (iter != StaticGlobalVariable::ptrToolNodeDataInteract->ptrMapMapAllNodesOutData->end()) {
        StaticGlobalVariable::ptrToolNodeDataInteract->ptrMapMapAllNodesOutData->erase(iter);
    }
   // this->deleteLater();
}


void GraphicsViewTaskFlowWidget::Initial()
{
	FrmLog* ptr_framlog = FrmLog::FrmLogInstance();
    //同步
	QObject::connect(this, &GraphicsViewTaskFlowWidget::SendLogInfoSynSig, ptr_framlog, &FrmLog::LogMassageShowSlot);
    //异步
    QObject::connect(this, &GraphicsViewTaskFlowWidget::SendLogInfoAsynSig, ptr_framlog, &FrmLog::LogMassageShowSlot, Qt::BlockingQueuedConnection);

	InitialQtNode();
	InitialTaskFlow();
	InitialPushButton();
	InitialButtonMenu();
	ptrThreadPools = std::make_shared<ThreadPool>(MaxThreadNum);
	ptrThreadPools->Init();
}

void GraphicsViewTaskFlowWidget::InitialQtNode()
{
    ptrMapNodesOutData = std::make_shared<std::unordered_map<int, std::vector<PtrVMObject>>>();

	ptrNodeDelegateModelRegistry = std::make_shared<NodeDelegateModelRegistry>();
	dataFlowGraphModel = new QtNodes::DataFlowGraphModelOverride(ptrNodeDelegateModelRegistry);
	dataFlowGraphicsScene = new  QtNodes::DataFlowGraphicsSceneOverride(*dataFlowGraphModel);
	//改变方向
	//dataFlowGraphicsScene->setOrientation(Qt::Vertical);
	graphicsView = new  GraphicsViewDrapEvent(dataFlowGraphicsScene);

    dataFlowGraphModel->SetToolNodeDataInteract(StaticGlobalVariable::ptrToolNodeDataInteract);
    dataFlowGraphModel->SetToolNodeMapData(ptrMapNodesOutData);

	auto map_function_plugin = PluginsManager::PluginsManagerInstance()->mapPtrInterfacePlugins;
	for (auto iter = map_function_plugin.begin(); iter != map_function_plugin.end(); ++iter) {
		iter->second->NodeModelRegistry(ptrNodeDelegateModelRegistry);
	}

	QObject::connect(dataFlowGraphicsScene, &QtNodes::DataFlowGraphicsSceneOverride::nodeContextMenu, [this](QtNodes::NodeId const nodeId, QPointF const pos) {
		QtNodes::NodeId id = nodeId;
		currentNodeId = id;
		mMenu->exec(QCursor::pos());
		});
}

void GraphicsViewTaskFlowWidget::InitialTaskFlow()
{
	ptrtfExecutor = std::make_shared<tf::Executor>();
	ptrtfTaskflow = std::make_shared<tf::Taskflow>();
}

void GraphicsViewTaskFlowWidget::InitialPushButton()
{
	QAction* qactive_open = new QAction(tr("打开"));
	qactive_open->setIcon(QIcon(":/MianWindows/image/open.png"));
	this->addAction(qactive_open);

	QAction* qactive_save = new QAction(tr("保存"));
	qactive_save->setIcon(QIcon(":/MianWindows/image/open.png"));
	this->addAction(qactive_save);

	QAction* qactive_once = new QAction(tr("运行一次"));
	qactive_once->setIcon(QIcon(":/MianWindows/image/Once.png"));
	this->addAction(qactive_once);

	QAction* qactive_n = new QAction(tr("运行n次"));
	qactive_n->setIcon(QIcon(":/MianWindows/image/NCycle.png"));
	this->addAction(qactive_n);

	QAction* qactive_cycle = new QAction(tr("循环运行"));
	qactive_cycle->setIcon(QIcon(":/MianWindows/image/Cycle.png"));
	this->addAction(qactive_cycle);

	QAction* qactive_stop = new QAction(tr("暂停"));
	qactive_stop->setIcon(QIcon(":/MianWindows/image/Stop.png"));
	this->addAction(qactive_stop);

	this->setContextMenuPolicy(Qt::ActionsContextMenu);

	qHBoxLayout = new QHBoxLayout(this);
	qHBoxLayout->setContentsMargins(0,4,0,0);
	qHBoxLayout->addWidget(graphicsView);


	QObject::connect(qactive_save, &QAction::triggered, [this]() {
		dataFlowGraphicsScene->save();
		emit SendLogInfoSynSig(this->taskFlowName + "--保存序列化文件", LOGTYPE::INFO);
		});

	QObject::connect(qactive_open, &QAction::triggered, [this]() {
		dataFlowGraphicsScene->load();
		emit SendLogInfoSynSig(this->taskFlowName + "--加载序列化文件", LOGTYPE::INFO);
		});

	QObject::connect(qactive_once, &QAction::triggered, [this]() {
		this->BuildNodeTaskFlow();
		ptrtfTaskflow->dump(std::cout);
		ptrThreadPools->Submit([this]() {
			ptrtfExecutor->run(*ptrtfTaskflow).wait();
			});
		emit SendLogInfoSynSig(this->taskFlowName + "--执行一次流程", LOGTYPE::INFO);
		});

	QObject::connect(qactive_n, &QAction::triggered, [this]() {

		QString dlgTitle = "执行次数";//对话框标题
		QString txtLabel = "设置执行次数";//对话框Label显示内容
		int defaultValue = 0;
		int minValue = 0, maxValue = 100000, stepValue = 1; //设置整数编辑框的最小、大值，与单步改变值
		bool ok = false;
		int inputValue = QInputDialog::getInt(this, dlgTitle, txtLabel, defaultValue, minValue, maxValue, stepValue, &ok);
		if (!ok) {
			return;
		}
		this->BuildNodeTaskFlow();
		ptrThreadPools->Submit([this, inputValue]() {
			this->TaskFlowNCycleExecutor(inputValue);
			});
		emit SendLogInfoSynSig(this->taskFlowName + "--循环流程" + QString::number(minValue) + "次数", LOGTYPE::INFO);
		});

	QObject::connect(qactive_cycle, &QAction::triggered, [this]() {
		emit SendLogInfoSynSig(this->taskFlowName + "--开始循环执行流程", LOGTYPE::INFO);
		this->BuildNodeTaskFlow();
		this->isCycleRun = true;
		ptrThreadPools->Submit([this]() {
			this->TaskFlowCycleExecutor();
			});
		});

	QObject::connect(qactive_stop, &QAction::triggered, [this]() {
		emit SendLogInfoSynSig(this->taskFlowName + "--结束循环执行", LOGTYPE::INFO);
		this->isCycleRun = false;
		});

	QObject::connect(dataFlowGraphicsScene, &QtNodes::DataFlowGraphicsSceneOverride::nodeDoubleClicked, [this](QtNodes::NodeId const nodeId) {
		QtNodes::NodeId id = nodeId;
		emit SendLogInfoSynSig(this->taskFlowName + "--打开工具：" + QString::number(id), LOGTYPE::INFO);
		auto ptr = dataFlowGraphModel->delegateModel<BaseNodeModel>(id);
		if (ptr) {
			auto inter = ptr->GetToolInterface();
			inter->exec();
		}
		});
}

void GraphicsViewTaskFlowWidget::InitialButtonMenu()
{
	mMenu = new QMenu(this);
	//菜单样式
	QString menuStyle(
		"QMenu::item{"
		"color: rgb(0, 0, 0);"
		"padding-up: 2px;"
		"padding-down: 2px;"
		"}"
		"QMenu::item:hover{"
		"background-color: rgb(176, 190, 225);"
		"}"
		"QMenu::item:selected{"
		"background-color: rgb(176, 190, 225);"
		"}"
	);
	QString buttonStyle(
		"QPushButton{"
		"color: rgb(0, 0, 0);"
		"border: none;"
		"text-align: left;"
		"font: 9pt Microsoft YaHei;"
		"padding-left: 5px;"
		"padding-right: 5px;"
		"min-height: 26px;"
		"}"
		"QPushButton::hover{"
		"background-color: rgba(176, 190, 225, 200);"
		"}"
		"QPushButton::selected{"
		"background-color: rgba(176, 190, 255, 255);"
		"}"
	);
	mMenu->setStyleSheet(menuStyle);
    QPushButton* currentAction_buf = new QPushButton(mMenu);
    currentAction_buf->setStyleSheet(buttonStyle);
    currentAction_buf->setText("运行当前工具");
    currentAction_buf->setIconSize(QSize(20, 22));
    currentAction_buf->setIcon(QIcon(":/res/ico/current.png"));
    QWidgetAction* currentAction = new QWidgetAction(this);
    currentAction->setDefaultWidget(currentAction_buf);
    connect(currentAction_buf, &QPushButton::clicked, this, &GraphicsViewTaskFlowWidget::RunCurrentNodeToolSlot);
    connect(currentAction_buf, &QPushButton::clicked, mMenu, &QMenu::close);
    mMenu->addAction(currentAction);


    QPushButton* enabledAction_buf = new QPushButton(mMenu);
    enabledAction_buf->setStyleSheet(buttonStyle);
    enabledAction_buf->setText("禁用");
    enabledAction_buf->setIconSize(QSize(22, 22));
    enabledAction_buf->setIcon(QIcon(":/res/ico/del.png"));
    QWidgetAction* enableAction = new QWidgetAction(this);
    enableAction->setDefaultWidget(enabledAction_buf);
    connect(enabledAction_buf, &QPushButton::clicked, this, &GraphicsViewTaskFlowWidget::EnableCurrentNodeToolSlot);
    connect(enabledAction_buf, &QPushButton::clicked, mMenu, &QMenu::close);
    mMenu->addAction(enableAction);


    QPushButton* modefyAction_buf = new QPushButton(mMenu);
    modefyAction_buf->setStyleSheet(buttonStyle);
    modefyAction_buf->setText("更改名称");
    modefyAction_buf->setIconSize(QSize(22, 22));
    modefyAction_buf->setIcon(QIcon(":/res/ico/del.png"));
    QWidgetAction* modefyAction = new QWidgetAction(this);
    modefyAction->setDefaultWidget(modefyAction_buf);
    connect(modefyAction_buf, &QPushButton::clicked, this, &GraphicsViewTaskFlowWidget::ModefyCurrentNodeTitleNameSlot);
    connect(modefyAction_buf, &QPushButton::clicked, mMenu, &QMenu::close);
    mMenu->addAction(modefyAction);


    QPushButton* deleteAction_buf = new QPushButton(mMenu);
    deleteAction_buf->setStyleSheet(buttonStyle);
    deleteAction_buf->setText("删除");
    deleteAction_buf->setIconSize(QSize(22, 22));
    deleteAction_buf->setIcon(QIcon(":/res/ico/del.png"));
    QWidgetAction* deleteAction = new QWidgetAction(this);
    deleteAction->setDefaultWidget(deleteAction_buf);
    connect(deleteAction_buf, &QPushButton::clicked, this, &GraphicsViewTaskFlowWidget::DeleteCurrentNodeToolSlot);
    connect(deleteAction_buf, &QPushButton::clicked, mMenu, &QMenu::close);
    mMenu->addAction(deleteAction);
}


void GraphicsViewTaskFlowWidget::SetTaskFlowId(const int id)
{
    taskFlowId = id;
    StaticGlobalVariable::ptrToolNodeDataInteract->ptrMapMapAllNodesOutData->insert(std::make_pair(taskFlowId, ptrMapNodesOutData));
}

void GraphicsViewTaskFlowWidget::EnableCurrentNodeToolSlot()
{
	auto ptr = dataFlowGraphModel->delegateModel<BaseNodeModel>(currentNodeId);
	auto ptr_node = ptr->GetToolInterface();
}

void GraphicsViewTaskFlowWidget::ModefyCurrentNodeTitleNameSlot()
{
	auto ptr = dataFlowGraphModel->delegateModel<BaseNodeModel>(currentNodeId);
	QtNodes::NodeId node_id = ptr->GetNodeId();
	auto node_widget = ptr->nodeEmbedWidget;

	QString dlgTitle = "更改节点名称";
	QString txtLabel = "输入节点名称";
	QString defaultInput = node_widget->GetNodeTitle();
	QLineEdit::EchoMode echoMode = QLineEdit::Normal;//正常文字输入
	//QLineEdit::EchoMode echoMode=QLineEdit::Password;//密码输入
	bool ok = false;
	QString text = QInputDialog::getText(this, dlgTitle, txtLabel, echoMode, defaultInput, &ok);
	if (ok && !text.isEmpty()) {
		node_widget->ModifyNodeTitle(node_id, text);
	}

	emit SendLogInfoSynSig(this->taskFlowName + "--节点：" + QString::number(node_id) + "修改名称: " + text, LOGTYPE::INFO);
}


void GraphicsViewTaskFlowWidget::RunCurrentNodeToolSlot()
{
	auto ptr = dataFlowGraphModel->delegateModel<BaseNodeModel>(currentNodeId);
	auto ptr_node = ptr->GetToolInterface();
	ptr_node->OnceExecute();
}

void GraphicsViewTaskFlowWidget::DeleteCurrentNodeToolSlot()
{
	bool res = dataFlowGraphModel->deleteNode(currentNodeId);
}

void GraphicsViewTaskFlowWidget::BuildNodeTaskFlow()
{
    if (!ptrtfTaskflow) 
        return;
	ptrtfTaskflow->clear();
	ptrtfTaskflow->name(taskFlowName.toStdString());
	std::vector<QtNodes::NodeId> vSourceNodeId;
	std::unordered_map<QtNodes::NodeId, PtrtfTask> mapTaskNodeId;
	std::unordered_set<QtNodes::NodeId> all_node = dataFlowGraphModel->allNodeIds();

	for (auto iter = all_node.begin(); iter != all_node.end(); ++iter) {
		QtNodes::NodeId id = *iter;
		auto ptr_node = dataFlowGraphModel->delegateModel<BaseNodeModel>(id);
		if (ptr_node == nullptr)
			continue;
		mapTaskNodeId[id] = std::make_shared<tf::Task>();
		*mapTaskNodeId[id] = ptrtfTaskflow->emplace([ptr_node]() {
			ToolInterface* ptr_interface = ptr_node->GetToolInterface();
			ptr_interface->Execute();
			}).name(std::to_string(id));
	}

	for (auto iter = all_node.begin(); iter != all_node.end(); ++iter) {
		QtNodes::NodeId id = *iter;
		std::unordered_set<QtNodes::ConnectionId> connect_id = dataFlowGraphModel->connections(id, PortType::Out, 0);
		for (auto connect_iter = connect_id.begin(); connect_iter != connect_id.end(); connect_iter++) {
			QtNodes::NodeId in_port_index = connect_iter->inNodeId;
			mapTaskNodeId[id]->precede(*mapTaskNodeId[in_port_index]);
		}
	}

	emit SendLogInfoSynSig(this->taskFlowName + "--构建流程图成功", LOGTYPE::INFO);
}

void GraphicsViewTaskFlowWidget::BuildNodeTaskFlow(PtrtfTaskflow& ptr_task_flow, PtrtfTask& ptr_tftask, const QtNodes::NodeId& id)
{
	QtNodes::NodeId node_port_index = 0;
	std::unordered_set<QtNodes::ConnectionId> connect_id = dataFlowGraphModel->connections(id, PortType::Out, 0);
	for (auto connect_iter = connect_id.begin(); connect_iter != connect_id.end(); connect_iter++) {
		QtNodes::NodeId out_port_index = connect_iter->outPortIndex;
		if (out_port_index == node_port_index) {
			QtNodes::NodeId in_port_index = connect_iter->inNodeId;
			auto ptr_node = dataFlowGraphModel->delegateModel<BaseNodeModel>(in_port_index);
			//std::cout << "NodeId: "<< in_port_index << std::endl;
			PtrtfTask tf_task = std::make_shared<tf::Task>();
			*tf_task = ptr_task_flow->emplace([ptr_node]() {
				ToolInterface* ptr_interface = ptr_node->GetToolInterface();
				//	std::cout << "GetNodeId: " << ptr_node->GetNodeId() << std::endl;
				ptr_interface->Execute();
				}).name(std::to_string(in_port_index));
				ptr_tftask->precede(*tf_task);
				BuildNodeTaskFlow(ptr_task_flow, tf_task, in_port_index);
		}
	}
}

void GraphicsViewTaskFlowWidget::TaskFlowStopExecutor()
{
    if (this == nullptr) {
        return;
    }
    isCycleRun.store(false);
}

void GraphicsViewTaskFlowWidget::TaskFlowOnceExecutor()
{
    if (!isEnabledState && RateLimiter(std::chrono::milliseconds(500))) {
        emit SendLogInfoAsynSig(this->taskFlowName + " -- 当前流程使用为False", LOGTYPE::WARN);
        return;
    }

	this->BuildNodeTaskFlow();
	std::chrono::high_resolution_clock::time_point tp1 = std::chrono::high_resolution_clock::now();
	ptrtfTaskflow->dump(std::cout);
	auto future_get = ptrtfExecutor->run(*ptrtfTaskflow);

	if (future_get.wait_for(StaticGlobalVariable::threadTimeOut) == std::future_status::timeout) {
		emit SendLogInfoAsynSig(this->taskFlowName + " -- 线程超时", LOGTYPE::ERRORS);
		return;
	}

	std::chrono::high_resolution_clock::time_point tp2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<size_t, std::nano> dur = tp2 - tp1;
	float elapsed_time = float(std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());
	QString str = this->taskFlowName + " -- 耗时: " + QString::number(elapsed_time, 'f', 2) + "ms";
	emit SendLogInfoAsynSig(str, LOGTYPE::INFO);
}


void GraphicsViewTaskFlowWidget::TaskFlowNCycleExecutor(const int& set_cyc_num)
{
    if (!isEnabledState && RateLimiter(std::chrono::milliseconds(500))) {
        emit SendLogInfoAsynSig(this->taskFlowName + " -- 当前流程使用为False", LOGTYPE::WARN);
        return;
    }
    if (set_cyc_num <= 0) {
        emit SendLogInfoAsynSig(this->taskFlowName + " -- 执行次数必须大于0", LOGTYPE::ERRORS);
        return;
    }
	int cycle_num = 0;
	isCycleRun = true;
	this->BuildNodeTaskFlow();
    WhileRateLimiter whilerate_limiter;
    while (isCycleRun.load() && cycle_num < set_cyc_num) {
		std::chrono::high_resolution_clock::time_point tp1 = std::chrono::high_resolution_clock::now();

		cycle_num = cycle_num + 1;
		auto future_get = ptrtfExecutor->run(*ptrtfTaskflow);

		if (future_get.wait_for(StaticGlobalVariable::threadTimeOut) == std::future_status::timeout) {
			emit SendLogInfoAsynSig(this->taskFlowName + ": " + QString::number(cycle_num) + " -- 线程超时", LOGTYPE::ERRORS);
			this->TaskFlowStopExecutor();
			continue;
		}

		std::chrono::high_resolution_clock::time_point tp2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<size_t, std::nano> dur = tp2 - tp1;
		float elapsed_time = float(std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());
		QString str = this->taskFlowName + ": " + QString::number(cycle_num) + " 耗时: " + QString::number(elapsed_time, 'f', 2) + "ms";
        if(whilerate_limiter.Allow(std::chrono::milliseconds(500))) {
            emit SendLogInfoAsynSig(str, LOGTYPE::INFO);
        }
	}

	this->TaskFlowStopExecutor();
	//ptrtfExecutor->run_until(*ptrtfTaskflow, [this, cycle_num,counter = cyc_num]() mutable {
	//	emit SendCycleNumSig(cycle_num++);
	//	return counter-- == 0;
	//	}).wait();
}

void GraphicsViewTaskFlowWidget::TaskFlowCycleExecutor()
{
    if (!isEnabledState && RateLimiter(std::chrono::milliseconds(500))) {
        emit SendLogInfoAsynSig(this->taskFlowName + " -- 当前流程使用为False", LOGTYPE::WARN);
        return;
    }
	int cycle_num = 0;
	isCycleRun = true;
	this->BuildNodeTaskFlow();
    WhileRateLimiter whilerate_limiter;
    while (isCycleRun.load()) {
		std::chrono::high_resolution_clock::time_point tp1 = std::chrono::high_resolution_clock::now();
		cycle_num = cycle_num + 1;
		auto future_get = ptrtfExecutor->run(*ptrtfTaskflow);

		if(future_get.wait_for(StaticGlobalVariable::threadTimeOut) == std::future_status::timeout) {	
			emit SendLogInfoAsynSig(this->taskFlowName + ": " + QString::number(cycle_num) + "线程超时", LOGTYPE::ERRORS);
			this->TaskFlowStopExecutor();
			continue;
		}

		std::chrono::high_resolution_clock::time_point tp2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<size_t, std::nano> dur = tp2 - tp1;
		float elapsed_time = float(std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());
		QString str = this->taskFlowName + ": " + QString::number(cycle_num) + " 耗时: " + QString::number(elapsed_time, 'f', 2) + "ms";
        if (whilerate_limiter.Allow(std::chrono::milliseconds(500))) {
            emit SendLogInfoAsynSig(str, LOGTYPE::INFO);
        }
	}
	this->TaskFlowStopExecutor();
}