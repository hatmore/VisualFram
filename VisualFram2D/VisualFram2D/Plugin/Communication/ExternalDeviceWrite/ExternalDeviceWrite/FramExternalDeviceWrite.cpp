#include "FramExternalDeviceWrite.h"
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

FramExternalDeviceWrite::FramExternalDeviceWrite(QWidget *parent)
    : ToolInterface(parent)
{
    ui.setupUi(this);
    Initial();
}

FramExternalDeviceWrite::~FramExternalDeviceWrite()
{}


void FramExternalDeviceWrite::Initial()
{
    InitialNodeData();
    InitialImageShow();
    InitialPushButton();
    InitialQCodeEditor();
    InitialExserializeUi();

    ptrPyBind11PythonEngine = std::make_shared<PyBind11PythonEngine>();
    std::string path = "D:\\VisualFram2.0II\\VisualFram2D\\VisualFram2D\\ThirdParty\\Python310";
    ptrPyBind11PythonEngine->InitailPython(path);
}

void FramExternalDeviceWrite::InitialNodeData()
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

void FramExternalDeviceWrite::InitialImageShow()
{
    this->setWindowTitle("写设备通讯");
    this->setWindowIcon(QIcon(":/FramExternalDeviceWrite/Image/write.png"));
    // 去掉问号，只保留关闭
    this->setWindowFlags(Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint);
    this->installEventFilter(this);

    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);

    connect(this, &FramExternalDeviceWrite::SendNodeRuningStateSyncSig, this, &FramExternalDeviceWrite::AcceptSelfRunringStateSlot);
}

void FramExternalDeviceWrite::InitialQCodeEditor()
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
    QString python_path = path + "/Python/demo/CommunicateDeviceWrite.py";
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

    QFile fl(":/FramExternalDeviceWrite/Resources/default_style.xml");
    if (!fl.open(QIODevice::ReadOnly)) {
        return;
    }

    if (!mStyles->load(fl.readAll())) {
        delete mStyles;
        return;
    }
    mcodeEditor->setSyntaxStyle(mStyles);
}

void FramExternalDeviceWrite::InitialPushButton()
{
    // 按下打开Python文件按钮
    connect(ui.pushButtonOpenPython, &QPushButton::clicked, this, &FramExternalDeviceWrite::PushButtonOpenPython);

    connect(ui.pushButtonpPrecompile, &QPushButton::clicked, this, &FramExternalDeviceWrite::PushButtonPrecompilePython);
    
    connect(ui.pushButtonDebug, &QPushButton::clicked, this, &FramExternalDeviceWrite::PushButtonDebugPython);
    
    connect(ui.pushButtonExecute, &QPushButton::clicked, this, [&]() {
        isSelfRunring = true;
        RunningTool();
        });  //按钮按下后信号发出执行操作
}

void FramExternalDeviceWrite::InitialExserializeUi()
{
    connect(this, &FramExternalDeviceWrite::SendExserializeFinishSig, this, [this]() {
            ui.qLineEditPythonPath->setText(functionParam.pythonFilePath);
            QString python_data;
            if (ReadPythonCodeFromFile(functionParam.pythonFilePath, python_data)) {
                mcodeEditor->clear();
            }
            else {
                mcodeEditor->setPlainText("读取Python文件失败，请检查文件路径是否正确！");
                return;
            }
            mcodeEditor->insertPlainText(python_data);
            ptrPyBind11PythonEngine->PythonCodeLoad(functionParam.pythonFilePath.toStdString());
        });
}

bool FramExternalDeviceWrite::ReadPythonCodeFromFile(const QString& file_path, QString &python_data)
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

