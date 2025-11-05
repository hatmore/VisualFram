#include "FramExternalDeviceRead.h"
#include <QDesktopWidget>

FramExternalDeviceRead::FramExternalDeviceRead(QWidget *parent)
    : ToolInterface(parent)
{
    ui.setupUi(this);
    Initial();
}


FramExternalDeviceRead::~FramExternalDeviceRead()
{
    ptrVMNodeState.reset();
    ptrOutNodeVMNodeState.reset();
    ptrMapQueueDeviceCommunicationData.reset();
    mapCGObjectFlowOutNodeAndName.clear();
    isSelfRunring = false;
}

void FramExternalDeviceRead::Initial()
{
    InitialNodeData();
    InitialImageShow();
    InitialPushButton();
    InitialExserializeUi();
}

void FramExternalDeviceRead::InitialNodeData()
{
    ptrVMNodeState = std::make_shared<VMNodeState>();
    ptrOutNodeVMNodeState = std::make_shared<BaseNodeData>();
    ptrOutNodeVMNodeState->SetCGObjectNodeFlow(ptrVMNodeState);

    FlowNodeAndName flow_node_and_name;
    flow_node_and_name.nodeName = "运行状态";
    flow_node_and_name.ptrNodedata = ptrOutNodeVMNodeState;

    mapCGObjectFlowOutNodeAndName.insert(std::make_pair(0, flow_node_and_name));
}

void FramExternalDeviceRead::InitialImageShow()
{
    this->setWindowTitle("外部设备通讯");
    this->setWindowIcon(QIcon(":/FramExternalDeviceRead/Image/read.png"));
    // 去掉问号，只保留关闭
    this->setWindowFlags(Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint);
    this->installEventFilter(this);


    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);

    connect(this, &FramExternalDeviceRead::SendNodeRuningStateSyncSig, this, &FramExternalDeviceRead::AcceptSelfRunringStateSlot);
}

void FramExternalDeviceRead::InitialPushButton()
{
    QString qss = ("QComboBox{  font-size: 15px; color: #000000; font-style: normal ;  font-weight: bold; } \
						  	  QComboBox QAbstractItemView{ outline: 2px solid gray; }\
                              QComboBox QAbstractItemView::item{height: 30px;  padding-left:3px;  }");
    ui.comboBox->setStyleSheet(qss);

    connect(ui.pushButtonUpdata, &QPushButton::clicked, this, [this]() {
        ptrMapQueueDeviceCommunicationData = ToolInterface::toolNodeDataInteract->ptrMapQueueDeviceCommunicationData;
        if (ptrMapQueueDeviceCommunicationData == nullptr) {
            QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "设备通信数据为空，请先绑定设备通信数据");
            return;
        }
        std::vector<QString> v_keys;
        std::transform(ptrMapQueueDeviceCommunicationData->begin(), ptrMapQueueDeviceCommunicationData->end(), std::back_inserter(v_keys),
            [](const auto& pair) { return pair.first; });

        if (v_keys.empty()) {
            QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "相机为空，请先绑定相机");
            return;
        }
        ui.comboBox->clear();
        for (const auto key : v_keys) {
            ui.comboBox->addItem(key);
        }
        });  //按钮按下后信号发出执行操作

    connect(ui.comboBox, QOverload<const QString&>::of(&QComboBox::currentTextChanged), [this](const QString& str) {
        functionParam.signalName = str;
        });

    connect(ui.pushButtonExecute, &QPushButton::clicked, this, [&]() {
        isSelfRunring = true;
        RunningTool();
        });  //按钮按下后信号发出执行操作
}

void FramExternalDeviceRead::InitialExserializeUi()
{
    connect(this, &FramExternalDeviceRead::SendExserializeFinishSig, this, [this]() {
        ui.comboBox->clear();
        ui.comboBox->addItem(functionParam.signalName);
        });
}

void FramExternalDeviceRead::AcceptSelfRunringStateSlot(const float& time, const RUNNINGSTATE& run_state)
{
    QString qstr_des = "状态: ";
    switch (run_state) {
    case RUNNING_SUCESS: {

        qstr_des = qstr_des + "运行成功";
    }
                       break;
    case RUNNING_FAIL: {
        qstr_des = qstr_des + "运行失败";
    }
                     break;
    case DATA_NULL: {
        qstr_des = qstr_des + "输入数据为空，运行失败";
    }
                  break;
    case PARAM_EINVAL: {
        qstr_des = qstr_des + "参数错误，运行失败";
    }
                     break;
    default:
        break;
    }
    QString str_time = "耗时:" + QString::number(time, 'f', 2) + "ms -- ";
     ui.txtDescribe->setText(str_time + qstr_des);
}

int FramExternalDeviceRead::RunningTool()
{
    std::chrono::high_resolution_clock::time_point tp1 = std::chrono::high_resolution_clock::now();
    RUNNINGSTATE node_state = RUNNINGSTATE::RUNNING_SUCESS;
    float  elapsed_time = 0.0;
    ptrVMNodeState->state = false;

    ptrMapQueueDeviceCommunicationData = ToolInterface::toolNodeDataInteract->ptrMapQueueDeviceCommunicationData;

    auto iter = ptrMapQueueDeviceCommunicationData->find(functionParam.signalName);
    if (iter == ptrMapQueueDeviceCommunicationData->end()) {
        RUNNINGSTATE node_state = RUNNINGSTATE::PARAM_EINVAL;
        if (isSelfRunring) {
            emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
        }
        else {
            emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
        }
        return -1;
    }
    std::shared_ptr < tbb::concurrent_queue<int>> ptr_queue = iter->second;
    int order;
    if (ptr_queue->try_pop(order)) {
        switch (order) {
        case 0: {
            ptrVMNodeState->nodeState = false;
        }
               break;
        case 1: {
            ptrVMNodeState->nodeState = true;
        }
              break;
        default: {
            ptrVMNodeState->nodeState = false;
        }
              break;
        }
  }

    ptrVMNodeState->state = true;
    std::chrono::high_resolution_clock::time_point tp2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<size_t, std::nano> dur = tp2 - tp1;
    elapsed_time = float(std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());

    if (isSelfRunring) {
        emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
    }
    else {
        emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
    }
    return 0;
}

int FramExternalDeviceRead::Execute()
{
    isSelfRunring = false;
    int res = this->RunningTool();
    return res;
}

int FramExternalDeviceRead::OnceExecute()
{
    isSelfRunring = false;
    std::thread thrad = std::thread(&FramExternalDeviceRead::RunningTool, this);
    thrad.detach();
    return 0;
}

void FramExternalDeviceRead::SetNodeId(const int node_id)
{
    this->nodeId = node_id;
    ptrVMNodeState->nodeID = this->nodeId;
    std::vector<PtrVMObject> v_ptrvmobject;
    v_ptrvmobject.emplace_back(ptrVMNodeState);
    ptrMapNodesOutData->insert(std::make_pair(this->nodeId, v_ptrvmobject));
}

QJsonObject FramExternalDeviceRead::SerializeFunciton()
{
    QJsonObject model_json;
    SerializeTParament(model_json, "FunctionParam", functionParam);
    SerializeTParament(model_json, "UiParam", uiParam);
    return model_json;
}

void FramExternalDeviceRead::ExserializeFunciton(const QJsonObject& json)
{
    QString fun_json = json["FunctionParam"].toString();
    QString ui_json = json["UiParam"].toString();

    ExserializeTFunciton(fun_json, functionParam);
    ExserializeTFunciton(ui_json, uiParam);

    emit SendExserializeFinishSig();
}

