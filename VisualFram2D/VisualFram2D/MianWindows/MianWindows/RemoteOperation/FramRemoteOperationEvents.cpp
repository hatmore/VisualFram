#include "FramRemoteOperationEvents.h"
#include "../GraphicsViewWidget/FramTaskFlowViewWidget.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMenu>
#include <qDebug>
#include <QJsonArray>
#include "../GeneralStruct.h"

FramRemoteOperationEvents::FramRemoteOperationEvents(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
    Initial();
}

FramRemoteOperationEvents::~FramRemoteOperationEvents()
{}

void FramRemoteOperationEvents::Initial()
{
    InitialSetUI();
    InitialTabWidget();
    InitialPushButton();
    InitialCombox();
    InitialInitialBottonMenu();
    FrmLog* ptr_framlog = FrmLog::FrmLogInstance();
    QObject::connect(this, &FramRemoteOperationEvents::SendLogInfoSynSig, ptr_framlog, &FrmLog::LogMassageShowSlot);
}

void FramRemoteOperationEvents::InitialSetUI()
{
    this->setWindowTitle("远程操作");
    this->setWindowIcon(QIcon(":/MianWindows/Image/RemoteOperation.png"));
    // 去掉问号，只保留关闭
    this->setWindowFlags(Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint);
    this->installEventFilter(this);
    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);
}

void FramRemoteOperationEvents::InitialTabWidget()
{
    ui.tableWidgetRemoteOperation->setColumnCount(1);
    ui.tableWidgetRemoteOperation->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.tableWidgetRemoteOperation->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行
    ui.tableWidgetRemoteOperation->setSelectionMode(QAbstractItemView::SingleSelection);  //设置只能单选	
    connect(ui.tableWidgetRemoteOperation, SIGNAL(cellClicked(int, int)), this, SLOT(TableWidgetRemoteOperationClicked(int, int)));
    //connect(ui.tableWidgetRemoteOperation, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(TabWidgetDoubleClicked(int, int)));

    ui.tableWidgetSignalEvent->setColumnCount(5);
    ui.tableWidgetSignalEvent->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.tableWidgetSignalEvent->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行
    ui.tableWidgetSignalEvent->setSelectionMode(QAbstractItemView::SingleSelection);  //设置只能单选
}

void FramRemoteOperationEvents::InitialPushButton()
{
    connect(ui.pushButtonAdd, &QPushButton::clicked, this, &FramRemoteOperationEvents::AddRemoteOpterate);
    connect(ui.pushButtonReload, &QPushButton::clicked, this, &FramRemoteOperationEvents::ReloadRemoteOpterate);
    connect(ui.pushButtonCompleteBind, &QPushButton::clicked, this, &FramRemoteOperationEvents::RemoteOperationCompleteBind);

}

void FramRemoteOperationEvents::InitialInitialBottonMenu()
{
    QMenu* m_menu = new QMenu(this);
    //菜单样式
    QString menuStyle(
        "QMenu::item{"
        "color: rgb(0, 0, 0);"
        "}"
        "QMenu::item:hover{"
        "background-color: rgb(176, 190, 225);"
        "}"
        "QMenu::item:selected{"
        "background-color: rgb(176, 190, 225);"
        "}"
    );
    m_menu->setStyleSheet(menuStyle);
    m_menu = new QMenu();
    m_menu->setIcon(QIcon(":/MianWindows/image/muen.png"));

    QMenu* child_menu = m_menu->addMenu("添加");
    child_menu->setIcon(QIcon(":/MianWindows/image/add.png"));
    QIcon icon = QIcon(":/MianWindows/image/int.png");

    child_menu->addAction(icon, "Int", this, SLOT(MenuActionsSlot()));
    child_menu->addAction("Float", this, SLOT(MenuActionsSlot()));
    child_menu->addAction("String", this, SLOT(MenuActionsSlot()));
    m_menu->addAction("删除", this, SLOT(MenuActionsSlot()));

    //给控件设置上下文菜单策略
    ui.tableWidgetSignalEvent->setContextMenuPolicy(Qt::CustomContextMenu);
    //给信号设置相应的槽函数
    connect(ui.tableWidgetSignalEvent, &FramRemoteOperationEvents::customContextMenuRequested, [m_menu](const QPoint& pos) {
        m_menu->exec(QCursor::pos());
        });
}

