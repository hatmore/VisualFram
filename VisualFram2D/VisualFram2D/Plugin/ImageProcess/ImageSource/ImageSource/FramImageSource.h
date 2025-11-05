#pragma once

#include <QtWidgets/QDialog>
#include "Toolnterface/ToolInterface.h"
#include "ToolPublicFunction.h"
#include "ImageView/QGraphicsViews2d.h"
#include "SerializeStructParam.h"
#include "ui_FramImageSource.h"

class FramImageSource : public ToolInterface, private ToolPublicFunction
{
    Q_OBJECT

public:
    FramImageSource(QWidget *parent = nullptr);
    virtual ~FramImageSource() override;

public:
	//Execute
	virtual int Execute() override;

	/// <summary>
	/// 运行一次
	/// </summary>
	/// <returns></returns>
	virtual int OnceExecute() override;
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

    /// <summary>
    /// 设置nodeid，唯一编码
    /// 为寻找全局变量做准备的
    /// </summary>
    /// <param name="node_id"></param>
    virtual void SetNodeId(const int node_id) override;

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
	void Initial();

	void InitialNodeData();

	void InitialImageShow();

	void InitialPushButton();

	void InitialExserializeUi();
	/// <summary>
	/// 运行算法工具
	/// </summary>
	/// <returns></returns>
	int RunningTool();
	/// <summary>
	/// 读取文件夹下所有文件
	/// </summary>
	/// <param name="qstr"></param>
	void ReadImageDirPath(const QString& qstr);
	/// <summary>
	/// 获得所有文件
	/// </summary>
	/// <param name="path"></param>
	/// <param name="files"></param>
	void GetAllFiles(const std::string& path, std::vector<std::string>& files);

protected:
	/// <summary>
	/// 重写事件，主要是右上角询问按键
	/// </summary>
	/// <param name="obj"></param>
	/// <param name="event"></param>
	/// <returns></returns>
	virtual bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
	/// <summary>
	/// 读取本地文件
	/// </summary>
	void ReadLoalImageAndDir();
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
	PtrVMMat ptrVMMat{nullptr};
	BaseNodeData::Ptr ptrOutNodeVMMat;

	QGraphicsViews2d* ptrGraphicsViews2d{nullptr};
	FunctionParam functionParam; //算法结构参数
	UiParam uiParam; //界面参数

	int numLocalImage{ 0 };
	std::vector<std::string> vImageDirPaths;
	IMAGESOURCE sourceImageType{ IMAGESOURCE::SourceLocalImage };

	bool isSelfRunring{false};

/// <summary>
/// 与主界面相关的参数
/// </summary>
private:
        //ToolNodeDataInteract::Ptr ptrToolNodeDataInteract{ nullptr }; //与主界面交互
        std::shared_ptr<std::unordered_map<QString, std::shared_ptr<tbb::concurrent_bounded_queue<PtrExDeviceData>>>> ptrMapQueuePtrExDeviceData;  //外部设备数据

private:
    Ui::FramImageSourceClass ui;
};