void FramExternalDeviceWrite::AcceptSelfRunringStateSlot(const float& time, const RUNNINGSTATE& run_state)
{
    QString qstr_des = "状态: ";
    switch (run_state) {
    case RUNNING_SUCESS: {
        ui.plainTextEditDebug->setPlainText(writeByteArray);
      //  ui.plainTextEditDebug->appendPlainText(str_log);
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

void FramExternalDeviceWrite::PushButtonOpenPython()
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

void FramExternalDeviceWrite::PushButtonPrecompilePython()
{
    int res =  ptrPyBind11PythonEngine->PythonPrecompile();
    if (res == 0) {
        ui.plainTextEditDebug->setPlainText("预编译成功！");
    }
    else {
        ui.plainTextEditDebug->setPlainText("预编译失败，请检查Python文件是否正确！");
    }
}

void FramExternalDeviceWrite::PushButtonDebugPython()
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

//int FramExternalDeviceWrite::RunningTool()
// {
//    ptrInDeviceWriteData = std::make_shared<VMVector<PtrVMObject>>();
//
//    auto in_node = std::make_shared<VMNodeState>();
//    in_node->nodeState = true; // 设置节点状态为true，表示使能
//    in_node->calssId = 10;
//    in_node->nodeID = 0;
//
//    PtrVMPoint2d ptrPoint2d = std::make_shared<VMPoint2d>();
//    ptrPoint2d->x = 10.0f;
//    ptrPoint2d->y = 20.0f;
//    ptrPoint2d->calssId = 1;
//    ptrPoint2d->nodeID =1;
//
//    PtrVMPolygon ptrPolygon = std::make_shared<VMPolygon>();
//    ptrPolygon->vectorPoints.push_back(VMPoint2d(10,20));
//    ptrPolygon->vectorPoints.push_back(VMPoint2d(100, 200));
//    ptrPolygon->nodeID = 2;
//
//
//    PtrVMVector<PtrVMObject> ptr_vmvector = std::make_shared<VMVector<PtrVMObject>>();
//    ptr_vmvector->nodeID = 3;
//    for (size_t i = 0; i < 5; i++) {
//        PtrVMPoint2d ptrPoint2d = std::make_shared<VMPoint2d>();
//        ptrPoint2d->x = i*10.0f;
//        ptrPoint2d->y = i*20.0f;
//        ptrPoint2d->calssId = i;
//        ptr_vmvector->vmVector.push_back(ptrPoint2d);
//        ptrPoint2d->nodeID = i + 4; // 设置节点ID，确保唯一性
//    }
//
//    ptrInDeviceWriteData->vmVector.push_back(in_node);
//    ptrInDeviceWriteData->vmVector.push_back(ptrPoint2d);
//    ptrInDeviceWriteData->vmVector.push_back(ptrPolygon);
//    ptrInDeviceWriteData->vmVector.push_back(ptrPolygon);
//    ptrInDeviceWriteData->vmVector.push_back(ptr_vmvector);
//
//    int res = ptrPyBind11PythonEngine->PythonExecute(ptrInDeviceWriteData, writeByteArray);
// 
//    return 0;
// }

int FramExternalDeviceWrite::RunningTool()
{
    std::chrono::high_resolution_clock::time_point tp1 = std::chrono::high_resolution_clock::now();
    RUNNINGSTATE node_state = RUNNINGSTATE::RUNNING_SUCESS;

    std::pair<LOGTYPE, QString> p_log_info;
    QString str_log = "节点: " + QString::number(this->nodeId) + ": ";
    p_log_info.first = LOGTYPE::INFO;
    p_log_info.second = str_log + "开始发送";
    this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);

    float  elapsed_time = 0.0;
    ptrInVMNodeState = ptrInNodeVMNodeState->GetCGObjectNodeFlow<VMNodeState>();
    ptrInDeviceWriteData = ptrInNodeDeviceWriteData->GetCGObjectNodeFlow<VMVector<PtrVMObject>>();

    if(ptrInDeviceWriteData == nullptr || ptrInDeviceWriteData->vmVector.empty()) {
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

    int res = 0;
    if (ptrInVMNodeState == nullptr) {
        res = ptrPyBind11PythonEngine->PythonExecute(ptrInDeviceWriteData, writeByteArray);
        auto str_python_log = ptrPyBind11PythonEngine->GetPythonLog();
        ToolInterface::toolNodeDataInteract->ptrQueueNodeLogData->push(std::make_pair(LOGTYPE::INFO, str_python_log));
        ToolInterface::toolNodeDataInteract->ptrQueueExternDeviceWriteData->push(writeByteArray);

        std::chrono::high_resolution_clock::time_point tp2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<size_t, std::nano> dur = tp2 - tp1;
        elapsed_time = float(std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());

        if (res != 0) {
            p_log_info.first = LOGTYPE::ERRORS;
            p_log_info.second = str_log + " 数据发送失败! ";
            this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);
        }
        else {
            p_log_info.first = LOGTYPE::INFO;
            p_log_info.second = str_log + " 数据发送成功;-- Time: " + QString::number(elapsed_time);
            this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);
        }
    }
    else {
        if (ptrInVMNodeState->nodeState) { 
            res = ptrPyBind11PythonEngine->PythonExecute(ptrInDeviceWriteData, writeByteArray);
            auto str_python_log = ptrPyBind11PythonEngine->GetPythonLog();
            ToolInterface::toolNodeDataInteract->ptrQueueNodeLogData->push(std::make_pair(LOGTYPE::INFO, str_python_log));
            ToolInterface::toolNodeDataInteract->ptrQueueExternDeviceWriteData->push(writeByteArray);

            std::chrono::high_resolution_clock::time_point tp2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<size_t, std::nano> dur = tp2 - tp1;
            elapsed_time = float(std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());

            if (res != 0) {
                p_log_info.first = LOGTYPE::ERRORS;
                p_log_info.second = str_log + " 数据发送失败! ";
                this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);
            }
            else{
                p_log_info.first = LOGTYPE::INFO;
                p_log_info.second = str_log + " 数据发送成功;-- Time: " + QString::number(elapsed_time);
                this->toolNodeDataInteract->ptrQueueNodeLogData->push(p_log_info);
            }
        }

    }

    if (isSelfRunring) {
        emit SendNodeRuningStateSyncSig(elapsed_time, node_state);
    }
    else {
        emit SendNodeRuningStateAsychSig(elapsed_time, node_state);
    }
    return 0;
}


int FramExternalDeviceWrite::Execute()
{
    isSelfRunring = false;
    int res = this->RunningTool();
    return res;
}

///这里使用多线程，分离运行会报错，不知道为什么。
int FramExternalDeviceWrite::OnceExecute()
{
    isSelfRunring = false;
    std::thread thrad = std::thread(&FramExternalDeviceWrite::RunningTool, this);
    thrad.detach();
    //this->RunningTool();
    return 0;
}

void FramExternalDeviceWrite::SetNodeId(const int node_id)
{
    this->nodeId = node_id;
    std::vector<PtrVMObject> v_ptrvmobject;
    ptrMapNodesOutData->insert(std::make_pair(this->nodeId, v_ptrvmobject));
}

QJsonObject FramExternalDeviceWrite::SerializeFunciton()
{
    QJsonObject model_json;
    SerializeTParament(model_json, "FunctionParam", functionParam);
    SerializeTParament(model_json, "UiParam", uiParam);
    return model_json;
}

void FramExternalDeviceWrite::ExserializeFunciton(const QJsonObject& json)
{
    QString fun_json = json["FunctionParam"].toString();
    QString ui_json = json["UiParam"].toString();

    ExserializeTFunciton(fun_json, functionParam);
    ExserializeTFunciton(ui_json, uiParam);

    emit SendExserializeFinishSig();
}