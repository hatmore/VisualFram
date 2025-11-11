#include "FramYoloV11Detect.h"
#include <QDesktopWidget>
#include <QFileDialog>
#include <QTableView>
#include <QHeaderView> 

FramYoloV11Detect::FramYoloV11Detect(QWidget *parent)
    : ToolInterface(parent)
{
    ui.setupUi(this);
    Initial();
}

FramYoloV11Detect::~FramYoloV11Detect()
{
}

void FramYoloV11Detect::Initial()
{
    InitialNodeData();
    InitialImageShow();
    InitialPushButton();
    InitilaToolParamUI();
    InitialExserializeUi();

    ptrYoloV11DetectAlgorithm = std::make_shared<YoloV11DetectAlgorithm>();
}

void FramYoloV11Detect::InitialNodeData()
{
    ptrOutVMUnorderedMap = std::make_shared<VMUnorderedMap<int, PtrVMVector<PtrVMRectangle>>>();
    ptrOutNodeVMUnorderedMap = std::make_shared<BaseNodeData>();
    ptrOutNodeVMUnorderedMap->SetCGObjectNodeFlow(ptrOutVMUnorderedMap);
    
    FlowNodeAndName out_node_and_name;
    out_node_and_name.nodeName = "预测结果";
    out_node_and_name.ptrNodedata = ptrOutNodeVMUnorderedMap;
    mapCGObjectFlowOutNodeAndName.insert(std::make_pair(0, out_node_and_name));

    ptrInVMMat = std::make_shared<VMMat>();
    ptrInNodeVMMat = std::make_shared<BaseNodeData>();
    //ptrInNodeVMMat->SetCGObjectNodeFlow(ptrInVMMat);
    ptrInNodeVMMat->AddScopeNodeDataType(QtNodes::NodeDataType{ "5","Mat" });

    FlowNodeAndName in_node_and_name;
    in_node_and_name.nodeName = "图像";
    in_node_and_name.ptrNodedata = ptrInNodeVMMat;
    mapCGObjectFlowInNodeAndName.insert(std::make_pair(0, in_node_and_name));
}

void FramYoloV11Detect::InitialPushButton()
{
    connect(ui.pushButtonExecute, &QPushButton::clicked, this, [this]() {
        isSelfRunring = true;
        RunningTool();
        });  //按钮按下后信号发出执行操作
    connect(ui.pushButtonOpenModel, &QPushButton::clicked, this, &FramYoloV11Detect::PushButtonOpenModelClicked);
    connect(ui.pushButtonSetConfirm, &QPushButton::clicked, this, &FramYoloV11Detect::PushButtonSetConfirmClicked);
    connect(ui.pushButtonModelParaConfirm, &QPushButton::clicked, this, &FramYoloV11Detect::PushButtonModelParaConfirmClicked);
}

void FramYoloV11Detect::InitialImageShow()
{
    this->setWindowTitle("目标检测模型推理");
    this->setWindowIcon(QIcon(":/FramYoloV11Detect/Image/YoloDetect.png"));
    // 去掉问号，只保留关闭
    this->setWindowFlags(Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint);
    this->installEventFilter(this);

    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);

    ptrGraphicsViews2d = new QGraphicsViews2d(ui.widgetImageShow);
    QRect rect = ui.widgetImageShow->geometry();
    ptrGraphicsViews2d->setGeometry(QRect(0, 0, rect.width(), rect.height()));

    connect(this, &FramYoloV11Detect::SendNodeRuningStateSyncSig, this, &FramYoloV11Detect::AcceptSelfRunringStateSlot);
    connect(ui.spinBoxActionNum, QOverload<int>::of(&QSpinBox::valueChanged),this, &FramYoloV11Detect::SpinBoxValueChange);
}

