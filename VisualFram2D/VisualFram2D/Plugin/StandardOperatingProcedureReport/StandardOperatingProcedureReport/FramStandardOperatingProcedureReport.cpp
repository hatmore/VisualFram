#pragma execution_character_set("utf-8")
#include "FramStandardOperatingProcedureReport.h"
#include <QDesktopWidget>
#include <QTextStream>
#include <QTextCodec>
#include <QFileDialog>
#include <stdlib.h>
#include <string>
#include <chrono>
#include <thread>
#include <future>
#include <io.h>

FramStandardOperatingProcedureReport::FramStandardOperatingProcedureReport(QWidget *parent)
    : ToolInterface(parent)
{
    ui.setupUi(this);
    Initial();
}

FramStandardOperatingProcedureReport::~FramStandardOperatingProcedureReport()
{}


void FramStandardOperatingProcedureReport::Initial()
{
    InitialNodeData();
    InitialImageShow();
    InitialPushButton();
    InitialQCodeEditor();
    InitialExserializeUi();

    ptrPyBind11PythonEngine = std::make_shared<PyBind11PythonEngine>();
    std::string path = "D:\\ProgramData\\Anaconda3\\envs\\Python310";
    ptrPyBind11PythonEngine->InitailPython(path);
}

void FramStandardOperatingProcedureReport::InitialNodeData()
{
    ptrInNodeDeviceWriteData = std::make_shared<MultiInputNodeData>();
    ptrInNodeDeviceWriteData->AddScopeNodeDataType(QtNodes::NodeDataType{ "0","Bool" });
    ptrInNodeDeviceWriteData->AddScopeNodeDataType(QtNodes::NodeDataType{ "1","Point" });
    ptrInNodeDeviceWriteData->AddScopeNodeDataType(QtNodes::NodeDataType{ "2","Line" });
    ptrInNodeDeviceWriteData->AddScopeNodeDataType(QtNodes::NodeDataType{ "3","Rectangle" });
    ptrInNodeDeviceWriteData->AddScopeNodeDataType(QtNodes::NodeDataType{ "4","Circle" });
    ptrInNodeDeviceWriteData->AddScopeNodeDataType(QtNodes::NodeDataType{ "100","Vector" });
    ptrInNodeDeviceWriteData->AddScopeNodeDataType(QtNodes::NodeDataType{ "101","Unordered_map" });
    FlowNodeAndName in_node_and_name;
    in_node_and_name.nodeName = "发送数据";
    in_node_and_name.ptrNodedata = ptrInNodeDeviceWriteData;

    ptrInNodeVMNodeState = std::make_shared<BaseNodeData>();
    ptrInNodeVMNodeState->AddScopeNodeDataType(QtNodes::NodeDataType{ "0","Bool" });

    FlowNodeAndName in_node_and_name2;
    in_node_and_name2.nodeName = "使能";
    in_node_and_name2.ptrNodedata = ptrInNodeVMNodeState;
    mapCGObjectFlowInNodeAndName.insert(std::make_pair(0, in_node_and_name));
    mapCGObjectFlowInNodeAndName.insert(std::make_pair(1, in_node_and_name2));
}

void FramStandardOperatingProcedureReport::InitialImageShow()
{
    this->setWindowTitle("SOP报表");
    this->setWindowIcon(QIcon(":/FramStandardOperatingProcedureReport/Image/SOP.png"));
    // 去掉问号，只保留关闭
    this->setWindowFlags(Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint);
    this->installEventFilter(this);

    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);

    connect(this, &FramStandardOperatingProcedureReport::SendNodeRuningStateSyncSig, this, &FramStandardOperatingProcedureReport::AcceptSelfRunringStateSlot);
}

void FramStandardOperatingProcedureReport::InitialQCodeEditor()
{
    mCompleters = new QLuaCompleter(this);
    mHighlighter = new QLuaHighlighter;
    mStyles = new QSyntaxStyle(this);

    mcodeEditor = new QCodeEditor();
    mcodeEditor->setCompleter(mCompleters);
    mcodeEditor->setHighlighter(mHighlighter);
    mcodeEditor->setWordWrapMode(QTextOption::WordWrap);
    auto h_box = new QHBoxLayout();
    ui.widgetEdit->setLayout(h_box);
    h_box->addWidget(mcodeEditor);
    h_box->setContentsMargins(0, 0, 0, 0);

    QString path = qApp->applicationDirPath();
    QString python_path = path + "/Python/demo/SOPReport.py";
    QString python_data;
    if (ReadPythonCodeFromFile(python_path, python_data)) {
        mcodeEditor->clear();
    }
    else {
        mcodeEditor->setPlainText("读取Python_Demo文件失败");
        return;
    }
    mcodeEditor->insertPlainText(python_data);
    ui.plainTextEditDebug->setReadOnly(true);

    QFile fl(":/FramStandardOperatingProcedureReport/Resources/default_style.xml");
    if (!fl.open(QIODevice::ReadOnly)) {
        return;
    }

    if (!mStyles->load(fl.readAll())) {
        delete mStyles;
        return;
    }
    mcodeEditor->setSyntaxStyle(mStyles);
}

