#include "FramLogicalOperation.h"
#include <QDesktopWidget>
#include <QListView>  
#include <QStandardItemModel>  

//该节点通过判断一个或多个模型输出的结果，输出逻辑运算结果

FramLogicalOperation::FramLogicalOperation(QWidget* parent)
    : ToolInterface(parent)
{
    ui.setupUi(this);
    Initial();
}

FramLogicalOperation::~FramLogicalOperation()
{}

void FramLogicalOperation::Initial()
{
    InitialNodeData();
    InitialImageShow();
    InitialPushButton();
    InitilaToolParamUI();
    InitialExserializeUi();
}

void FramLogicalOperation::InitialNodeData()
{
    //输出与逻辑判断结果
    ptrOrOutVMNodeState = std::make_shared<VMNodeState>();
    ptrOrOutNodeVMNodeState = std::make_shared<BaseNodeData>();
    ptrOrOutNodeVMNodeState->SetCGObjectNodeFlow(ptrOrOutVMNodeState);

    FlowNodeAndName out_node_and_name1;
    out_node_and_name1.nodeName = "或运算结果";
    out_node_and_name1.ptrNodedata = ptrOrOutNodeVMNodeState;
    mapCGObjectFlowOutNodeAndName.insert(std::make_pair(0, out_node_and_name1));

    //输出或逻辑判断结果
    ptrNotOutVMNodeState = std::make_shared<VMNodeState>();
    ptrNotOutNodeVMNodeState = std::make_shared<BaseNodeData>();
    ptrNotOutNodeVMNodeState->SetCGObjectNodeFlow(ptrNotOutVMNodeState);

    FlowNodeAndName out_node_and_name2;
    out_node_and_name2.nodeName = "非运算结果";
    out_node_and_name2.ptrNodedata = ptrNotOutNodeVMNodeState;
    mapCGObjectFlowOutNodeAndName.insert(std::make_pair(1, out_node_and_name2));

    //输入数据
    ptrInNodeMultiInputData = std::make_shared<MultiInputNodeData>();
    ptrInNodeMultiInputData->AddScopeNodeDataType(QtNodes::NodeDataType{ "0","Bool" });
    ptrInNodeMultiInputData->AddScopeNodeDataType(QtNodes::NodeDataType{ "1","Point" });
    ptrInNodeMultiInputData->AddScopeNodeDataType(QtNodes::NodeDataType{ "2","Line" });
    ptrInNodeMultiInputData->AddScopeNodeDataType(QtNodes::NodeDataType{ "3","Rectangle" });
    ptrInNodeMultiInputData->AddScopeNodeDataType(QtNodes::NodeDataType{ "4","Circle" });
    ptrInNodeMultiInputData->AddScopeNodeDataType(QtNodes::NodeDataType{ "100","Vector" });
    ptrInNodeMultiInputData->AddScopeNodeDataType(QtNodes::NodeDataType{ "101","Unordered_map" });
    FlowNodeAndName in_node_and_name;
    in_node_and_name.nodeName = "模型结果";
    in_node_and_name.ptrNodedata = ptrInNodeMultiInputData;
    mapCGObjectFlowInNodeAndName.insert(std::make_pair(0, in_node_and_name));
}

void FramLogicalOperation::InitialPushButton()
{
    connect(ui.pushButtonExecute, &QPushButton::clicked, this, [this]() {
        isSelfRunring = true;
        RunningTool();
        });  //按钮按下后信号发出执行操作
}

void FramLogicalOperation::InitialImageShow()
{
    this->setWindowTitle("逻辑运算");
    this->setWindowIcon(QIcon(":/FramLogicalOperation/Image/LogicalOperation.png"));
    // 去掉问号，只保留关闭
    this->setWindowFlags(Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint);
    this->installEventFilter(this);

    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);
   /* ptrGraphicsViews2d = new QGraphicsViews2d(ui.widgetImageShow);
    QRect rect = ui.widgetImageShow->geometry();
    ptrGraphicsViews2d->setGeometry(QRect(0, 0, rect.width(), rect.height()));*/
    connect(this, &FramLogicalOperation::SendNodeRuningStateSyncSig, this, &FramLogicalOperation::AcceptSelfRunringStateSlot);
}

void FramLogicalOperation::InitilaToolParamUI()
{
    judgeMentClassId = ui.spinBoxJudgementClassId->value();
}

void FramLogicalOperation::InitialExserializeUi()
{

}