void FramYoloV11Detect::InitilaToolParamUI()
{
    //初始化表格
    actionNum = ui.spinBoxActionNum->value();
    qTableViewModel = new QStandardItemModel(actionNum, 4, this);
    qTableViewModel->setHeaderData(0, Qt::Horizontal, QString::fromUtf8("序号"));
    qTableViewModel->setHeaderData(1, Qt::Horizontal, QString::fromUtf8("类别"));
    qTableViewModel->setHeaderData(2, Qt::Horizontal, QString::fromUtf8("类别名称"));
    qTableViewModel->setHeaderData(3, Qt::Horizontal, QString::fromUtf8("状态"));

    for (int i = 0; i < actionNum; ++i) {
        qTableViewModel->setItem(i, 0, new QStandardItem(QString::number(i + 1)));
        qTableViewModel->setItem(i, 1, new QStandardItem(QString::number(i)));
        qTableViewModel->setItem(i, 2, new QStandardItem(QString("名称" + QString::number(i + 1))));
        qTableViewModel->setItem(i, 3, new QStandardItem(QString::number(0)));
    }
    ui.tableView_1->setModel(qTableViewModel);
    ui.tableView_1->setStyleSheet(
        "QHeaderView::section {"
        "   font-family: 'Microsoft YaHei';"
        "   font-size: 14pt;"
        "   font-weight: bold;"
        "}"
        "QTableView::item {"
        "   border: 1px solid black;"
        "}"
    );
    QHeaderView* header = ui.tableView_1->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch); // 均匀分配列宽
    header->setStretchLastSection(false); // 禁用最后一列自动拉伸

    UpdateToolParam();
}

void FramYoloV11Detect::InitialExserializeUi()
{
    connect(this, &FramYoloV11Detect::SendExserializeFinishSig, this, [this]() {

        if (!functionParam.modelFilePath.isEmpty()) {
            ui.qLineEditNodeSource_2->setText(functionParam.modelFilePath);
        }
        ui.spinBoxActionNum->setValue(actionNum);
        ui.spinBoxClassNum->setValue(classNum);
        ui.spinBoxkInputHeight->setValue(kInputH);
        ui.spinBoxkInputWidth->setValue(kInputW);
        ui.spinBoxkConfThresh->setValue(static_cast<int>(kConfThresh*100));
        ui.inputModelLabels->setText(inputModelLabels.join(","));
    });
    //初始化数据
    connect(this, &FramYoloV11Detect::SendExserializeFinishSig, this, [this]() {

        if (modelClassId.count() > 0 && modelClassName.count() > 0) {
            if (ptrOutVMUnorderedMap != nullptr) {
                ptrOutVMUnorderedMap->vmMap.clear();
            }
            int validCount = qMin(qMin(actionNum, modelClassId.count()), modelClassName.count());
            for (int i = 0; i < validCount; ++i){
                qTableViewModel->setItem(i, 0, new QStandardItem(QString::number(i+1)));
                qTableViewModel->setItem(i, 1, new QStandardItem(QString::number(modelClassId[i])));
                qTableViewModel->setItem(i, 2, new QStandardItem(modelClassName[i]));
                qTableViewModel->setItem(i, 3, new QStandardItem(QString::number(0)));

                auto vmRectangle = std::make_shared <VMRectangle > ();
                vmRectangle->calssId = modelClassId[i];
                vmRectangle->state = 0;
                if (ptrOutVMUnorderedMap->vmMap.find(modelClassId[i]) == ptrOutVMUnorderedMap->vmMap.end()) {
                    ptrOutVMUnorderedMap->vmMap[modelClassId[i]] = std::make_shared<VMVector<PtrVMRectangle>>();
                }
                ptrOutVMUnorderedMap->vmMap[modelClassId[i]]->vmVector.push_back(vmRectangle);
            }
        }
    });
    connect(this, &FramYoloV11Detect::SendExserializeFinishSig, this, [this]() {

        std::string trtFile = functionParam.modelFilePath.toStdString();
        if (!trtFile.empty()) {
            int res = detector2.LoadEngineFromFile(trtFile, functionParam);
        }
    });
}