void FramRemoteOperationEvents::InitialCombox()
{
    ui.comboBoxOrderCodeType->setView(new QListView());
    ui.comboxSeparator->setView(new QListView());
}

void FramRemoteOperationEvents::TableWidgetRemoteOperationClicked(int row, int col)
{
    QString value = ui.tableWidgetRemoteOperation->item(row, 0)->text();
    if (IsJudgeRemoteOpterateName(value)) {
        ui.lineEditRemoteOpterateName->setText(value);
        SelectRemoteOpterateSetParamUI(value);
    }
    else {
        QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "不存在该远程操作!");
    }
}

void FramRemoteOperationEvents::GetExterHardwareDialog(QDialog* dialog)
{
    framCameraSetParam = dialog;
    ReloadRemoteOpterate();
}

void FramRemoteOperationEvents::SetFramTaskFlowViewWidget(QWidget* qwidget)
{
    framTaskFlowViewWidget = qwidget;
}

void FramRemoteOperationEvents::AddRemoteOpterate()
{
    QString device_name = ui.comboxCommDeviceName->currentText();
    bool ok; QString remote_name;
    int count = ui.tableWidgetRemoteOperation->rowCount();
    loop: remote_name = QInputDialog::getText(this, tr("远程操作名称"),
        tr("请输入自定义名称"), QLineEdit::Normal, QString("远程操作" + QString::number(remoteOpterateIndex + 1)), &ok);

    if (IsJudgeRemoteOpterateName(remote_name)) {
        QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "远程操作重命名!");
        goto loop;
    }
    if (!ok)
    return;

QTableWidgetItem* item_type = new QTableWidgetItem(remote_name);
ui.tableWidgetRemoteOperation->setRowCount(count + 1);  //设置行数
item_type->setFlags(item_type->flags() & (~Qt::ItemIsEditable));  //设置列不可编辑	
ui.tableWidgetRemoteOperation->setItem(count, 0, item_type);

remoteOpterateIndex = remoteOpterateIndex + 1;
RemoteOpterateParam param;
param.commDeviceName = device_name;
param.remoteOpeterName = remote_name;
qMapRemoteOpterateParam.insert(remoteOpterateIndex, param);
ui.lineEditRemoteOpterateName->setText(remote_name);
emit SendLogInfoSynSig(device_name + ": -- " + remote_name + "-- 添加远程操作 ", LOGTYPE::INFO);
}

void FramRemoteOperationEvents::MenuActionsSlot()
{
    QAction* action = (QAction*)sender();
    QString action_text = action->text();
    int row_index = ui.tableWidgetSignalEvent->currentRow();

    if (action_text == "删除") {
        if (row_index != -1) {
            auto item = ui.tableWidgetSignalEvent->item(row_index, 0);
            QString value = item->text();
            ui.tableWidgetSignalEvent->removeRow(row_index);
            DeleteSignaleEvnet(value);
        }
    }
    else {
        AddSignaleEvnet(action_text);
    }
}

