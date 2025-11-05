#include "FramResultStateJudgment.h"
#include <QDesktopWidget>
#include <QListView>  
#include <QStandardItemModel>  

//该节点通过判断目标检测模型输出的结果，实现流程的自动开始和结束。也可用于数据发送节点

FramResultStateJudgment::FramResultStateJudgment(QWidget* parent)
    : ToolInterface(parent)
{
    ui.setupUi(this);
    Initial();
}

FramResultStateJudgment::~FramResultStateJudgment()
{}

void FramResultStateJudgment::Initial()
{
    InitialNodeData();
    InitialImageShow();
    InitialPushButton();
    InitilaToolParamUI();
    InitialExserializeUi();
}

void FramResultStateJudgment::InitialNodeData()
{
    //输出与逻辑判断结果
    ptrAndOutVMNodeState = std::make_shared<VMNodeState>();
    ptrAndOutNodeVMNodeState = std::make_shared<BaseNodeData>();
    ptrAndOutNodeVMNodeState->SetCGObjectNodeFlow(ptrAndOutVMNodeState);

    FlowNodeAndName out_node_and_name1;
    out_node_and_name1.nodeName = "与判断结果";
    out_node_and_name1.ptrNodedata = ptrAndOutNodeVMNodeState;
    mapCGObjectFlowOutNodeAndName.insert(std::make_pair(0, out_node_and_name1));

    //输出非逻辑判断结果
    ptrNotOutVMNodeState = std::make_shared<VMNodeState>();
    ptrNotOutNodeVMNodeState = std::make_shared<BaseNodeData>();
    ptrNotOutNodeVMNodeState->SetCGObjectNodeFlow(ptrNotOutVMNodeState);

    FlowNodeAndName out_node_and_name2;
    out_node_and_name2.nodeName = "非判断结果";
    out_node_and_name2.ptrNodedata = ptrNotOutNodeVMNodeState;
    mapCGObjectFlowOutNodeAndName.insert(std::make_pair(1, out_node_and_name2));

    //输入数据
    ptrInNodeDeviceJudgeData = std::make_shared<BaseNodeData>();
    ptrInNodeDeviceJudgeData->AddScopeNodeDataType(QtNodes::NodeDataType{ "101","UnorderedMap" });
    FlowNodeAndName in_node_and_name;
    in_node_and_name.nodeName = "输入数据";
    in_node_and_name.ptrNodedata = ptrInNodeDeviceJudgeData;

    mapCGObjectFlowInNodeAndName.insert(std::make_pair(0, in_node_and_name));

}

void FramResultStateJudgment::InitialPushButton()
{
    connect(ui.pushButtonExecute, &QPushButton::clicked, this, [this]() {
        isSelfRunring = true;
        RunningTool();
        });  //按钮按下后信号发出执行操作
}

void FramResultStateJudgment::InitialImageShow()
{
    this->setWindowTitle("逻辑判断");
    this->setWindowIcon(QIcon(":/FramResultStateJudgment/Image/logicJudgement.png"));
    // 去掉问号，只保留关闭
    this->setWindowFlags(Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint);
    this->installEventFilter(this);


    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);

    ptrGraphicsViews2d = new QGraphicsViews2d(ui.widgetImageShow);
    QRect rect = ui.widgetImageShow->geometry();
    ptrGraphicsViews2d->setGeometry(QRect(0, 0, rect.width(), rect.height()));

    connect(this, &FramResultStateJudgment::SendNodeRuningStateSyncSig, this, &FramResultStateJudgment::AcceptSelfRunringStateSlot);
}

void FramResultStateJudgment::InitilaToolParamUI()
{
    judgementClassId = ui.spinBoxJudgementClassId->value();
}

void FramResultStateJudgment::InitialExserializeUi()
{
    
}

float CalculateIoU(const cv::Rect& rect1, const cv::Rect& rect2) {

    int x1 = std::max(rect1.x, rect2.x);
    int y1 = std::max(rect1.y, rect2.y);
    int x2 = std::min(rect1.x + rect1.width, rect2.x + rect2.width);
    int y2 = std::min(rect1.y + rect1.height, rect2.y + rect2.height);

    int intersection_area = std::max(0, x2 - x1) * std::max(0, y2 - y1);

    if (intersection_area == 0) {
        return 0.0f;
    }
    //考虑到一个大的检测区域可能多个位置都会发生某一种动作
    //通过计算交并比来判断物体是否出现在Roi区域，有点问题，换一种计算方法
    //原来：int union_area = rect1.area() + rect2.area() - intersection_area;
    //计算交集区域与各区域的占比大小，输出最大的那个
    float unionValue1 = static_cast<float>(intersection_area) / rect1.area();
    float unionValue2 = static_cast<float>(intersection_area) / rect2.area();
    return std::max(unionValue1, unionValue2);
}