void FramStandardOperatingProcedureReport::InitialPushButton()
{
    connect(ui.comboScreenNum, QOverload<const int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        functionParam.screenNum = index;
        });

    // 按下打开Python文件按钮
    connect(ui.pushButtonOpenPython, &QPushButton::clicked, this, &FramStandardOperatingProcedureReport::PushButtonOpenPython);

    connect(ui.pushButtonpPrecompile, &QPushButton::clicked, this, &FramStandardOperatingProcedureReport::PushButtonPrecompilePython);

    connect(ui.pushButtonDebug, &QPushButton::clicked, this, &FramStandardOperatingProcedureReport::PushButtonDebugPython);

    connect(ui.pushButtonExecute, &QPushButton::clicked, this, [&]() {
        isSelfRunring = true;
        RunningTool();
        });  //按钮按下后信号发出执行操作
}

void FramStandardOperatingProcedureReport::InitialExserializeUi()
{
    connect(this, &FramStandardOperatingProcedureReport::SendExserializeFinishSig, this, [this]() {
        ui.qLineEditPythonPath->setText(functionParam.pythonFilePath);
        QString python_data;
        if (ReadPythonCodeFromFile(functionParam.pythonFilePath, python_data)) {
            mcodeEditor->clear();
        }
        else {
            mcodeEditor->setPlainText("读取Python文件失败，请检查文件路径是否正确！");
            return;
        }
        ui.comboScreenNum->setCurrentIndex((int)functionParam.screenNum);
        mcodeEditor->insertPlainText(python_data);
        ptrPyBind11PythonEngine->PythonCodeLoad(functionParam.pythonFilePath.toStdString());
        });
}

bool FramStandardOperatingProcedureReport::ReadPythonCodeFromFile(const QString& file_path, QString& python_data)
{
    QFile file(file_path);
    if (file.open((QFile::ReadOnly | QFile::Text))) {
        QTextStream in(&file);
        in.setCodec("UTF-8");
        python_data = in.readAll();
        file.close();
        return true;
    }
    else {
        return false;
    }
}

