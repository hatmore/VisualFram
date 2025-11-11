#include <QDesktopWidget>
#include <QFileDialog>
#include <QComboBox>
#include"FramImageActionOrder.h"
#include <QListView>  
#include <QStandardItemModel>  
#include <fstream>
#include <iostream>

FramImageActionOrder::FramImageActionOrder(QWidget* parent)
    : ToolInterface(parent)
{
    ui.setupUi(this);
    Initial();
}

FramImageActionOrder::~FramImageActionOrder()
{}

void FramImageActionOrder::Initial()
{
    InitialNodeData();
    InitialImageShow();
    InitialPushButton();
    InitilaToolParamUI();
    InitialExserializeUi();

    ptrImageActionOrderAlgorithm= std::make_shared<ImageActionOrderAlgorithm>();
}

void FramImageActionOrder::InitialNodeData()
{
    // 输出   
    ptrOutVec = std::make_shared<VMUnorderedMap<int, PtrVMNodeState>>();
    ptrOutNodeData = std::make_shared<BaseNodeData>();
    ptrOutNodeData->SetCGObjectNodeFlow(ptrOutVec);

    FlowNodeAndName out_node_and_name;
    out_node_and_name.nodeName = "动作顺序";
    out_node_and_name.ptrNodedata = ptrOutNodeData;
    mapCGObjectFlowOutNodeAndName.insert(std::make_pair(0, out_node_and_name));

    // 输入1
    ptrInNodeData = std::make_shared<BaseNodeData>();
    ptrInNodeData->AddScopeNodeDataType(QtNodes::NodeDataType{ "101","UnorderedMap" });
    FlowNodeAndName in_node_and_name;
    in_node_and_name.nodeName = "ROI";
    in_node_and_name.ptrNodedata = ptrInNodeData;
    mapCGObjectFlowInNodeAndName.insert(std::make_pair(0, in_node_and_name));

    // 输入2
    ptrInNodeModelData = std::make_shared<BaseNodeData>();
    ptrInNodeModelData->AddScopeNodeDataType(QtNodes::NodeDataType{ "101","UnorderedMap" });
    FlowNodeAndName in_node_and_name2;
    in_node_and_name2.nodeName = "模型";
    in_node_and_name2.ptrNodedata = ptrInNodeModelData;
    mapCGObjectFlowInNodeAndName.insert(std::make_pair(1, in_node_and_name2));

    // 输入3
    ptrInNodeVMNodeState = std::make_shared<BaseNodeData>();
    ptrInNodeVMNodeState->AddScopeNodeDataType(QtNodes::NodeDataType{ "0","Bool" });
    FlowNodeAndName in_node_and_name3;
    in_node_and_name3.nodeName = "使能";
    in_node_and_name3.ptrNodedata = ptrInNodeVMNodeState;
    mapCGObjectFlowInNodeAndName.insert(std::make_pair(2, in_node_and_name3));
}

void FramImageActionOrder::InitialPushButton()
{
    connect(ui.m_exe_btn, &QPushButton::clicked, this, [this]() {
        isSelfRunring = true;
        RunningTool();
        });  //按钮按下后信号发出执行操作
}

void FramImageActionOrder::InitialImageShow()
{
    this->setWindowTitle("动作顺序");
    this->setWindowIcon(QIcon(":/FramImageFilter/Image/actionOrder.png"));
    // 去掉问号，只保留关闭
    this->setWindowFlags(Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint);
    this->installEventFilter(this);
    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);
    ptrGraphicsViews2d = new QGraphicsViews2d(ui.m_show_widget);
    QRect rect = ui.m_show_widget->geometry();
    ptrGraphicsViews2d->setGeometry(QRect(0, 0, rect.width(), rect.height()));
    connect(this, &FramImageActionOrder::SendNodeRuningStateSyncSig, this, &FramImageActionOrder::AcceptSelfRunringStateSlot);
}

void FramImageActionOrder::InitilaToolParamUI()
{
    ui.m_spinBox->setValue(IouValue);
    connect(ui.m_horizontalSlider, &QSlider::valueChanged, this, [=](int value) {
             IouValue = value;
             ui.m_spinBox->setValue(value);
        });
 
    connect(ui.m_spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value) {
        ui.m_horizontalSlider->setValue(value);
        });

}

void FramImageActionOrder::InitialExserializeUi()
{
     connect(this, &FramImageActionOrder::SendExserializeFinishSig, this, [this]() {
         ui.m_horizontalSlider->setValue(functionParam.IouParam);
         ui.m_spinBox->setValue(functionParam.IouParam);
         });
}