void FramRemoteOperationEvents::AddSignaleEvnet(const QString& action_text)
{
    int count = ui.tableWidgetSignalEvent->rowCount();
    ui.tableWidgetSignalEvent->setRowCount(count + 1);  //设置行数

    QTableWidgetItem* name_item = new QTableWidgetItem("Out" + QString::number(count + 1));
    name_item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    QTableWidgetItem* type_item = new QTableWidgetItem();
    type_item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    type_item->setFlags(type_item->flags() & (~Qt::ItemIsEditable));  //设置列不可编辑	
    QComboBox* combox_datatype = nullptr;
    CreateComboBoxDataType(combox_datatype);
    ui.tableWidgetSignalEvent->setItem(count, 0, name_item);
    ui.tableWidgetSignalEvent->setItem(count, 1, type_item);
    ui.tableWidgetSignalEvent->setCellWidget(count, 1, combox_datatype);

    if (action_text == "Int") {
        type_item->setText("Int");
        combox_datatype->setCurrentIndex(0);
    }
    else if (action_text == "Float") {
        type_item->setText("Float");
        combox_datatype->setCurrentIndex(1);
    }
    else if (action_text == "String") {
        type_item->setText("String");
        combox_datatype->setCurrentIndex(2);
    }

    QComboBox* combox_single_property = nullptr;
    CreateComboBoxSignalProperty(combox_single_property);
    QTableWidgetItem* single_type_item = new QTableWidgetItem();
    single_type_item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    single_type_item->setFlags(single_type_item->flags() & (~Qt::ItemIsEditable));  //设置列不可编辑	
    ui.tableWidgetSignalEvent->setItem(count, 2, single_type_item);
    ui.tableWidgetSignalEvent->setCellWidget(count, 2, combox_single_property);

    QComboBox* combox_single_event = new QComboBox();
    QTableWidgetItem* single_event_item = new QTableWidgetItem();
    single_event_item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    single_event_item->setFlags(single_event_item->flags() & (~Qt::ItemIsEditable));  //设置列不可编辑	
    ui.tableWidgetSignalEvent->setItem(count,3, single_event_item);
    ui.tableWidgetSignalEvent->setCellWidget(count, 3, combox_single_event);

    QString qss = ("QComboBox{  font-size: 14px; color: #000000; font-style: normal ;  font-weight: bold; } \
						  	  QComboBox QAbstractItemView{ outline: 2px solid gray; }\
                              QComboBox QAbstractItemView::item{height: 30px;  padding-left:3px;  }");
    combox_single_event->setStyleSheet(qss);
    combox_single_event->setView(new QListView());
    connect(combox_single_property, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this, combox_single_event](int index) {
        combox_single_event->clear();
        QMap<SignalProperty, QList<QString>> map_signal_property_name;
        this->GetMapSignalPropertyName(map_signal_property_name);
        SignalProperty signal_property = SignalProperty(index);
        switch (signal_property)   {
        case SIGDATA: {
            combox_single_event->clear();
        }
            break;
        case SIGCAMERA: {
            auto cameras_name = map_signal_property_name[SignalProperty::SIGCAMERA];
            combox_single_event->addItems(cameras_name);
            }
            break;
        case SIGFLOW: {
            auto flows_name = map_signal_property_name[SignalProperty::SIGFLOW];
            combox_single_event->addItems(flows_name);
            }
            break;
        default:
            break;
        }
     });
}

void FramRemoteOperationEvents::DeleteSignaleEvnet(const QString& value)
{
    QString remote_opterate_name = ui.lineEditRemoteOpterateName->text();
    RemoteOpterateParam* ptr_param{ nullptr };
    for (auto iter = qMapRemoteOpterateParam.begin(); iter != qMapRemoteOpterateParam.end(); ++iter) {
        int key = iter.key();
        RemoteOpterateParam param = iter.value();
        if (param.remoteOpeterName == remote_opterate_name) {
            ptr_param = &(iter.value());
        }
    }
    QVector<SignalParseParam>* ptr_qv_signal_param = &(ptr_param->qvSignalParseParam);
    for (int i = 0; i < ptr_qv_signal_param->size(); ) {
        auto signal_name = ptr_qv_signal_param->at(i).signalName;
        if (signal_name == value)   {
            ptr_qv_signal_param->removeAt(i);
        }
        else
            i++;
    }
}