int FramLogicalOperation::RunningTool()
{
    std::chrono::high_resolution_clock::time_point tp1 = std::chrono::high_resolution_clock::now();
    RUNNINGSTATE node_state = RUNNINGSTATE::RUNNING_SUCESS;

    std::pair<LOGTYPE, QString> p_log_info;
    QString str_log = "节点: " + QString::number(this->nodeId) + ": ";
    p_log_info.first = LOGTYPE::INFO;
    p_log_info.second = str_log + "判断输入数据状态";
    this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);

    ptrInMultiInputData = ptrInNodeMultiInputData->GetCGObjectNodeFlow<VMVector<PtrVMObject>>();
    float  elapsed_time = 0.0;
    ptrOrOutVMNodeState->state = false;
    ptrNotOutVMNodeState->state = false;

    if (ptrInMultiInputData == nullptr || ptrInMultiInputData->vmVector.empty()) {
        RUNNINGSTATE node_state = RUNNINGSTATE::DATA_NULL;

        p_log_info.first = LOGTYPE::ERRORS;
        p_log_info.second = str_log + "节点输入数据为空，请检查！";
        this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);

        if (isSelfRunring) {
            emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
        }
        else {
            emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
        }
        return -1;
    }

    std::vector<PtrVMObject>().swap(vPtrObjectMultiData);   //清空
    int res = ParseRanderData(vPtrObjectMultiData, ptrInMultiInputData);
    int classCount = 0;
    for (auto it = vPtrObjectMultiData.begin(); it != vPtrObjectMultiData.end(); ++it) {
        auto& ptr_mobject = *it;
        VMDATATYPE cg_type = ptr_mobject->cgType;
        switch (cg_type)
        {
        case VM_RECTANGLE: {
            /*if (ptr_mobject->calssId = functionParam.classId) {
                classCount++;
            }*/
            if (ptr_mobject->calssId == 0) {
                classCount++;
            }
        }
            break;
        default:
            break;
        }
    }

    if (classCount > 0) {
        ptrOrOutVMNodeState->nodeState = true;
        ptrNotOutVMNodeState->nodeState = false;
    }
    else {
        ptrOrOutVMNodeState->nodeState = false;
        ptrNotOutVMNodeState->nodeState = true;
    }

    ptrOrOutVMNodeState->state = true;
    ptrNotOutVMNodeState->state = true;

    std::chrono::high_resolution_clock::time_point tp2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<size_t, std::nano> dur = tp2 - tp1;
    elapsed_time = float(std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());

    QStandardItemModel* model = new QStandardItemModel();
    QStandardItem* item = new QStandardItem(QString("开始记录"));
    model->appendRow(item);

    item = new QStandardItem(QString("或逻辑判断结果为: %1").arg(ptrOrOutVMNodeState->nodeState ? "true" : "false"));
    model->appendRow(item);
    item = new QStandardItem(QString("非逻辑判断结果为: %1").arg(ptrNotOutVMNodeState->nodeState ? "true" : "false"));
    model->appendRow(item);

    ui.listView->setModel(model);
    judgeMentClassId = ui.spinBoxJudgementClassId->value();

    if (isSelfRunring) {
        emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
    }
    else {
        emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
    }
    return 0;
}

int FramLogicalOperation::ParseRanderData(std::vector<PtrVMObject>& v_ptr_object, const PtrVMObject& ptr_mobject)
{
    VMDATATYPE cg_type = ptr_mobject->cgType;
    bool state = ptr_mobject->state;
    ///可以修改，针对不同vector
    if (state == false) {
        return -1;
    }

    switch (cg_type)
    {
    case VM_NULL:
        break;
    case VM_BOOL: {
        v_ptr_object.emplace_back(ptr_mobject);
    }
                break;
    case VM_POINT: {
        v_ptr_object.emplace_back(ptr_mobject);
    }
                 break;
    case VM_RECTANGLE: {
        v_ptr_object.emplace_back(ptr_mobject);
    }
                     break;
    case VM_POLYGON: {
        v_ptr_object.emplace_back(ptr_mobject);
    }
                   break;
    case VM_MAT:
        break;
    case VM_VECTOR: {
        auto vm_p = std::static_pointer_cast<VMVector<PtrVMObject>>(ptr_mobject);
        for (size_t i = 0; i < vm_p->vmVector.size(); i++) {
            PtrVMObject ptr_object = vm_p->vmVector[i];
            ParseRanderData(v_ptr_object, ptr_object);
        }
    }
                  break;
    case VM_UNORDERED_MAP: {
        auto ptr_vmvmap = std::static_pointer_cast<VMUnorderedMap<int, PtrVMObject>>(ptr_mobject);
        for (size_t i = 0; i < ptr_vmvmap->vmMap.size(); i++) {
            PtrVMObject ptr_object = ptr_vmvmap->vmMap[i];
            ParseRanderData(v_ptr_object, ptr_object);
        }
    }
                         break;
    default:
        break;
    }
    return 0;
}