void FramStandardOperatingProcedureReport::AcceptSelfRunringStateSlot(const float& time, const RUNNINGSTATE& run_state)
{
    QString qstr_des = "状态: ";
    switch (run_state) {
    case RUNNING_SUCESS: {
        auto str_log = ptrPyBind11PythonEngine->GetPythonLog();
        ui.plainTextEditDebug->appendPlainText(str_log);
        if (sopMatData.empty())
            return;
        cv::namedWindow("Report", cv::WINDOW_AUTOSIZE); // 窗口自适应图像大小
        cv::imshow("Report", sopMatData);
        cv::waitKey(0);
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

void FramStandardOperatingProcedureReport::PushButtonOpenPython()
{
    QString path = QDir::currentPath();
    QString python_path = QFileDialog::getOpenFileName(this, tr("打开Python文件"), path, "*.py");
    if (false == python_path.isEmpty()) {
        ui.qLineEditPythonPath->setText(python_path);
        functionParam.pythonFilePath = python_path;
        QString python_data;
        if (ReadPythonCodeFromFile(python_path, python_data)) {
            mcodeEditor->clear();
        }
        else {
            mcodeEditor->setPlainText("读取Python文件失败，请检查文件路径是否正确！");
            return;
        }
        mcodeEditor->insertPlainText(python_data);
        ptrPyBind11PythonEngine->PythonCodeLoad(functionParam.pythonFilePath.toStdString());
    }
}

void FramStandardOperatingProcedureReport::PushButtonPrecompilePython()
{
    int res = ptrPyBind11PythonEngine->PythonPrecompile();
    if (res == 0) {
        ui.plainTextEditDebug->setPlainText("预编译成功！");
    }
    else {
        ui.plainTextEditDebug->setPlainText("预编译失败，请检查Python文件是否正确！");
    }
}

void FramStandardOperatingProcedureReport::PushButtonDebugPython()
{
    if (ptrInDeviceWriteData == nullptr) {
        QMessageBox::StandardButton result = QMessageBox::information(nullptr, "提示", "无节点数据链接，请检测后Debug! ");
        return;
    }

    QString str_debug;
    int res = ptrPyBind11PythonEngine->PythonDebug(str_debug, ptrInDeviceWriteData);
    auto str_log = ptrPyBind11PythonEngine->GetPythonLog();
    if (res == 0) {
        ui.plainTextEditDebug->setPlainText(str_debug);
        ui.plainTextEditDebug->appendPlainText(str_log);
    }
    else {
        ui.plainTextEditDebug->setPlainText("Debug失败，请检查Python文件是否正确！");
    }
}


void FramStandardOperatingProcedureReport::NodeTestData()
{
    ptrInDeviceWriteData = std::make_shared<VMVector<PtrVMObject>>();

    auto in_node = std::make_shared<VMNodeState>();
    in_node->nodeState = true; // 设置节点状态为true，表示使能
    in_node->calssId = 0;
    in_node->nodeID = 0;
    in_node->timeStamp = 100 + number *4;
    number++;
    PtrVMPoint2d ptrPoint2d = std::make_shared<VMPoint2d>();
    ptrPoint2d->x = 10.0f;
    ptrPoint2d->y = 20.0f;
    ptrPoint2d->calssId = 1;
    ptrPoint2d->nodeID =1;
    ptrPoint2d->timeStamp = 200 + number * 3;

    PtrVMPolygon ptrPolygon = std::make_shared<VMPolygon>();
    ptrPolygon->vectorPoints.push_back(VMPoint2d(10,20));
    ptrPolygon->vectorPoints.push_back(VMPoint2d(100, 200));
    ptrPolygon->nodeID = 2;
    ptrPolygon->calssId = 3;
    ptrPolygon->timeStamp = 300 + number *2;

    PtrVMVector<PtrVMObject> ptr_vmvector = std::make_shared<VMVector<PtrVMObject>>();
    ptr_vmvector->timeStamp = 500 + number;
    ptr_vmvector->nodeID = 3;
    for (size_t i = 0; i < 5; i++) {
        PtrVMPoint2d ptrPoint2d = std::make_shared<VMPoint2d>();
        ptrPoint2d->x = i*10.0f;
        ptrPoint2d->y = i*20.0f;
        ptrPoint2d->calssId = i + 4;
        ptr_vmvector->vmVector.push_back(ptrPoint2d);
        ptrPoint2d->nodeID = i + 4; // 设置节点ID，确保唯一性
    }

    ptrInDeviceWriteData->vmVector.push_back(in_node);
    ptrInDeviceWriteData->vmVector.push_back(ptrPoint2d);
    ptrInDeviceWriteData->vmVector.push_back(ptrPolygon);
    ptrInDeviceWriteData->vmVector.push_back(ptr_vmvector);

}

int FramStandardOperatingProcedureReport::RunningTool()
{
    std::chrono::high_resolution_clock::time_point tp1 = std::chrono::high_resolution_clock::now();
    RUNNINGSTATE node_state = RUNNINGSTATE::RUNNING_SUCESS;

    std::pair<LOGTYPE, QString> p_log_info;
    QString str_all_log = "节点: " + QString::number(this->nodeId) + ": ";
    p_log_info.first = LOGTYPE::INFO;
    p_log_info.second = str_all_log + "开始报表整理";
    this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);

    float  elapsed_time = 0.0;
    ptrInVMNodeState = ptrInNodeVMNodeState->GetCGObjectNodeFlow<VMNodeState>();
    ptrInDeviceWriteData = ptrInNodeDeviceWriteData->GetCGObjectNodeFlow<VMVector<PtrVMObject>>();
    //NodeTestData(); // 测试数据

    if (ptrInDeviceWriteData == nullptr || ptrInDeviceWriteData->vmVector.empty()) {
        RUNNINGSTATE node_state = RUNNINGSTATE::DATA_NULL;
        p_log_info.first = LOGTYPE::ERRORS;
        p_log_info.second = str_all_log + "节点输入数据为空，请检查！";
        this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);
        if (isSelfRunring) {
            emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
        }
        else {
            emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
        }
        return -1;
    }

    std::shared_ptr<tbb::concurrent_queue<std::pair<int, PtrVMObject>>> ptr_queue_sopreport = this->toolNodeDataInteract->ptrQueuePairSOPReport;
    PtrVMMat vm_pos_mat = std::make_shared<VMMat>();
    int res = 0;
    if (ptrInVMNodeState == nullptr) {
        res = ptrPyBind11PythonEngine->PythonExecute(ptrInDeviceWriteData, sopMatData);
        if (res == 0) {
            auto str_log = ptrPyBind11PythonEngine->GetPythonLog();
            ToolInterface::toolNodeDataInteract->ptrQueueNodeLogData->push(std::make_pair(LOGTYPE::INFO, str_log));
            vm_pos_mat->vmMat = sopMatData.clone();
            ptr_queue_sopreport->push(std::make_pair(functionParam.screenNum, vm_pos_mat));

            std::chrono::high_resolution_clock::time_point tp2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<size_t, std::nano> dur = tp2 - tp1;
            elapsed_time = float(std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());

            p_log_info.first = LOGTYPE::INFO;
            p_log_info.second = str_all_log + " 报表数据解析完成-- Time: " + QString::number(elapsed_time);
            this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);
        }
        else {
            p_log_info.first = LOGTYPE::ERRORS;
            p_log_info.second = str_all_log + " 报表数据解析失败，请检查! ";
            this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);
        }
    }
    else {
        //使能为true
        if (ptrInVMNodeState->nodeState) {
            //有连接数据节点
            if (ptrInDeviceWriteData->vmVector.size() > 0) {
                PtrVMObject ptr_object = ptrInDeviceWriteData->vmVector[0];
                //默认传入的数据类型为map
                auto ptr_vmvmap = std::static_pointer_cast<VMUnorderedMap<int, PtrVMObject>>(ptr_object);
                if (ptr_vmvmap->vmMap.size() > 0) {
                    res = ptrPyBind11PythonEngine->PythonExecute(ptrInDeviceWriteData, sopMatData);
                    if (res == 0) {
                        auto str_log = ptrPyBind11PythonEngine->GetPythonLog();
                        ToolInterface::toolNodeDataInteract->ptrQueueNodeLogData->push(std::make_pair(LOGTYPE::INFO, str_log));
                        vm_pos_mat->vmMat = sopMatData.clone();
                        ptr_queue_sopreport->push(std::make_pair(functionParam.screenNum, vm_pos_mat));

                        std::chrono::high_resolution_clock::time_point tp2 = std::chrono::high_resolution_clock::now();
                        std::chrono::duration<size_t, std::nano> dur = tp2 - tp1;
                        elapsed_time = float(std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());

                        p_log_info.first = LOGTYPE::INFO;
                        p_log_info.second = str_all_log + " 报表数据解析完成;-- Time: " + QString::number(elapsed_time);
                        this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);
                    }
                    else {
                        p_log_info.first = LOGTYPE::ERRORS;
                        p_log_info.second = str_all_log + " 报表数据解析失败，请检查! ";
                        this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);
                    }
                }
            }
            //for (size_t i = 0; i < ptrInDeviceWriteData->vmVector.size(); i++) {
            //    PtrVMObject ptr_object = ptrInDeviceWriteData->vmVector[i];
            //    //默认传入的数据类型为map
            //    auto ptr_vmvmap = std::static_pointer_cast<VMUnorderedMap<int, PtrVMObject>>(ptr_object);
            //    
            //}
        }
    }

    std::chrono::high_resolution_clock::time_point tp2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<size_t, std::nano> dur = tp2 - tp1;
    elapsed_time = float(std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());

    if (res != 0) {
        RUNNINGSTATE node_state = RUNNINGSTATE::RUNNING_FAIL;
        if (isSelfRunring) {
            emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
        }
        else {
            emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
        }
        return -1;
    }

    if (isSelfRunring) {
        emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
    }
    else {
        emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
    }
    return 0;
}


