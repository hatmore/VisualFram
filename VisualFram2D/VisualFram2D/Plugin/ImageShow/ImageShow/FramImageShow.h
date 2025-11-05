#pragma once
#include <QtWidgets/QDialog>
#include "Toolnterface/ToolInterface.h"
#include "ImageView/QGraphicsViews2d.h"
#include "SerializeStructParam.h"
#include "PublicFunction.h"
#include "ui_FramImageShow.h"

class FramImageShow : public ToolInterface, private PublicFunction
{
    Q_OBJECT

public:
    FramImageShow(QWidget *parent = nullptr);
    ~FramImageShow();

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

private:
	void Initial();

	void InitialNodeData();

	void InitialImageShow();

	void InitilaToolParamUI();

	void InitialExserializeUi();

	void InitialPushButton();
    /// <summary>
    /// 渲染图像
    /// </summary>
    /// <param name="image">输入图像</param>
    /// <param name="v_ptr_mobject">渲染参数</param>
    int RanderImageResult(cv::Mat &image, const  std::vector<PtrVMObject> &v_ptr_mobject);

    /// <summary>
    /// 解析渲染数据
    /// </summary>
    int ParseRanderData(std::vector<PtrVMObject> &v_ptr_object, const PtrVMObject& ptr_mobject);


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
    QImage randerQImage;  //渲染后图片

private:
	FunctionParam functionParam; //算法结构参数
	UiParam uiParam; //界面参数
	bool isSelfRunring{ true };
	QGraphicsViews2d* ptrGraphicsViews2d{ nullptr };

	PtrVMMat ptrInVMMat{ nullptr };
	BaseNodeData::Ptr ptrInNodeVMMat;

    PtrVMVector<PtrVMObject> ptrInROICoordinates{ nullptr }; //输入参数坐标，具体类型未知
	MultiInputNodeData::Ptr ptrInNodeRanderROI{ nullptr }; //数据模型

    /// <summary>
    /// 渲染数据，主要用于发送出去
    /// </summary>
    std::vector<PtrVMObject> vPtrObjectRanderData;

    const std::vector<std::vector<int>> skeleton{
  {16, 14},
  {14, 12},
  {17, 15},
  {15, 13},
  {12, 13},
  {6, 12},
  {7, 13},
  {6, 7},
  {6, 8},
  {7, 9},
  {8, 10},
  {9, 11},
  {2, 3},
  {1, 2},
  {1, 3},
  {2, 4},
  {3, 5},
  {4, 6},
  {5, 7}
    };

private:
    Ui::FramImageShowClass ui;


};
