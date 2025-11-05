#pragma once

#include <QtWidgets/QDialog>
#include "Toolnterface/ToolInterface.h"
#include "ToolPublicFunction.h"
#include "SerializeStructParam.h"
#include "PythonEngine/PyBind11PythonEngine.h"

#include "ui_FramStandardOperatingProcedureReport.h"

#include <QCodeEditor>
#include <QLuaCompleter>
#include <QSyntaxStyle>
#include <QLuaHighlighter>


class FramStandardOperatingProcedureReport : public ToolInterface, private ToolPublicFunction
{
    Q_OBJECT

public:
    FramStandardOperatingProcedureReport(QWidget *parent = nullptr);
    ~FramStandardOperatingProcedureReport();


public:
    //Execute
    virtual int Execute() override;

    /// <summary>
    /// 运行一次
    /// </summary>
    /// <returns></returns>
    virtual int OnceExecute() override;
    /// <summary>
    /// 设置nodeid，唯一编码
    /// 为寻找全局变量做准备的
    /// </summary>
    /// <param name="node_id"></param>
    virtual void SetNodeId(const int node_id) override;
    /// <summary>
    /// 序列化文件
    /// </summary>
    /// <param name="qdom_doc">根文件</param>
    /// <param name="root_element">上一级文件，目录</param>
    /// <returns></returns>
    virtual QJsonObject SerializeFunciton() override;
    /// <summary>
    /// 反序列化文件
    /// </summary>
    /// <returns></returns>
    virtual void ExserializeFunciton(const QJsonObject& json) override;


public:
    /// <summary>
    /// 输出节点的数据和节点名称
    /// </summary>
    std::unordered_map<PortIndex, FlowNodeAndName> mapCGObjectFlowOutNodeAndName;
    /// <summary>
    /// 输入节点的数据和节点名称
    /// </summary>
    std::unordered_map<PortIndex, FlowNodeAndName> mapCGObjectFlowInNodeAndName;

private:
    /// <summary>
    void Initial();
    /// <summary>
    void InitialNodeData();

    void InitialImageShow();

    void InitialPushButton();

    void InitialExserializeUi();

    /// <summary>
    /// 代码编辑器，初始化
    /// </summary>
    void InitialQCodeEditor();

    /// <summary>
    /// 测试节点数据
    /// </summary>
    void NodeTestData();

private:
    /// <summary>
    /// 运行算法工具
    /// </summary>
    /// <returns></returns>
    int RunningTool();

    /// <summary>
    /// 读取Python代码文件
    /// </summary>
    /// <param name="file_path"></param>
    bool ReadPythonCodeFromFile(const QString& file_path, QString& python_data);


private slots:
    /// <summary>
    /// 接收自己发送的信息，显示一部分信息
    /// </summary>
    /// <param name="time"></param>
    /// <param name="run_state"></param>
    void AcceptSelfRunringStateSlot(const float& time, const RUNNINGSTATE& run_state);

    /// <summary>
    /// 打开 Python 的推按钮操作。
    /// </summary>
    void PushButtonOpenPython();

    /// <summary>
    void PushButtonPrecompilePython();

    void PushButtonDebugPython();

signals:
    /// <summary>
    /// 循环运行时，线程调用，信号
    /// </summary>
    void SendNodeRuningStateAsychSig(const float& time, const RUNNINGSTATE& run_state);
    /// <summary>
    /// 在本线程运行时，信号发送
    /// </summary>
    /// <param name="time">时间</param>
    /// <param name="run_state">运行状态</param>
    void SendNodeRuningStateSyncSig(const float& time, const RUNNINGSTATE& run_state);
    /// <summary>
    /// 序列化完成信号
    /// </summary>
    void SendExserializeFinishSig();

private:
    FunctionParam functionParam; //算法结构参数
    UiParam uiParam; //界面参数
    bool isSelfRunring{ false };

    PtrVMVector<PtrVMObject> ptrInDeviceWriteData{ nullptr }; //输入各种数据，进行解析
    MultiInputNodeData::Ptr ptrInNodeDeviceWriteData{ nullptr }; //数据模型

    PtrVMNodeState ptrInVMNodeState{ nullptr };
    BaseNodeData::Ptr ptrInNodeVMNodeState;

    PyBind11PythonEngine::Ptr ptrPyBind11PythonEngine{ nullptr };

    cv::Mat sopMatData;


    /// <summary>
    /// 渲染数据，主要用于发送出去
    /// </summary>
    std::vector<PtrVMObject> vPtrObjectRanderData;

    int number = 0;

private:
    QCodeEditor* mcodeEditor{ nullptr };
    QCompleter* mCompleters{ nullptr };
    QStyleSyntaxHighlighter* mHighlighter{ nullptr };
    QSyntaxStyle* mStyles{ nullptr };

private:
    Ui::FramStandardOperatingProcedureReportClass ui;
};