void FramRemoteOperationEvents::RemoteOperationCompleteBind()
{
    QString remote_opterate_name = ui.lineEditRemoteOpterateName->text();
    if (!IsJudgeRemoteOpterateName(remote_opterate_name)) {
        QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "不存在该远程操作!");
        return;
    }
    //标识码
   QString  code_address = ui.lineEditorderCodeAddress->text();
   if (code_address.isEmpty()) {
       QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "识别码为空，请填写");
       return;
   }

    RemoteOpterateParam* ptr_param {nullptr};
    for (auto iter = qMapRemoteOpterateParam.begin(); iter != qMapRemoteOpterateParam.end(); ++iter) {
        int key = iter.key();
        RemoteOpterateParam param = iter.value();
        if (param.remoteOpeterName == remote_opterate_name) {
            ptr_param = &(iter.value());
        }
    }

    ptr_param->commDeviceName = ui.comboxCommDeviceName->currentText();
    ptr_param->remoteOpeterName = remote_opterate_name;
    ptr_param->isDetectLength = ui.checkBoxIs->isChecked();
    ptr_param->remoteCodeAddress = code_address;
    ptr_param->textLength = ui.spinBoxTextLength->value();
    ptr_param->remoteType = SignalType(ui.comboBoxOrderCodeType->currentIndex());
    ptr_param->separatorType = ui.comboxSeparator->currentText();
    ptr_param->qvSignalParseParam.clear();

    for (int i = 0; i < ui.tableWidgetSignalEvent->rowCount(); i++) {
        SignalParseParam signal_param;
        auto item0 = ui.tableWidgetSignalEvent->item(i, 0);
        auto item1 = ui.tableWidgetSignalEvent->item(i, 1);
        auto item2 = ui.tableWidgetSignalEvent->item(i, 2);
        auto item3 = ui.tableWidgetSignalEvent->item(i, 3);
        auto item_char = ui.tableWidgetSignalEvent->item(i, 4);
        QComboBox* data_type_item = dynamic_cast<QComboBox*>(ui.tableWidgetSignalEvent->cellWidget(i, 1));
        int signal_type_index = data_type_item->currentIndex();

        QComboBox* combox_single_property = dynamic_cast<QComboBox*>(ui.tableWidgetSignalEvent->cellWidget(i, 2));
        int single_property_index = combox_single_property->currentIndex();

        QComboBox* combox_single_event = dynamic_cast<QComboBox*>(ui.tableWidgetSignalEvent->cellWidget(i, 3));
        QString signal_event_current_name = combox_single_event->currentText();

        signal_param.signalName = item0->text();
        signal_param.signalType = SignalType(signal_type_index);
        signal_param.signalProperty = SignalProperty(single_property_index);
        signal_param.triggerObject = signal_event_current_name;
        signal_param.triggerChar =  item_char->text();
        ptr_param->qvSignalParseParam.push_back(signal_param);
        QString key = remote_opterate_name + "-" + signal_param.signalName ;
        NewExDeviceDataQueue(key, signal_param.signalType, signal_param.signalProperty);
    }
}

void FramRemoteOperationEvents::NewExDeviceDataQueue(const QString& name, const SignalType type, const SignalProperty property)
{
    if (property != SignalProperty::SIGFLOW) {
        return;
    }
    ///添加相机设备
    //StaticGlobalVariable::ptrToolNodeDataInteract->MapInserExDevice(name);
}

void FramRemoteOperationEvents::SelectRemoteOpterateSetParamUI(const QString& name)
{
    RemoteOpterateParam select_param;
    for (auto iter = qMapRemoteOpterateParam.constBegin(); iter != qMapRemoteOpterateParam.constEnd(); ++iter) {
        int key = iter.key();
        RemoteOpterateParam param = iter.value();
        if (param.remoteOpeterName == name) {
            select_param = param;
        }
    }

    ui.comboxCommDeviceName->setCurrentText(select_param.commDeviceName);
    ui.lineEditorderCodeAddress->setText(select_param.remoteCodeAddress);
    SignalType remote_type = select_param.remoteType;
    ui.comboBoxOrderCodeType->setCurrentIndex(int(remote_type));

    ui.spinBoxTextLength->setValue(select_param.textLength);
    ui.checkBoxIs->setChecked(select_param.isDetectLength);

    if (select_param.separatorType == ";") {
        ui.comboxSeparator->setCurrentIndex(0);
    }
    else if (select_param.separatorType == ",") {
        ui.comboxSeparator->setCurrentIndex(1);
    }
    else if (select_param.separatorType == "-") {
        ui.comboxSeparator->setCurrentIndex(2);
    }

    ui.tableWidgetSignalEvent->clearContents();
    ui.tableWidgetSignalEvent->setRowCount(0);

    for (size_t i = 0; i < select_param.qvSignalParseParam.size(); i++){
        SignalParseParam signal_param = select_param.qvSignalParseParam[i];
        SignalEventParamShow(signal_param);
    }
}