int FramStandardOperatingProcedureReport::Execute()
{
    isSelfRunring = false;
    int res = this->RunningTool();
    return res;
}

///这里使用多线程，分离运行会报错，不知道为什么。
int FramStandardOperatingProcedureReport::OnceExecute()
{
    isSelfRunring = false;
    std::thread thrad = std::thread(&FramStandardOperatingProcedureReport::RunningTool, this);
    thrad.detach();
    //this->RunningTool();
    return 0;
}

void FramStandardOperatingProcedureReport::SetNodeId(const int node_id)
{
    this->nodeId = node_id;
    std::vector<PtrVMObject> v_ptrvmobject;
    ptrMapNodesOutData->insert(std::make_pair(this->nodeId, v_ptrvmobject));
}

QJsonObject FramStandardOperatingProcedureReport::SerializeFunciton()
{
    QJsonObject model_json;
    SerializeTParament(model_json, "FunctionParam", functionParam);
    SerializeTParament(model_json, "UiParam", uiParam);
    return model_json;
}

void FramStandardOperatingProcedureReport::ExserializeFunciton(const QJsonObject& json)
{
    QString fun_json = json["FunctionParam"].toString();
    QString ui_json = json["UiParam"].toString();

    ExserializeTFunciton(fun_json, functionParam);
    ExserializeTFunciton(ui_json, uiParam);

    emit SendExserializeFinishSig();
}