int FramResultStateJudgment::RunningTool()
{
    std::chrono::high_resolution_clock::time_point tp1 = std::chrono::high_resolution_clock::now();
    RUNNINGSTATE node_state = RUNNINGSTATE::RUNNING_SUCESS;

    std::pair<LOGTYPE, QString> p_log_info;
    QString str_log = "节点: " + QString::number(this->nodeId) + ": ";
    p_log_info.first = LOGTYPE::INFO;
    p_log_info.second = str_log + "判断输入数据状态";
    this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);

    float  elapsed_time = 0.0;
    ptrAndOutVMNodeState->state = false;
    ptrNotOutVMNodeState->state = false;

    // 第一步：识别输入数据类型
    int Idex = 0;
    if (ptrInNodeDeviceJudgeData->GetCGObjectNodeFlow<VMUnorderedMap<int, PtrVMPoint2d>>()) {
        Idex = 1;  // 点类型数据
    }
    else if (ptrInNodeDeviceJudgeData->GetCGObjectNodeFlow<VMUnorderedMap<int, PtrVMVector<PtrVMRectangle>>>()) {
        Idex = 2;  // 矩形类型数据
    }
    else if (ptrInNodeDeviceJudgeData->GetCGObjectNodeFlow<VMUnorderedMap<int, PtrVMPolygon>>()) {
        Idex = 3;  // 多边形类型数据
    }

    // 第二步：根据不同类型进行数据处理
    switch (Idex)
    {
    case 1:  // 处理点数据
    {
        break;
    }
    case 2:  // 处理矩形数据
    {
        auto rect_map = ptrInNodeDeviceJudgeData->GetCGObjectNodeFlow<VMUnorderedMap<int, PtrVMVector<PtrVMRectangle>>>();
        if (!rect_map) break;

        rect2.x = 819;
        rect2.y = 410;
        rect2.width = 907;
        rect2.height = 700;

        //测试代码，只针对类别0
        PtrVMVector<PtrVMRectangle> vmVector = rect_map->vmMap[0];
        int count = vmVector->vmVector.size();
        float maxIoU = 0.0;
        for (size_t i = 0; i < count; i++) {
            PtrVMRectangle rect = vmVector->vmVector[i];
            if (rect->calssId == judgementClassId) {
                rect1.x = rect->centerPoint.x - rect->width / 2.0f;
                rect1.y = rect->centerPoint.y - rect->height / 2.0f;
                rect1.width = rect->width;
                rect1.height = rect->height;
                float calculateIoU = CalculateIoU(rect1, rect2);
                if (calculateIoU > maxIoU) {
                    maxIoU = calculateIoU;
                }
                if (maxIoU > 0.3) {
                    ptrAndOutVMNodeState->nodeState = true;
                    ptrNotOutVMNodeState->nodeState = false;
                }
                else {
                    ptrAndOutVMNodeState->nodeState = false;
                    ptrNotOutVMNodeState->nodeState = true;
                }
            }
        }




        /*for (size_t i = 0; i < rect_map->vmMap.size(); i++) {
            PtrVMVector<PtrVMRectangle> vmVector = rect_map->vmMap[i];
            for (size_t i = vmVector->vmVector.size()  - 1; i < vmVector->vmVector.size(); i++) {
                PtrVMRectangle rect = vmVector->vmVector[i];
                if (rect->calssId == judgementClassId) {
                    rect1.x = rect->centerPoint.x - rect->width / 2.0f;
                    rect1.y = rect->centerPoint.y - rect->height / 2.0f;
                    rect1.width = rect->width;
                    rect1.height = rect->height;
                    float calculateIoU = CalculateIoU(rect1, rect2);
                    if (calculateIoU > 0.3) {
                        ptrAndOutVMNodeState->nodeState = true;
                        ptrNotOutVMNodeState->nodeState = false;
                    }
                    else {
                        ptrAndOutVMNodeState->nodeState = false;
                        ptrNotOutVMNodeState->nodeState = true;
                    }
                }
            }
        }*/
                //if (rect->calssId == judgementClassId) {
                //    // 更新检测状态
                //    judgeThreshold.pop_front();
                //    judgeThreshold.push_back(true);
                //    // 检查最近5帧中是否有任意4帧检测到
                //    bool anyDetectedInLastThreeFrames = false;
                //    int judgeCount = 0;
                //    for (bool frameDetected : judgeThreshold) {
                //        if (frameDetected) {
                //            judgeCount++;
                //            if (judgeCount >= 3) {
                //                anyDetectedInLastThreeFrames = true;
                //                break;
                //            }
                //        }
                //    }
                //    if (anyDetectedInLastThreeFrames) {
                //        if (rect->state) {
                //            ptrAndOutVMNodeState->nodeState = true;
                //            ptrNotOutVMNodeState->nodeState = false;
                //        }
                //        else {
                //            ptrAndOutVMNodeState->nodeState = false;
                //            ptrNotOutVMNodeState->nodeState = true;
                //        }
                //    }
                //}
            
        
        break;
    }
    case 3:  // 处理多边形数据
    {
        break;
    }
    default:  // 未知类型不做处理
        break;
    }

    ptrAndOutVMNodeState->state = true;
    ptrNotOutVMNodeState->state = true;

    std::chrono::high_resolution_clock::time_point tp2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<size_t, std::nano> dur = tp2 - tp1;
    elapsed_time = float(std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());

    QStandardItemModel* model = new QStandardItemModel();
    QStandardItem* item = new QStandardItem(QString("开始记录"));
    model->appendRow(item);

    item = new QStandardItem(QString("与逻辑判断结果为: %1").arg(ptrAndOutVMNodeState->nodeState ? "true" : "false"));
    model->appendRow(item);
    item = new QStandardItem(QString("非逻辑判断结果为: %1").arg(ptrNotOutVMNodeState->nodeState ? "true" : "false"));
    model->appendRow(item);

    ui.listView->setModel(model);


    if (isSelfRunring) {
        emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
    }
    else {
        emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
    }
    return 0;
}