void FramRemoteOperationEvents::SignalEventParamShow(const SignalParseParam& signal_param)
{
    int count = ui.tableWidgetSignalEvent->rowCount();
    ui.tableWidgetSignalEvent->setRowCount(count + 1);  //设置行数
    QTableWidgetItem* name_item = new QTableWidgetItem(signal_param.signalName);
    name_item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    ui.tableWidgetSignalEvent->setItem(count, 0, name_item);

    QTableWidgetItem* item_char = new QTableWidgetItem(signal_param.triggerChar);
    item_char->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    ui.tableWidgetSignalEvent->setItem(count, 4, item_char);

    QComboBox* combox_datatype = nullptr;
    CreateComboBoxDataType(combox_datatype);
    QTableWidgetItem* data_type_item = new QTableWidgetItem();
    data_type_item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    data_type_item->setFlags(data_type_item->flags() & (~Qt::ItemIsEditable));  //设置列不可编辑	
    ui.tableWidgetSignalEvent->setItem(count, 1, data_type_item);
    ui.tableWidgetSignalEvent->setCellWidget(count, 1, combox_datatype);
    combox_datatype->setCurrentIndex(int(signal_param.signalType));

    QComboBox* combox_single_property = nullptr;
    CreateComboBoxSignalProperty(combox_single_property);
    QTableWidgetItem* single_type_item = new QTableWidgetItem();
    single_type_item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    single_type_item->setFlags(single_type_item->flags() & (~Qt::ItemIsEditable));  //设置列不可编辑	
    ui.tableWidgetSignalEvent->setItem(count, 2, single_type_item);
    ui.tableWidgetSignalEvent->setCellWidget(count, 2, combox_single_property);

    QComboBox* combox_single_event = new QComboBox();
    QTableWidgetItem* single_event_item = new QTableWidgetItem();
    single_event_item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    single_event_item->setFlags(single_event_item->flags() & (~Qt::ItemIsEditable));  //设置列不可编辑	
    ui.tableWidgetSignalEvent->setItem(count, 3, single_event_item);
    ui.tableWidgetSignalEvent->setCellWidget(count, 3, combox_single_event);
    QString qss = ("QComboBox{  font-size: 14px; color: #000000; font-style: normal ;  font-weight: bold; } \
						  	  QComboBox QAbstractItemView{ outline: 2px solid gray; }\
                              QComboBox QAbstractItemView::item{height: 30px;  padding-left:3px;  }");
    combox_single_event->setStyleSheet(qss);
    combox_single_event->setView(new QListView());
    connect(combox_single_property, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this, signal_param, combox_single_event](int index) {
        combox_single_event->clear();
        QMap<SignalProperty, QList<QString>> map_signal_property_name;
        this->GetMapSignalPropertyName(map_signal_property_name);
        SignalProperty signal_property = SignalProperty(index);
        switch (signal_property) {
        case SIGDATA: {
            combox_single_event->clear();
        }
                    break;
        case SIGCAMERA: {
            auto cameras_name = map_signal_property_name[SignalProperty::SIGCAMERA];
            combox_single_event->addItems(cameras_name);
        }
                      break;
        case SIGFLOW: {
            auto flows_name = map_signal_property_name[SignalProperty::SIGFLOW];
            combox_single_event->addItems(flows_name);
        }
                    break;
        default:
            break;
        }

        SignalProperty current_signal_property = SignalProperty(signal_param.signalProperty);
        QString trigger_name = signal_param.triggerObject;
        switch (current_signal_property) {
        case SIGDATA: {
            combox_single_event->clear();
        }
            break;
        case SIGCAMERA: {
            auto cameras_name = map_signal_property_name[SignalProperty::SIGCAMERA];
            int index  = cameras_name.count(trigger_name);
            if (index == 0) {
                QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", trigger_name + " : 相机不存在，请检查后选择！");
              //  return;
            }
            combox_single_event->setCurrentText(trigger_name);      
            }
            break;
        case SIGFLOW: {
            auto flows_name = map_signal_property_name[SignalProperty::SIGFLOW];
            int index = flows_name.count(trigger_name);
            if (index == 0) {
                QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", trigger_name + " : 流程不存在，请检查后选择！");
           //     return;
            }
            combox_single_event->setCurrentText(trigger_name);
            }
            break;
        default:
            break;
        }
        });

    combox_single_property->setCurrentIndex(int(signal_param.signalProperty));
}