int FramYoloV11Detect::RunningTool()
{
    std::chrono::high_resolution_clock::time_point tp1 = std::chrono::high_resolution_clock::now();
    RUNNINGSTATE node_state = RUNNINGSTATE::RUNNING_SUCESS;

    std::pair<LOGTYPE, QString> p_log_info;
    QString str_log = "节点: " + QString::number(this->nodeId) + ": ";
    p_log_info.first = LOGTYPE::INFO;
    p_log_info.second = str_log + "开始检测";
    this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);

    float  elapsed_time = 0.0;
    ptrInVMMat = ptrInNodeVMMat->GetCGObjectNodeFlow<VMMat>();
    ptrOutVMUnorderedMap->state = false;
    
    if (ptrInVMMat == nullptr || ptrInVMMat->vmMat.data == nullptr || ptrInVMMat->state == false) {
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

    //获取目标检测模型预测结果
    int returnState = ptrYoloV11DetectAlgorithm->RunYoloV11Detect(ptrInVMMat->vmMat, detector2, ptrOutVMUnorderedMap, functionParam, inferResultShow);
    
    if (returnState != 0) {
        p_log_info.first = LOGTYPE::ERRORS;
        p_log_info.second = str_log + " 目标检测失败，请检查! ";
        this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);
    }
    ptrOutVMUnorderedMap->timeStamp = ptrInVMMat->timeStamp;
    ptrOutVMUnorderedMap->state = true;
    std::chrono::high_resolution_clock::time_point tp2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<size_t, std::nano> dur = tp2 - tp1;
    elapsed_time = float(std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());

    p_log_info.first = LOGTYPE::INFO;
    p_log_info.second = str_log + " 目标检测完成;-- Time: " + QString::number(elapsed_time);
    this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);

    if (isSelfRunring) {
        emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
    }
    else {
        emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
    }
    return 0;
}

void FramYoloV11Detect::AcceptNodeBindingSucceedSlot(const QtNodes::PortIndex port_index)
{
    auto iter = mapCGObjectFlowInNodeAndName.find(port_index);
    if (iter == mapCGObjectFlowInNodeAndName.end())
        return;

    auto ptr = std::static_pointer_cast<BaseNodeData>(iter->second.ptrNodedata);
    auto vm_object = ptr->GetCGObjectNodeFlow<VMObject>();
    QString name = vm_object->nodeDataType.name;
    QtNodes::NodeId node_id = ptr->GetNodeId();
    ui.qLineEditNodeSource->setText(QString::number(node_id) + ": " + name);
}

void FramYoloV11Detect::PushButtonOpenModelClicked()
{
    filePath = QFileDialog::getOpenFileName(
        this, "Open Plan or TRT File", "", "Plan Files (*.plan);;TRT Files (*.trt);;All Files (*.*)"
    );
    if (!filePath.isEmpty()) {
        ui.qLineEditNodeSource_2->setText(filePath);
        modelFilePath = filePath;
        std::string trtFile = ui.qLineEditNodeSource_2->text().toStdString();
        if (!trtFile.empty()) { 
            int res = detector2.LoadEngineFromFile(trtFile, functionParam); 
        }
    }
    else {
        qDebug() << "No file selected.";
    }
}

void FramYoloV11Detect::SpinBoxValueChange(int value)
{
    actionNum = value;
    int currentRows = qTableViewModel->rowCount();

    if (value > currentRows) {
        for (int i = currentRows; i < value; i++) {
            QList<QStandardItem*> newRow;
            newRow.append(new QStandardItem(QString::number(i + 1)));
            newRow.append(new QStandardItem(QString::number(i)));
            newRow.append(new QStandardItem(QString("名称" + QString::number(i + 1))));
            newRow.append(new QStandardItem(QString::number(0)));
            qTableViewModel->appendRow(newRow);
        }
    }
    else if (value < currentRows) {
        for (int i = currentRows - 1; i >= value; i--) {
            qTableViewModel->removeRow(i);
        }
    }
}

void FramYoloV11Detect::PushButtonSetConfirmClicked()
{
    UpdateToolParam();
}

void FramYoloV11Detect::UpdateToolParam()
{
    //更新模型输出结果和表格数据
    modelClassId.clear();
    modelClassName.clear();
    if (ptrOutVMUnorderedMap != nullptr) {
        ptrOutVMUnorderedMap->vmMap.clear();
    }

    for (int i = 0; i < actionNum; ++i) {
        int classId = qTableViewModel->data(qTableViewModel->index(i, 1)).toInt();
        QString name = qTableViewModel->data(qTableViewModel->index(i, 2)).toString();

        auto vmRectangle = std::make_shared <VMRectangle >();
        vmRectangle->calssId = classId;
        vmRectangle->state = 0;
        if (ptrOutVMUnorderedMap->vmMap.find(classId) == ptrOutVMUnorderedMap->vmMap.end()) {
            ptrOutVMUnorderedMap->vmMap[classId] = std::make_shared<VMVector<PtrVMRectangle>>();
        }
        ptrOutVMUnorderedMap->vmMap[classId]->vmVector.push_back(vmRectangle);
        //记录表格数据
        modelClassId.append(classId);
        modelClassName.append(name);
    }
}