void FramLogicalOperation::AcceptNodeBindingSucceedSlot(const QtNodes::PortIndex port_index)
{
    //auto iter = mapCGObjectFlowInNodeAndName.find(port_index);
    //if (iter == mapCGObjectFlowInNodeAndName.end())
    //    return;

    //auto ptr = std::static_pointer_cast<BaseNodeData>(iter->second.ptrNodedata);
    //auto vm_object = ptr->GetCGObjectNodeFlow<VMObject>();
    //QString name = vm_object->nodeDataType.name;
    //QtNodes::NodeId node_id = ptr->GetNodeId();
    ////ui.qLineEditNodeSource->setText(QString::number(node_id) + ": " + name);
    if (port_index == 1) {
        auto iter = mapCGObjectFlowInNodeAndName.find(port_index);
        if (iter == mapCGObjectFlowInNodeAndName.end())
            return;

        auto ptr = std::static_pointer_cast<BaseNodeData>(iter->second.ptrNodedata);
        auto vm_object = ptr->GetCGObjectNodeFlow<VMObject>();
        QString name = vm_object->nodeDataType.name;
        QtNodes::NodeId node_id = ptr->GetNodeId();
    }
    else {
        auto iter = mapCGObjectFlowInNodeAndName.find(port_index);
        if (iter == mapCGObjectFlowInNodeAndName.end())
            return;
        auto ptr = std::static_pointer_cast<MultiInputNodeData>(iter->second.ptrNodedata);
        auto vmvector_object = ptr->GetCGObjectNodeFlow<VMVector<PtrVMObject>>();
        for (size_t i = 0; i < vmvector_object->vmVector.size(); i++) {
            PtrVMObject ptr_object = vmvector_object->vmVector[i];
            QString name = ptr_object->nodeDataType.name;
            QtNodes::NodeId node_id = ptr_object->nodeID;;
        }
    }
}


void FramLogicalOperation::AcceptSelfRunringStateSlot(const float& time, const RUNNINGSTATE& run_state)
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

    QStandardItemModel* model = new QStandardItemModel();
    QStandardItem* item = new QStandardItem(QString("开始记录"));
    model->appendRow(item);

    item = new QStandardItem(QString("或逻辑判断结果为: %1").arg(ptrOrOutVMNodeState->nodeState ? "true" : "false"));
    model->appendRow(item);
    item = new QStandardItem(QString("非逻辑判断结果为: %1").arg(ptrNotOutVMNodeState->nodeState ? "true" : "false"));
    model->appendRow(item);

    ui.listView->setModel(model);
}


int FramLogicalOperation::Execute()
{
    isSelfRunring = false;
    int res = this->RunningTool();
    return res;
}

int FramLogicalOperation::OnceExecute()
{
    isSelfRunring = false;
    std::thread thrad = std::thread(&FramLogicalOperation::RunningTool, this);
    thrad.detach();
    return 0;
}

void FramLogicalOperation::SetNodeId(const int node_id)
{
    this->nodeId = node_id;
    ptrOrOutVMNodeState->nodeID = this->nodeId;
    std::vector<PtrVMObject> v_ptrvmobject;
    v_ptrvmobject.emplace_back(ptrOrOutVMNodeState);
    ptrMapNodesOutData->insert(std::make_pair(this->nodeId, v_ptrvmobject));
}

QJsonObject FramLogicalOperation::SerializeFunciton()
{
    QJsonObject model_json;

    functionParam.classId = judgeMentClassId;

    SerializeTParament(model_json, "FunctionParam", functionParam);
    SerializeTParament(model_json, "UiParam", uiParam);
    return model_json;
}

void FramLogicalOperation::ExserializeFunciton(const QJsonObject& json)
{
    QString fun_json = json["FunctionParam"].toString();
    QString ui_json = json["UiParam"].toString();

    ExserializeTFunciton(fun_json, functionParam);
    ExserializeTFunciton(ui_json, uiParam);

    judgeMentClassId = functionParam.classId;

    emit SendExserializeFinishSig();
}
