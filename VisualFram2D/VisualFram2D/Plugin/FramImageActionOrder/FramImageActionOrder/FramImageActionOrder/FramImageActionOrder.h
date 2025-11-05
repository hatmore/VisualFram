#pragma once

#include <QtWidgets/QDialog>
#include "PublicFunction.h"
#include "ImageView/QGraphicsViews2d.h"
#include"ImageActionOrderAlgorithm.h"
#include "SerializeStructParam.h"
#include"ui_FramImageActionOrder.h"

class FramImageActionOrder : public ToolInterface, private PublicFunction
{
    Q_OBJECT

public:
    FramImageActionOrder(QWidget* parent = nullptr);
    ~FramImageActionOrder();

public:
    //Execute
    virtual int Execute() override;

    /// <summary>
    /// 运行一次
    /// </summary>
    /// <returns></returns>
    virtual int OnceExecute() override;
    /// <summary>
    /// 
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
    
private:
    void Initial();

    void InitialNodeData();

    void InitilaToolParamUI();

    void InitialPushButton();

    void InitialImageShow();

    void InitialExserializeUi();

    int RunningTool();


public slots:
    /// <summary>
    /// 接收绑定完成槽函数，用于显示，那个数据绑定上了
    /// </summary>
    void AcceptNodeBindingSucceedSlot(const QtNodes::PortIndex port_index);

private slots:
    /// <summary>
    /// 接收自己发送的信息，显示一部分信息
    /// </summary>
    /// <param name="time"></param>
    /// <param name="run_state"></param>
    void AcceptSelfRunringStateSlot(const float& time, const RUNNINGSTATE& run_state);//运行状态显示

public:
    /// <summary>
    /// 输出节点的数据和节点名称
    /// </summary>
    std::unordered_map<PortIndex, FlowNodeAndName> mapCGObjectFlowOutNodeAndName; //存入输出映射表
    /// <summary>
    /// 输入节点的数据和节点名称
    /// </summary>
    std::unordered_map<PortIndex, FlowNodeAndName> mapCGObjectFlowInNodeAndName;

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

    //输出
    //PtrVMUnorderedMap<int, PtrVMNodeState>ptrOutVec = {nullptr};
    PtrVMUnorderedMap<int, PtrVMNodeState> ptrOutVec = { nullptr };
    BaseNodeData::Ptr ptrOutNodeData;
    //输入:1 roi
    PtrVMUnorderedMap<int, PtrVMVector<VMRectangle>>ptrInVMIds = { nullptr };
    BaseNodeData::Ptr ptrInNodeData;
    //输入:2 模型
    PtrVMUnorderedMap<int, PtrVMVector<PtrVMRectangle>>ptrInVec = { nullptr };
    BaseNodeData::Ptr ptrInNodeModelData;
    QGraphicsViews2d* ptrGraphicsViews2d{ nullptr };//视图显示（包括鼠标操作事件）
    //输入3
    PtrVMNodeState ptrInVMNodeState{ nullptr };
    BaseNodeData::Ptr ptrInNodeVMNodeState;
  
    float IouValue;
    FunctionParam functionParam; //算法结构参数
    UiParam uiParam; //界面参数
    bool isSelfRunring{ true };
    ImageActionOrderAlgorithm::Ptr ptrImageActionOrderAlgorithm;

private:
        int clearCount = 1;

private:
    Ui::FramImageActionOrderClass ui;

};