bool FramRemoteOperationEvents::IsJudgeRemoteOpterateName(const QString& name)
{
    for (auto iter = qMapRemoteOpterateParam.constBegin(); iter != qMapRemoteOpterateParam.constEnd(); ++iter) {
        int key = iter.key();
        RemoteOpterateParam param = iter.value();
        if (param.remoteOpeterName == name) {
            return true;
        }
    }
    return false;
}

void FramRemoteOperationEvents::ReloadRemoteOpterate()
{
    //FramExterHardwareSetting* fram_exter_hardware = dynamic_cast<FramExterHardwareSetting*>(framCameraSetParam);
    //QList<QString>device_names = fram_exter_hardware->GetAllCommunicateDeviceName();
    //ui.comboxCommDeviceName->clear();
    //ui.comboxCommDeviceName->addItems(device_names);
}

void FramRemoteOperationEvents::ReloadCameraDeviceNames()
{
   /* FramExterHardwareSetting* fram_exter_hardware = dynamic_cast<FramExterHardwareSetting*>(framCameraSetParam);
    QList<QString>device_names = fram_exter_hardware->GetAllCameraDeviceName();*/
}

void FramRemoteOperationEvents::GetMapSignalPropertyName(QMap<SignalProperty, QList<QString>>& map_name)
{
    //FramExterHardwareSetting* fram_exter_hardware = dynamic_cast<FramExterHardwareSetting*>(framCameraSetParam);
    //QList<QString> camera_names = fram_exter_hardware->GetAllCameraDeviceName();
    //map_name.insert(SignalProperty::SIGCAMERA, camera_names);

    //auto fram_task_flow  = dynamic_cast<FramTaskFlowViewWidget*>(framTaskFlowViewWidget);
    //QList<QString> flow_names = fram_task_flow->GetAllTaskFlowName();
    //map_name.insert(SignalProperty::SIGFLOW, flow_names);
}

void FramRemoteOperationEvents::CreateComboBoxDataType(QComboBox*& combox)
{
    if (combox == nullptr) {
        combox = new QComboBox();
    }
    QString qss = ("QComboBox{  font-size: 14px; color: #000000; font-style: normal ;  font-weight: bold; } \
						  	  QComboBox QAbstractItemView{ outline: 2px solid gray; }\
                              QComboBox QAbstractItemView::item{height: 30px;  padding-left:3px;  }");
    combox->setStyleSheet(qss);
    combox->setView(new QListView());
    combox->addItem("Int");
    combox->addItem("Float");
    combox->addItem("String");
}

void FramRemoteOperationEvents::CreateComboBoxSignalProperty(QComboBox*& combox)
{
    if (combox == nullptr) {
        combox = new QComboBox();
    }
    QString qss = ("QComboBox{  font-size: 14px; color: #000000; font-style: normal ;  font-weight: bold; } \
						  	  QComboBox QAbstractItemView{ outline: 2px solid gray; }\
                              QComboBox QAbstractItemView::item{height: 30px;  padding-left:3px;  }");
    combox->setStyleSheet(qss);
    combox->setView(new QListView());

    combox->addItem("数据信号");
    combox->addItem("相机信号");
    combox->addItem("流程信号");
}

