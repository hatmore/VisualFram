#include "FramYoloV11Pose.h"
#include <QDesktopWidget>
#include <QFileDialog>
#include <QTableView>
#include <QHeaderView> 

FramYoloV11Pose::FramYoloV11Pose(QWidget* parent)
    : ToolInterface(parent)
{
    ui.setupUi(this);
    Initial();
}

FramYoloV11Pose::~FramYoloV11Pose()
{}

void FramYoloV11Pose::Initial()
{
    InitialNodeData();
    InitialImageShow();
    InitialPushButton();
    InitilaToolParamUI();
    InitialExserializeUi();

    ptrYoloV11PoseAlgorithm = std::make_shared<YoloV11PoseAlgorithm>();
}

void FramYoloV11Pose::InitialNodeData()
{
    ptrOutVMUnorderedMap = std::make_shared<VMUnorderedMap<int, PtrVMPolygon>>();
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

void FramYoloV11Pose::InitialPushButton()
{
    connect(ui.pushButtonExecute, &QPushButton::clicked, this, [this]() {
        isSelfRunring = true;
        RunningTool();
        });  //按钮按下后信号发出执行操作
    connect(ui.pushButtonOpenModel, &QPushButton::clicked, this, &FramYoloV11Pose::PushButtonOpenModelClicked);
}

void FramYoloV11Pose::InitialImageShow()
{
    this->setWindowTitle("关键点检测模型");
    this->setWindowIcon(QIcon(":/FramYoloV11Pose/Image/YoloPose.png"));
    // 去掉问号，只保留关闭
    this->setWindowFlags(Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint);
    this->installEventFilter(this);

    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);

    ptrGraphicsViews2d = new QGraphicsViews2d(ui.widgetImageShow);
    QRect rect = ui.widgetImageShow->geometry();
    ptrGraphicsViews2d->setGeometry(QRect(0, 0, rect.width(), rect.height()));

    connect(this, &FramYoloV11Pose::SendNodeRuningStateSyncSig, this, &FramYoloV11Pose::AcceptSelfRunringStateSlot);
    connect(ui.spinBoxKeypointsNum, QOverload<int>::of(&QSpinBox::valueChanged), this, &FramYoloV11Pose::SpinBoxValueChange);
}

void FramYoloV11Pose::InitilaToolParamUI()
{
    //初始化表格
    classNum = ui.spinBoxKeypointsNum->value();
    qTableViewModel = new QStandardItemModel(classNum, 3, this);
    qTableViewModel->setHeaderData(0, Qt::Horizontal, QString::fromUtf8("类别序号"));
    qTableViewModel->setHeaderData(1, Qt::Horizontal, QString::fromUtf8("名称"));
    qTableViewModel->setHeaderData(2, Qt::Horizontal, QString::fromUtf8("预测结果"));

    for (int i = 0; i < classNum; ++i) {
        qTableViewModel->setItem(i, 0, new QStandardItem(QString::number(i)));
        qTableViewModel->setItem(i, 1, new QStandardItem(QString("ClassName" + QString::number(i))));
        qTableViewModel->setItem(i, 2, new QStandardItem(QString::number(0)));
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

    //初始化表格数据
    modelClassName.clear();
    for (int i = 0; i < ui.spinBoxKeypointsNum->value(); ++i) {
        QString name = qTableViewModel->data(qTableViewModel->index(i, 1)).toString();
        modelClassName.append(name);
    }
}

void FramYoloV11Pose::InitialExserializeUi()
{
    connect(this, &FramYoloV11Pose::SendExserializeFinishSig, this, [this]() {

        ui.qLineEditNodeSource_2->setText(functionParam.modelFilePath);
        });
    //初始化表格数据
    connect(this, &FramYoloV11Pose::SendExserializeFinishSig, this, [this]() {

        for (int i = 0; i < classNum; ++i) { qTableViewModel->setItem(i, 1, new QStandardItem(modelClassName[i])); }
        });
    //初始化预测模型
    connect(this, &FramYoloV11Pose::SendExserializeFinishSig, this, [this]() {

        std::string trtFile = functionParam.modelFilePath.toStdString();
        if (!trtFile.empty()) {
            int res = detector.LoadEngineFromFile(trtFile, functionParam);
        }
    });
}

int FramYoloV11Pose::RunningTool()
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

    //获取检测模型预测结果
    int returnState = ptrYoloV11PoseAlgorithm->RunYoloV11Pose(ptrInVMMat->vmMat, detector, ptrOutVMUnorderedMap, functionParam);
    if (returnState != 0) {
        p_log_info.first = LOGTYPE::ERRORS;
        p_log_info.second = str_log + " 目标检测失败，请检查! ";
        this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);
    }

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