int FramImageActionOrder::RunningTool()
{
    std::chrono::high_resolution_clock::time_point tp1 = std::chrono::high_resolution_clock::now();
    RUNNINGSTATE node_state = RUNNINGSTATE::RUNNING_SUCESS;
    float  elapsed_time = 0.0;

    // 第一次运行时设置算法类的日志队列
    static bool logQueueInitialized = false;
    if (!logQueueInitialized) {
        ptrImageActionOrderAlgorithm->SetLogQueue(toolNodeDataInteract->ptrQueueNodeLogData);
        logQueueInitialized = true;
    }

    std::pair<LOGTYPE, QString>log_qstr;
    log_qstr.first = LOGTYPE::INFO;
    log_qstr.second = this->selfToolName + "--开始进行动作顺序检测";
    toolNodeDataInteract->ptrQueueNodeLogData->push(log_qstr);

    ptrInVMNodeState = ptrInNodeVMNodeState->GetCGObjectNodeFlow<VMNodeState>();
    ptrInVMIds = ptrInNodeData->GetCGObjectNodeFlow<VMUnorderedMap<int, PtrVMVector<VMRectangle>>>();//ROI信息
 
    ptrOutVec->state = false;

    if (ptrInVMIds==nullptr||ptrInVMIds->state==false)
    {
        RUNNINGSTATE node_state = RUNNINGSTATE::DATA_NULL;
        if (isSelfRunring) {
            emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
        }
        else {
            emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
        }
        return -1;
    }

    
     //初始化ROI   
    std::vector<ROI>rois = ptrImageActionOrderAlgorithm->InitRoi(ptrInVMIds);
    //初始化模型
    uint64_t timestamp = 0;
    std::vector<DetectedObject>Mymodel = ptrImageActionOrderAlgorithm->InitModel(ptrInNodeModelData, timestamp);

    ptrImageActionOrderAlgorithm->SetParam(rois);
    int res = ptrImageActionOrderAlgorithm->RunProcessFrame(ptrOutVec, Mymodel, timestamp, IouValue);


    if (res!=0)
    {
        RUNNINGSTATE node_state = RUNNINGSTATE::RUNNING_FAIL;
        if (isSelfRunring) {
            emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
        }
        else {
            emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
        }
        return 0;
    }

    ptrOutVec->state = true;
    std::chrono::high_resolution_clock::time_point tp2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<size_t, std::nano> dur = tp2 - tp1;
    elapsed_time = float(std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());

    //外部信号为true，清空数据
    if (ptrInVMNodeState != nullptr) {
        if (ptrInVMNodeState->nodeState) {
            if (clearCount > 0) {
                clearCount--;
            }
            else {
                if (ptrOutVec->vmMap.size() > 0) {
                    std::ofstream outFile("G:/PengSiyao/TestData/OrderOutput.txt", std::ios::app);
                    for (auto& [orderId, vmp] : ptrOutVec->vmMap) {
                        outFile << "类别ID: " << orderId << "     动作时间戳: " << vmp->timeStamp << std::endl;
                    }
                    outFile << " " << std::endl;
                    ptrOutVec->vmMap.clear();

                    // 清除检测历史和重置顺序进度
                    ptrImageActionOrderAlgorithm->roiDetectionHistory.clear();
                    ptrImageActionOrderAlgorithm->highestRecordedOrderId = 0;

                    // 添加日志
                    std::pair<LOGTYPE, QString> log_clear;
                    log_clear.first = LOGTYPE::INFO;
                    log_clear.second = this->selfToolName + "--工件离开，清除历史数据，重置顺序检测";
                    toolNodeDataInteract->ptrQueueNodeLogData->push(log_clear);
                }
            }
        }
        else {
            clearCount = 1;
        }
    }

    // 修改列表显示逻辑
    QStandardItemModel* model = new QStandardItemModel();
    QStandardItem* item = new QStandardItem(QString("开始记录"));
    model->appendRow(item);

    if (ptrOutVec != nullptr) {
        for (auto& [order_id, node_states] : ptrOutVec->vmMap) {
            // 直接访问内部的 vmVector
            if (node_states != nullptr) {
                QStandardItem* item = new QStandardItem(
                    "动作:" + QString::number(order_id) +
                    "发生的时间为: " + QString::number(node_states->timeStamp)
                );
                model->appendRow(item);
            }
        }
    }
    ui.listView->setModel(model);

    if (isSelfRunring) {
        emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
    }
    else {
        emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
    }
    return 0;

}


//接受绑定完成的槽函数，用于显示
void FramImageActionOrder::AcceptNodeBindingSucceedSlot(const QtNodes::PortIndex port_index)
{
    auto iter = mapCGObjectFlowInNodeAndName.find(port_index);
    if (iter == mapCGObjectFlowInNodeAndName.end())
        return;

    auto ptr = std::static_pointer_cast<BaseNodeData>(iter->second.ptrNodedata);
    auto vm_object = ptr->GetCGObjectNodeFlow<VMObject>();
    QString name = vm_object->nodeDataType.name;
    QtNodes::NodeId node_id = ptr->GetNodeId();
    //ui.qLineEditNodeSource->setText(QString::number(node_id) + ": " + name);
}


void FramImageActionOrder::AcceptSelfRunringStateSlot(const float& time, const RUNNINGSTATE& run_state)
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
    ui.m_show_lineEdit->setText(str_time + qstr_des);
}


int FramImageActionOrder::Execute()
{
    isSelfRunring = false;
    int res = this->RunningTool();
    return res;
}

int FramImageActionOrder::OnceExecute()
{
    isSelfRunring = false;  
    std::thread thrad = std::thread(&FramImageActionOrder::RunningTool, this);
    thrad.detach();
    return 0;
}

void FramImageActionOrder::SetNodeId(const int node_id)
{
    this->nodeId = node_id;
    ptrOutVec->nodeID = this->nodeId;
    std::vector<PtrVMObject> v_ptrvmobject;
    v_ptrvmobject.emplace_back(ptrOutVec);
    ptrMapNodesOutData->insert(std::make_pair(this->nodeId, v_ptrvmobject));
}

QJsonObject FramImageActionOrder::SerializeFunciton()
{
    QJsonObject model_json;
    functionParam.IouParam = IouValue;  
    SerializeTParament(model_json, "FunctionParam", functionParam);
    SerializeTParament(model_json, "UiParam", uiParam);
    return model_json;
}

void FramImageActionOrder::ExserializeFunciton(const QJsonObject& json)
{
    QString fun_json = json["FunctionParam"].toString();
    QString ui_json = json["UiParam"].toString();
    ExserializeTFunciton(fun_json, functionParam);
    ExserializeTFunciton(ui_json, uiParam);
    if (functionParam.IouParam > 0) {
        IouValue = functionParam.IouParam;
    }
    emit SendExserializeFinishSig();
}