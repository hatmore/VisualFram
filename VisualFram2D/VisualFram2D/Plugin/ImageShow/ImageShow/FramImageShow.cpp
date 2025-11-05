#include "FramImageShow.h"
#include <QDesktopWidget>
#include <QFileDialog>

FramImageShow::FramImageShow(QWidget *parent)
    : ToolInterface(parent)
{
    ui.setupUi(this);
    Initial();
}

FramImageShow::~FramImageShow()
{}

void FramImageShow::Initial()
{
    InitialNodeData();
    InitialImageShow();
    InitilaToolParamUI();
    InitialExserializeUi();
    InitialPushButton();
}

void FramImageShow::InitialNodeData()
{
    ptrInVMMat = std::make_shared<VMMat>();
    ptrInNodeVMMat = std::make_shared<BaseNodeData>();
    //ptrInNodeVMMat->SetCGObjectNodeFlow(ptrInVMMat);
    ptrInNodeVMMat->AddScopeNodeDataType(QtNodes::NodeDataType{ "5","Mat" });
    FlowNodeAndName in_node_and_name;
    in_node_and_name.nodeName = "图像";
    in_node_and_name.ptrNodedata = ptrInNodeVMMat;

    ptrInNodeRanderROI = std::make_shared<MultiInputNodeData>();
    ptrInNodeRanderROI->AddScopeNodeDataType(QtNodes::NodeDataType{ "0","Bool" });
    ptrInNodeRanderROI->AddScopeNodeDataType(QtNodes::NodeDataType{ "1","Point" });
    ptrInNodeRanderROI->AddScopeNodeDataType(QtNodes::NodeDataType{ "2","Line" });
    ptrInNodeRanderROI->AddScopeNodeDataType(QtNodes::NodeDataType{ "3","Rectangle" });
    ptrInNodeRanderROI->AddScopeNodeDataType(QtNodes::NodeDataType{ "4","Circle" });
    ptrInNodeRanderROI->AddScopeNodeDataType(QtNodes::NodeDataType{ "100","Vector" });
    ptrInNodeRanderROI->AddScopeNodeDataType(QtNodes::NodeDataType{ "101","Unordered_map" });
    FlowNodeAndName in_node_and_name2;
    in_node_and_name2.nodeName = "渲染参数";
    in_node_and_name2.ptrNodedata = ptrInNodeRanderROI;

    mapCGObjectFlowInNodeAndName.insert(std::make_pair(1, in_node_and_name));
    mapCGObjectFlowInNodeAndName.insert(std::make_pair(0, in_node_and_name2));
} 

void FramImageShow::InitialImageShow()
{
    this->setWindowIcon(QIcon(":/FramImageShow/Image/ImageShow.png"));
    // 去掉问号，只保留关闭
    this->setWindowFlags(Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint);
    this->installEventFilter(this);
    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);
    ptrGraphicsViews2d = new QGraphicsViews2d(ui.widgetImageShow);
    QRect rect = ui.widgetImageShow->geometry();
    ptrGraphicsViews2d->setGeometry(QRect(0, 0, rect.width(), rect.height()));

    connect(this, &FramImageShow::SendNodeRuningStateSyncSig, this, &FramImageShow::AcceptSelfRunringStateSlot);
}

void FramImageShow::InitilaToolParamUI()
{
    ui.tableWidgetRanderParam->setColumnCount(1);
    ui.tableWidgetRanderParam->setHorizontalHeaderItem(0, new QTableWidgetItem("渲染项"));
    ui.tableWidgetRanderParam->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.tableWidgetRanderParam->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行
    ui.tableWidgetRanderParam->setSelectionMode(QAbstractItemView::SingleSelection);  //设置只能单选	
    ui.tableWidgetRanderParam->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");
}

void FramImageShow::InitialExserializeUi()
{
    connect(this, &FramImageShow::SendExserializeFinishSig, this, [this]() {
        ui.comboScreenNum->setCurrentIndex((int)functionParam.screenNum);
        });

}

void FramImageShow::InitialPushButton()
{
    connect(ui.comboScreenNum, QOverload<const int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        functionParam.screenNum = index;
        });

    connect(ui.pushButtonExecute, &QPushButton::clicked, this, [&]() {
        isSelfRunring = true;
        RunningTool();
        });  
}

