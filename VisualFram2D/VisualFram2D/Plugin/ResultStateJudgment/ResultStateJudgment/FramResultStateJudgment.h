#pragma once

#include <QtWidgets/QDialog>
#include "ui_FramResultStateJudgment.h"
#include "PublicFunction.h"
#include "ImageView/QGraphicsViews2d.h"
#include "SerializeStructParam.h"
#include <QStandardItemModel>

class FramResultStateJudgment : public ToolInterface, private PublicFunction
{
    Q_OBJECT

public:
    FramResultStateJudgment(QWidget *parent = nullptr);
    ~FramResultStateJudgment();

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
	void AcceptNodeBindingSucceedSlot(const  QtNodes::PortIndex port_index);

private slots:
	/// <summary>
	/// 接收自己发送的信息，显示一部分信息
	/// </summary>
	/// <param name="time"></param>
	/// <param name="run_state"></param>
	void AcceptSelfRunringStateSlot(const float& time, const RUNNINGSTATE& run_state);

public:
	/// <summary>
	/// 输出节点的数据和节点名称
	/// </summary>
	std::unordered_map<PortIndex, FlowNodeAndName> mapCGObjectFlowOutNodeAndName;
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
	//输出节点
	VMNodeState::Ptr ptrAndOutVMNodeState{ nullptr };
	BaseNodeData::Ptr ptrAndOutNodeVMNodeState;

	VMNodeState::Ptr ptrNotOutVMNodeState{ nullptr };
	BaseNodeData::Ptr ptrNotOutNodeVMNodeState;

private:
	//输入节点
    PtrVMUnorderedMap<int, PtrVMVector<PtrVMRectangle>> ptrInDeviceJudgeData{ nullptr }; //输入各种数据，进行解析
    BaseNodeData::Ptr ptrInNodeDeviceJudgeData{ nullptr }; //数据模型

	QGraphicsViews2d* ptrGraphicsViews2d{ nullptr };

	FunctionParam functionParam; //算法结构参数
	UiParam uiParam; //界面参数
	int judgementClassId;

	bool isSelfRunring{ true };
    std::deque<bool> judgeThreshold{false, false,  false, false};


    //测试
    cv::Rect rect1;
    cv::Rect rect2;

private:
    Ui::FramResultStateJudgmentClass ui;
};