void FramYoloV11Detect::PushButtonModelParaConfirmClicked()
{
    classNum = ui.spinBoxClassNum->value();
    kInputH = ui.spinBoxkInputHeight->value();
    kInputW = ui.spinBoxkInputWidth->value();
    kConfThresh = static_cast<float>(ui.spinBoxkConfThresh->value()) / 100;
    inputModelLabels = ui.inputModelLabels->text().trimmed().split(',', Qt::SkipEmptyParts);
    inferResultShow = ui.checkBoxResultShow->isChecked();
}

void FramYoloV11Detect::AcceptSelfRunringStateSlot(const float& time, const RUNNINGSTATE& run_state)
{
    //获取模型输出图像
    displayMat = ptrYoloV11DetectAlgorithm->GetInferImage();
    QString qstr_des = "状态: ";
    switch (run_state) {
    case RUNNING_SUCESS: {
        QImage image = CvMat2QImage(displayMat);
        ptrGraphicsViews2d->DispImage(image);
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

    //更新表格模型检测结果
    int index = 0;
    for (const auto& [classId, vec] : ptrOutVMUnorderedMap->vmMap) {
        for (const auto& vmRectangle : vec->vmVector) {
            qTableViewModel->setItem(index, 3, new QStandardItem(QString::number(vmRectangle->state)));
            index ++;
        }
    }
}

int FramYoloV11Detect::Execute()
{
    isSelfRunring = false;
    int res = this->RunningTool();
    return res;
}

int FramYoloV11Detect::OnceExecute()
{
    isSelfRunring = false;
    std::thread thrad = std::thread(&FramYoloV11Detect::RunningTool, this);
    thrad.detach();
    return 0;
}

void FramYoloV11Detect::SetNodeId(const int node_id)
{
    this->nodeId = node_id;
    ptrOutVMUnorderedMap->nodeID = this->nodeId;
    std::vector<PtrVMObject> v_ptrvmobject;
    v_ptrvmobject.emplace_back(ptrOutVMUnorderedMap);
    ptrMapNodesOutData->insert(std::make_pair(this->nodeId, v_ptrvmobject));
}

QJsonObject FramYoloV11Detect::SerializeFunciton()
{
    QJsonObject model_json;

    functionParam.actionNum = actionNum;
    functionParam.classNum = classNum;
    functionParam.modelClassId = modelClassId;
    functionParam.modelClassName = modelClassName;
    functionParam.modelFilePath = modelFilePath;
    functionParam.kInputH = kInputH;
    functionParam.kInputW = kInputW;
    functionParam.kConfThresh = kConfThresh;
    functionParam.inputModelLabels = inputModelLabels;

    SerializeTParament(model_json, "FunctionParam", functionParam);
    SerializeTParament(model_json, "UiParam", uiParam);
    return model_json;
}

void FramYoloV11Detect::ExserializeFunciton(const QJsonObject& json)
{
    QString fun_json = json["FunctionParam"].toString();
    QString ui_json = json["UiParam"].toString();

    ExserializeTFunciton(fun_json, functionParam);
    ExserializeTFunciton(ui_json, uiParam);
    
    //序列化文件中functionParam有值时更新
    if (functionParam.modelClassName.size() > 1) {
        actionNum = functionParam.actionNum;
        classNum = functionParam.classNum;
        modelClassId = functionParam.modelClassId;
        modelClassName = functionParam.modelClassName;
        modelFilePath = functionParam.modelFilePath;
        kInputH = functionParam.kInputH;
        kInputW = functionParam.kInputW;
        kConfThresh = functionParam.kConfThresh;
        inputModelLabels = functionParam.inputModelLabels;
    }

    emit SendExserializeFinishSig();
}