void FramImageShow::AcceptNodeBindingSucceedSlot(const QtNodes::PortIndex port_index)
{
    if (port_index == 1) {
        auto iter = mapCGObjectFlowInNodeAndName.find(port_index);
        if (iter == mapCGObjectFlowInNodeAndName.end())
            return;

        auto ptr = std::static_pointer_cast<BaseNodeData>(iter->second.ptrNodedata);
        auto vm_object = ptr->GetCGObjectNodeFlow<VMObject>();
        QString name = vm_object->nodeDataType.name;
        QtNodes::NodeId node_id = ptr->GetNodeId();
        ui.qLineEditNodeImageSource->setText(QString::number(node_id) + ": " + name);
    }
    else{
        auto iter = mapCGObjectFlowInNodeAndName.find(port_index);
        if (iter == mapCGObjectFlowInNodeAndName.end())
            return;
        auto ptr = std::static_pointer_cast<MultiInputNodeData>(iter->second.ptrNodedata);
        auto vmvector_object = ptr->GetCGObjectNodeFlow<VMVector<PtrVMObject>>();
        ui.tableWidgetRanderParam->setRowCount(0);
        for (size_t i = 0; i < vmvector_object->vmVector.size(); i++){
            PtrVMObject ptr_object = vmvector_object->vmVector[i];
            QString name = ptr_object->nodeDataType.name;
            QtNodes::NodeId node_id = ptr_object->nodeID;;
            int count = ui.tableWidgetRanderParam->rowCount();
            QTableWidgetItem* item_type = new QTableWidgetItem(QString::number(node_id) + ": " + name);
            ui.tableWidgetRanderParam->setRowCount(count + 1);  //设置行数
            item_type->setFlags(item_type->flags() & (~Qt::ItemIsEditable));  //设置列不可编辑	
            ui.tableWidgetRanderParam->setItem(count, 0, item_type);
        }
    }
}

void FramImageShow::AcceptSelfRunringStateSlot(const float& time, const RUNNINGSTATE& run_state)
{
    QString qstr_des = "状态: ";
    switch (run_state) {
    case RUNNING_SUCESS: {
        if (ptrInVMMat == nullptr) {
            qstr_des = qstr_des + "输入图像指针为空，请链接数据！";
            return;
        }
        cv::Mat input_image = ptrInVMMat->vmMat;
        if (input_image.empty()) {
            qstr_des = qstr_des + "输入图像为空，请检查数据!";
            return;
        }
        RanderImageResult(input_image, vPtrObjectRanderData);
        ptrGraphicsViews2d->DispImage(CvMat2QImage(input_image));
        qstr_des = qstr_des + "运行成功";
    }
                       break;
    case RUNNING_FAIL: {
        qstr_des = qstr_des + "运行失败";
    }
                     break;
    case DATA_NULL: {
        qstr_des = qstr_des + "输入数据为空，运行失败";

        break;
    case PARAM_EINVAL: {
        qstr_des = qstr_des + "参数错误，运行失败";
    }
                     break;
    default:
        break;
    }
    }
    QString str_time = "耗时:" + QString::number(time, 'f', 2) + "ms -- ";
    ui.txtDescribe->setText(str_time + qstr_des);
}