QJsonObject FramRemoteOperationEvents::SerializeRemoteOperationEvents()
{
    emit SendLogInfoSynSig("开始远程操作序列化", LOGTYPE::INFO);
    QJsonObject remote_opterate_jsons;
    QJsonArray remote_opterate_array;
    for (auto iter = qMapRemoteOpterateParam.constBegin(); iter != qMapRemoteOpterateParam.constEnd(); ++iter) {
        int key = iter.key();
        RemoteOpterateParam opterate_param = iter.value();
        QJsonObject remote_opterate_json;
        remote_opterate_json["Key"] = key;
        QJsonObject param_json;
        SerializeParament(param_json, "OpterateParam", opterate_param);
        remote_opterate_json["Value"] = param_json;
        auto signals_parse_param = opterate_param.qvSignalParseParam;
        QJsonArray signals_param_array;
        for (auto signal_param : signals_parse_param){
            QJsonObject signle_json;
            SerializeParament(signle_json, "SignalParam", signal_param);
            signals_param_array.append(signle_json);
        }
        remote_opterate_json["SignalsParam"] = signals_param_array;
        remote_opterate_array.append(remote_opterate_json);

        emit SendLogInfoSynSig("远程设备名：" + opterate_param.remoteOpeterName + " - 序列化完成 ", LOGTYPE::INFO);
    }
    remote_opterate_jsons["RemoteOperation"] = remote_opterate_array;
    emit SendLogInfoSynSig("开始远程操作序列化结束", LOGTYPE::INFO);
    return remote_opterate_jsons;
}

void FramRemoteOperationEvents::DeserializeRemoteOperationEvents(QJsonObject const& devices_json)
{
    emit SendLogInfoSynSig("开始远程操作序反列化", LOGTYPE::INFO);
    QJsonArray remote_opterate_array = devices_json["RemoteOperation"].toArray();
    for (QJsonValue remote_opterate : remote_opterate_array) {
        int key = remote_opterate["Key"].toInt();
        QJsonObject  param_json = remote_opterate["Value"].toObject();
        QString param_json_str =  param_json["OpterateParam"].toString();
        RemoteOpterateParam opterate_param;
        ExserializeFunciton(param_json_str, opterate_param);
        QJsonArray signals_param_array = remote_opterate["SignalsParam"].toArray();    
        for (QJsonValue signals_param_json : signals_param_array){
           auto  signals_param_str = signals_param_json["SignalParam"].toString();
           SignalParseParam signal_param;
           ExserializeFunciton(signals_param_str, signal_param);
           opterate_param.qvSignalParseParam.append(signal_param);

           QString key = opterate_param.remoteOpeterName + ":" + signal_param.signalName;
           NewExDeviceDataQueue(key, signal_param.signalType, signal_param.signalProperty);
        }

        emit SendLogInfoSynSig("远程设备名：" + opterate_param.remoteOpeterName + " - 反序列化完成 ", LOGTYPE::INFO);
        qMapRemoteOpterateParam.insert(key, opterate_param);
    }

    for (auto iter = qMapRemoteOpterateParam.constBegin(); iter != qMapRemoteOpterateParam.constEnd(); ++iter) {
        int key = iter.key();
        remoteOpterateIndex++;
        RemoteOpterateParam opterate_param = iter.value();
        int count = ui.tableWidgetRemoteOperation->rowCount();
        QTableWidgetItem* item_type = new QTableWidgetItem(opterate_param.remoteOpeterName);
        ui.tableWidgetRemoteOperation->setRowCount(count + 1);  //设置行数
        item_type->setFlags(item_type->flags() & (~Qt::ItemIsEditable));  //设置列不可编辑	
        ui.tableWidgetRemoteOperation->setItem(count, 0, item_type);
    }

    SelectRemoteOpterateSetParamUI(qMapRemoteOpterateParam.constBegin().value().remoteOpeterName);
}

template<class T>
inline void FramRemoteOperationEvents::SerializeParament(QJsonObject& json_object, const QString& name, const T& param)
{
    QByteArray data_array;
    QDataStream stream(&data_array, QIODevice::WriteOnly);
    stream << param;
    QString str = QString(data_array.toHex());
    json_object[name] = str;
}

template<class T>
inline void FramRemoteOperationEvents::ExserializeFunciton(const QString& json, T& param)
{
    QByteArray by = QByteArray::fromHex(json.toLatin1());
    QDataStream stream(&by, QIODevice::ReadOnly);
    stream >> param;
}