void FramYoloV11Pose::AcceptNodeBindingSucceedSlot(const QtNodes::PortIndex port_index)
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

void FramYoloV11Pose::PushButtonOpenModelClicked()
{
    filePath = QFileDialog::getOpenFileName(
        this, "Open Plan or TRT File", "", "Plan Files (*.plan);;TRT Files (*.trt);;All Files (*.*)"
    );
    if (!filePath.isEmpty()) {
        ui.qLineEditNodeSource_2->setText(filePath);
        modelFilePath = filePath;
        std::string trtFile = ui.qLineEditNodeSource_2->text().toStdString();
        if (!trtFile.empty()) { detector.LoadEngineFromFile(trtFile, functionParam); }
    }
    else {
        qDebug() << "No file selected.";
    }
}

void FramYoloV11Pose::SpinBoxValueChange(int value)
{
    int currentRows = qTableViewModel->rowCount();

    if (value > currentRows) {
        for (int i = currentRows; i < value; i++) {
            QList<QStandardItem*> newRow;
            newRow.append(new QStandardItem(QString::number(i)));
            newRow.append(new QStandardItem(QString("example" + QString::number(i))));
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

void FramYoloV11Pose::AcceptSelfRunringStateSlot(const float& time, const RUNNINGSTATE& run_state)
{
    //获取模型输出图像
    displayMat = ptrYoloV11PoseAlgorithm->GetInferImage();
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

    //更新表格数据,多目标情况
    if (ptrOutVMUnorderedMap->vmMap.size() > 0) {
        for (int i = 0; i < ptrOutVMUnorderedMap->vmMap.size(); ++i) {
            for (int j = 0; j < ui.spinBoxKeypointsNum->value(); ++j) {
                qTableViewModel->setItem(j, 2, new QStandardItem(QString::number(ptrOutVMUnorderedMap->vmMap[i]->vectorPoints[j].state)));
            }
        }
    }
    classNum = ui.spinBoxKeypointsNum->value();
    modelClassName.clear();
    //存储表格数据
    for (int i = 0; i < ui.spinBoxKeypointsNum->value(); ++i) {
        QString name = qTableViewModel->data(qTableViewModel->index(i, 1)).toString();
        modelClassName.append(name);
    }
}


int FramYoloV11Pose::Execute()
{
    isSelfRunring = false;
    int res = this->RunningTool();
    return res;
}

int FramYoloV11Pose::OnceExecute()
{
    isSelfRunring = false;
    std::thread thrad = std::thread(&FramYoloV11Pose::RunningTool, this);
    thrad.detach();
    return 0;
}

void FramYoloV11Pose::SetNodeId(const int node_id)
{
    this->nodeId = node_id;
    ptrOutVMUnorderedMap->nodeID = this->nodeId;
    std::vector<PtrVMObject> v_ptrvmobject;
    v_ptrvmobject.emplace_back(ptrOutVMUnorderedMap);
    ptrMapNodesOutData->insert(std::make_pair(this->nodeId, v_ptrvmobject));
}

QJsonObject FramYoloV11Pose::SerializeFunciton()
{
    QJsonObject model_json;

    functionParam.classNum = classNum;
    functionParam.modelFilePath = modelFilePath;
    functionParam.modelClassName = modelClassName;

    SerializeTParament(model_json, "FunctionParam", functionParam);
    SerializeTParament(model_json, "UiParam", uiParam);
    return model_json;
}

void FramYoloV11Pose::ExserializeFunciton(const QJsonObject& json)
{
    QString fun_json = json["FunctionParam"].toString();
    QString ui_json = json["UiParam"].toString();

    ExserializeTFunciton(fun_json, functionParam);
    ExserializeTFunciton(ui_json, uiParam);

    classNum = functionParam.classNum;
    modelFilePath = functionParam.modelFilePath;
    if (functionParam.modelClassName.size() > 1) {
        modelClassName = functionParam.modelClassName;
    }

    emit SendExserializeFinishSig();
}