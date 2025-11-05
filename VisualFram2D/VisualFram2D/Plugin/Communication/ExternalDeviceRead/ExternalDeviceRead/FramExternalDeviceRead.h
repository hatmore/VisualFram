#pragma once

#include <QtWidgets/QDialog>
#include "Toolnterface/ToolInterface.h"
#include "ToolPublicFunction.h"
#include "ImageView/QGraphicsViews2d.h"
#include "SerializeStructParam.h"
#include "ui_FramExternalDeviceRead.h"

class FramExternalDeviceRead : public ToolInterface, private ToolPublicFunction
{
    Q_OBJECT

public:
    FramExternalDeviceRead(QWidget *parent = nullptr);
    ~FramExternalDeviceRead();

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
    /// 运行算法工具
    /// </summary>
    /// <returns></returns>
    int RunningTool();

private slots:
    /// <summary>
    /// 接收自己发送的信息，显示一部分信息
    /// </summary>
    /// <param name="time"></param>
    /// <param name="run_state"></param>
    void AcceptSelfRunringStateSlot(const float& time, const RUNNINGSTATE& run_state);

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
    VMNodeState::Ptr ptrVMNodeState{ nullptr };
    BaseNodeData::Ptr ptrOutNodeVMNodeState;

    FunctionParam functionParam; //算法结构参数
    UiParam uiParam; //界面参数
    bool isSelfRunring{ false };

private:
    //设备通信数据,用于设备通信的输入输出数据
    std::shared_ptr<std::unordered_map<QString, std::shared_ptr<tbb::concurrent_queue<int>>>> ptrMapQueueDeviceCommunicationData; 

private:
    Ui::FramExternalDeviceReadClass ui;
};