int FramImageShow::RunningTool()
{
    std::chrono::high_resolution_clock::time_point tp1 = std::chrono::high_resolution_clock::now();
    RUNNINGSTATE node_state = RUNNINGSTATE::RUNNING_SUCESS;

    std::pair<LOGTYPE, QString> p_log_info;
    QString str_log = "节点: " + QString::number(this->nodeId) + ": ";
    p_log_info.first = LOGTYPE::INFO;
    p_log_info.second = str_log + "开始发送渲染数据";
    this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);

    float  elapsed_time = 0.0;
    ptrInVMMat = ptrInNodeVMMat->GetCGObjectNodeFlow<VMMat>();
    //cv::Mat input_image = ptrInVMMat->vmMat;
    ptrInROICoordinates = ptrInNodeRanderROI->GetCGObjectNodeFlow<VMVector<PtrVMObject>>();

    if (ptrInROICoordinates == nullptr || ptrInROICoordinates->vmVector.empty()) {
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
    ///把渲染结果放到队列里面
    std::vector<PtrVMObject>().swap(vPtrObjectRanderData);   //清空
    int res = ParseRanderData(vPtrObjectRanderData, ptrInROICoordinates);
    std::shared_ptr<tbb::concurrent_queue<std::vector<PtrVMObject>>> ptr_queue = (*this->toolNodeDataInteract->ptrMapPtrQueueNodeRanderObject)[functionParam.screenNum];
    ptr_queue->push(vPtrObjectRanderData);

    if (res != 0) {
        RUNNINGSTATE node_state = RUNNINGSTATE::DATA_NULL;
        p_log_info.first = LOGTYPE::ERRORS;
        p_log_info.second = str_log + " 节点数据解析失败，请检测数据！ ";
        this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);
        if (isSelfRunring) {
            emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
        }
        else {
            emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
        }
        return -1;
    }

    std::chrono::high_resolution_clock::time_point tp2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<size_t, std::nano> dur = tp2 - tp1;
    elapsed_time = float(std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());

    p_log_info.first = LOGTYPE::INFO;
    p_log_info.second = str_log + " 结果渲染数据完成;-- Time: " + QString::number(elapsed_time);
    this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);

    if (isSelfRunring) {
        emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
    }
    else {
        emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
    }
    return 0;
}

int FramImageShow::RanderImageResult(cv::Mat& image, const std::vector<PtrVMObject>& v_ptr_mobject)
{
    size_t index = 0;
    for (auto it = v_ptr_mobject.cbegin(); it != v_ptr_mobject.cend(); ++it) {
        auto ptr_mobject = *it;
        VMDATATYPE cg_type = ptr_mobject->cgType;
        bool state = ptr_mobject->state;
        ///可以修改，针对不同vector
        if (state == false) {
            return -1;
        }

        int class_id = ptr_mobject->calssId;
        QString tool_name = ptr_mobject->toolName;
        float confidence = ptr_mobject->confidence;

        switch (cg_type) {
        case VM_NULL:
            break;
        case VM_BOOL: {
            auto vm_node_state = std::static_pointer_cast<VMNodeState>(ptr_mobject);
            uint64 time_stamp = vm_node_state->timeStamp;

            std::string label = cv::format("ID: %d; Time: %d; %.2f", class_id, time_stamp, confidence);
            int base_line;
            cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &base_line);
            cv::putText(image, label, cv::Point(0, (index++) * 20 - base_line), 2.5, cv::FONT_HERSHEY_DUPLEX, cv::Scalar(255, 255, 255), 3, cv::LINE_AA);
        }
                    break;
        case VM_POINT: {
            auto vm_p = std::static_pointer_cast<VMPoint2d>(ptr_mobject);
            std::string label = cv::format("ID: %d; %.2f", class_id, confidence);
            int base_line;
            cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &base_line);
            cv::Point p = cv::Point(vm_p->x, vm_p->y);
            cv::circle(image, p, 10, cv::Scalar(0, 255, 120), -1);//画点，其实就是实心圆
            cv::putText(image, label, cv::Point(vm_p->x, vm_p->y - base_line), 2.5, cv::FONT_HERSHEY_DUPLEX, cv::Scalar(255, 255, 255), 3, cv::LINE_AA);
        }
                     break;
        case VM_RECTANGLE: {
            auto vm_rectangle = std::static_pointer_cast<VMRectangle>(ptr_mobject);
            cv::RotatedRect rorect = cv::RotatedRect(cv::Point(vm_rectangle->centerPoint.x, vm_rectangle->centerPoint.y),
                cv::Size(vm_rectangle->width, vm_rectangle->height), -vm_rectangle->angle);
            cv::Point2f vertices[4];
            rorect.points(vertices);
            cv::Point p1 = cv::Point((vertices[0].x + vertices[3].x) / 2, (vertices[0].y + vertices[3].y) / 2);
            cv::Point p2 = cv::Point((vertices[1].x + vertices[2].x) / 2, (vertices[1].y + vertices[2].y) / 2);
            for (int i = 0; i < 4; i++) {
                line(image, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 0, 255), 3);
            }

            std::string label = cv::format("ID: %d; %.2f", class_id, confidence);
            int base_line;
            cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &base_line);
            cv::putText(image, label, cv::Point(rorect.boundingRect().tl().x, rorect.boundingRect().tl().y - base_line), 2.5, cv::FONT_HERSHEY_DUPLEX, cv::Scalar(255, 255, 255), 3, cv::LINE_AA);
        }
                         break;
        case VM_POLYGON: {
            //画点
            auto vm_polygon = std::static_pointer_cast<VMPolygon>(ptr_mobject);
            std::vector<cv::Point> v_point;
            float pointConfidence;
            for (size_t i = 0; i < vm_polygon->vectorPoints.size(); i++) {
                VMPoint2d vm_point = vm_polygon->vectorPoints[i];
                pointConfidence = vm_polygon->vectorPoints[i].confidence;
                cv::Point cv_point = cv::Point(vm_point.x, vm_point.y);
                v_point.push_back(cv_point);

                if (vm_point.x < 0 || vm_point.y < 0) continue;
                if (pointConfidence < 0.5) continue;
                cv::circle(image, cv_point, 4, cv::Scalar(0, 255, 255), -1);//画点，其实就是实心圆
            }
            //画矩形框
            int penWidth = 2;
            cv::Rect aabb = cv::boundingRect(v_point);
            cv::rectangle(image, aabb, cv::Scalar(255, 125, 255), penWidth, cv::LINE_AA);
            //画关节
            int kpt1_idx, kpt2_idx, kpt1_x, kpt1_y, kpt2_x, kpt2_y;
            float kpt1_conf, kpt2_conf;

            cv::Scalar skeletonColor(0, 255, 255);
            for (size_t m = 0; m < skeleton.size(); m++) {
                kpt1_idx = skeleton[m][0] - 1;
                kpt2_idx = skeleton[m][1] - 1;
                kpt1_x = (int)v_point[kpt1_idx].x;
                kpt1_y = (int)v_point[kpt1_idx].y;
                kpt1_conf = vm_polygon->vectorPoints[kpt1_idx].confidence;
                kpt2_x = (int)v_point[kpt2_idx].x;
                kpt2_y = (int)v_point[kpt2_idx].y;
                kpt2_conf = vm_polygon->vectorPoints[kpt2_idx].confidence;
                if (kpt1_conf < 0.5 || kpt2_conf < 0.5) continue;
                if (kpt1_x < 0 || kpt1_y < 0) continue;
                if (kpt2_x < 0 || kpt2_y < 0) continue;
                cv::line(image, cv::Point(kpt1_x, kpt1_y), cv::Point(kpt2_x, kpt2_y), skeletonColor, penWidth, cv::LINE_AA);
            }

            std::string label = cv::format("ID: %d; %.2f", class_id, confidence);
            int base_line;
            cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &base_line);
            cv::putText(image, label, cv::Point(aabb.x, aabb.y - base_line), 2.5, cv::FONT_HERSHEY_DUPLEX, cv::Scalar(255, 255, 255), 3, cv::LINE_AA);

        }break;
        case VM_MAT:
            break;
        case VM_VECTOR:
            break;
        default:
            break;
        }
    }
    return 0;
}