void FramResultStateJudgment::AcceptNodeBindingSucceedSlot(const QtNodes::PortIndex port_index)
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


void FramResultStateJudgment::AcceptSelfRunringStateSlot(const float& time, const RUNNINGSTATE& run_state)
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

    item = new QStandardItem(QString("与逻辑判断结果为: %1").arg(ptrAndOutVMNodeState->nodeState ? "true" : "false"));
    model->appendRow(item);
    item = new QStandardItem(QString("非逻辑判断结果为: %1").arg(ptrNotOutVMNodeState->nodeState ? "true" : "false"));
    model->appendRow(item);

    ui.listView->setModel(model);
}


int FramResultStateJudgment::Execute()
{
    isSelfRunring = false;
    int res = this->RunningTool();
    return res;
}

int FramResultStateJudgment::OnceExecute()
{
    isSelfRunring = false;
    std::thread thrad = std::thread(&FramResultStateJudgment::RunningTool, this);
    thrad.detach();
    return 0;
}

void FramResultStateJudgment::SetNodeId(const int node_id)
{
    this->nodeId = node_id;
    ptrAndOutVMNodeState->nodeID = this->nodeId;
    //ptrNotOutVMNodeState->nodeID = this->nodeId;
    std::vector<PtrVMObject> v_ptrvmobject;
    v_ptrvmobject.emplace_back(ptrAndOutVMNodeState);
    //v_ptrvmobject.emplace_back(ptrNotOutVMNodeState);
    ptrMapNodesOutData->insert(std::make_pair(this->nodeId, v_ptrvmobject));
}

QJsonObject FramResultStateJudgment::SerializeFunciton()
{
    QJsonObject model_json;

    //functionParam.filterMethod = filterMethod;
    //functionParam.meanBlurParam = meanBlurParam;

    SerializeTParament(model_json, "FunctionParam", functionParam);
    SerializeTParament(model_json, "UiParam", uiParam);
    return model_json;
}

void FramResultStateJudgment::ExserializeFunciton(const QJsonObject& json)
{
    QString fun_json = json["FunctionParam"].toString();
    QString ui_json = json["UiParam"].toString();

    ExserializeTFunciton(fun_json, functionParam);
    ExserializeTFunciton(ui_json, uiParam);

    //filterMethod = functionParam.filterMethod;
    //meanBlurParam = functionParam.meanBlurParam;

    emit SendExserializeFinishSig();
}