int FramImageShow::ParseRanderData(std::vector<PtrVMObject>& v_ptr_object, const PtrVMObject& ptr_mobject)
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

int FramImageShow::Execute()
{
    isSelfRunring = false;
    int res = this->RunningTool();
    return res;
}

int FramImageShow::OnceExecute()
{
    isSelfRunring = false;
    std::thread thrad = std::thread(&FramImageShow::RunningTool, this);
    thrad.detach();
    return 0;
}

void FramImageShow::SetNodeId(const int node_id)
{
    //this->nodeId = node_id;
    //std::vector<PtrVMObject> v_ptrvmobject;
    //ptrMapNodesOutData->insert(std::make_pair(this->nodeId, v_ptrvmobject));
}

QJsonObject FramImageShow::SerializeFunciton()
{
    QJsonObject model_json;

    SerializeTParament(model_json, "FunctionParam", functionParam);
    SerializeTParament(model_json, "UiParam", uiParam);
    return model_json;
}

void FramImageShow::ExserializeFunciton(const QJsonObject& json)
{
    QString fun_json = json["FunctionParam"].toString();
    QString ui_json = json["UiParam"].toString();

    ExserializeTFunciton(fun_json, functionParam);
    ExserializeTFunciton(ui_json, uiParam);

    emit SendExserializeFinishSig();
}