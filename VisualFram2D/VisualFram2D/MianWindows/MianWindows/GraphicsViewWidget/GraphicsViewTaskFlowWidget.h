#pragma once
#include <QObject>
#include <QWidget>
#include <QGraphicsSceneDragDropEvent>
#include <QtWidgets/QGraphicsView>
#include <QGroupBox>
#include <QPushButton>
#include <QWidgetAction>
#include <QVBoxLayout>
#include <taskflow.hpp>  // Taskflow is header-only

#include "../NodeEditor/OverrideDataFlowGraphicsScene.h"
#include "../NodeEditor/OverrideDataFlowGraphModel.h"
#include "../NodeEditor/GraphicsViewDrapEvent.h"
#include "../ThreadPools/ThreadPool.h"
#include "../PlugiManager/PluginsManager.h"
#include "../Log/frmLog.h"
#include "../GeneralStruct.h"


class tf::Executor;
using PtrtfExecutor = std::shared_ptr<tf::Executor>;
class tf::Taskflow;
using PtrtfTaskflow = std::shared_ptr<tf::Taskflow>;
class tf::Task;
using PtrtfTask = std::shared_ptr<tf::Task>;


class GraphicsViewTaskFlowWidget : public QWidget
{
	Q_OBJECT
public:
	GraphicsViewTaskFlowWidget(QWidget* parent = Q_NULLPTR);
	~GraphicsViewTaskFlowWidget();
	/// <summary>
	/// 执行一次
	/// </summary>
	void TaskFlowOnceExecutor();
	/// <summary>
	/// 执行工具
	/// </summary>
	/// <param name="cyc_num">执行次数</param>
	void TaskFlowNCycleExecutor(const int& cyc_num);
	/// <summary>
	/// 执行工具
	/// </summary>
	/// <param name="cyc_num">执行次数</param>
	void TaskFlowCycleExecutor();
	/// <summary>
	/// 暂停当先线程
	/// </summary>
	void TaskFlowStopExecutor();
	/// <summary>
	/// 获得当前节点图
	/// </summary>
	/// <returns>返回节点图</returns>
    QtNodes::DataFlowGraphModelOverride* GetDataFlowGraphModel()const {
		return this->dataFlowGraphModel;
	}
	/// <summary>
	/// 设置当前流的名称
	/// </summary>
	/// <param name="name"></param>
	void SetTaskFlowName(const QString &name) {	
		this->taskFlowName = name;
	}
	
	/// <summary>
	/// 返回当前序列化名称
	/// </summary>
	/// <returns></returns>
	const QString GetTaskFlowName() const {	
		return this->taskFlowName;
	}

    /// <summary>
    /// 设置当前流的ID
    /// </summary>
    /// <param name="name"></param>
    void SetTaskFlowId(const int id);
    /// <summary>
    /// 返回当前ID
    /// </summary>
    /// <returns></returns>
    const int GetTaskFlowId() const {
        return this->taskFlowId;
    }

	/// <summary>
	/// 返回当前流是否运行
	/// </summary>
	/// <param name="state"></param>
	void SetTaskFlowEnabledState(const bool& state) {
		this->isEnabledState = state;
	}

	/// <summary>
	/// 返回当前流使能状态，是否为可以运行
	/// </summary>
	/// <returns></returns>
	const bool GetTaskFlowEnabledState() const {
		return this->isEnabledState;
	}

	/// <summary>
	/// 获得当前流是否在运行状态，true：运行，false:不在运行
	/// </summary>
	/// <returns></returns>
	const bool GetTaskFlowRunState() const {
        if (this) {
            return this->isCycleRun;
        }
        else {
            return false;
        }
	}

private:
	void Initial();
	/// <summary>
	/// 初始话按键相关
	/// </summary>
	void InitialPushButton();
	/// <summary>
	/// 初始化左键
	/// </summary>
	void InitialButtonMenu();
	/// <summary>
	/// 初始化节点相关
	/// </summary>
	void InitialQtNode();
	/// <summary>
	/// 初始化流相关diamagnetic
	/// </summary>
	void InitialTaskFlow();

private:
	void BuildNodeTaskFlow();
	void BuildNodeTaskFlow(PtrtfTaskflow& ptr_task_flow, PtrtfTask& ptr_tftask, const QtNodes::NodeId& id);
    /// <summary>
    /// 日志间断输出控制
    /// </summary>
    /// <param name="interval"></param>
    /// <returns></returns>
    inline bool RateLimiter(std::chrono::milliseconds interval) {
        auto now = std::chrono::steady_clock::now();
        if (now - lastLogInputTime >= interval) {
            lastLogInputTime = now;
            return true;
        }
        return false;
    }

private slots:
	/// <summary>
	/// 修改节点名称
	/// </summary>
	void ModefyCurrentNodeTitleNameSlot();
	/// <summary>
	/// 运行当前节点槽函数
	/// </summary>
	void RunCurrentNodeToolSlot();
	/// <summary>
	/// 删除当前节点
	/// </summary>
	void DeleteCurrentNodeToolSlot();
	/// <summary>
	/// 禁用当前节点
	/// </summary>
	void EnableCurrentNodeToolSlot();

signals:
    /// <summary>
    /// 异步信号
    /// </summary>
	void SendLogInfoAsynSig(const QString& info,const LOGTYPE type);
    /// <summary>
    /// 同步信号
    /// </summary>
	void SendLogInfoSynSig(const QString& info, const LOGTYPE type);

private:
	/// <summary>
	/// 流程名称
	/// </summary>
	QString taskFlowName;
	/// <summary>
	/// 流程是否打开标志
	/// </summary>
	bool isEnabledState{false};

	std::shared_ptr<NodeDelegateModelRegistry> ptrNodeDelegateModelRegistry;
	QtNodes::DataFlowGraphModelOverride* dataFlowGraphModel;
	QtNodes::DataFlowGraphicsSceneOverride* dataFlowGraphicsScene;
	QtNodes::GraphicsView* graphicsView;
	QtNodes::NodeId currentNodeId{ 0 };

	PtrtfExecutor ptrtfExecutor;
	PtrtfTaskflow ptrtfTaskflow;
	
	QHBoxLayout* qHBoxLayout{nullptr};
    QMenu* mMenu{ nullptr };

    std::atomic<bool> isCycleRun{ false };//开启循环执行
	std::mutex mutexCycleRun;
	ThreadPool::Ptr ptrThreadPools;   //新建线程池
	ToolInterface* pluginInterface{ nullptr };
    /// <summary>
    /// 流程唯一标识ID
    /// </summary>
    int taskFlowId{ 0 };

    /// <summary>
      /// 自身流程的节点数据
      /// </summary>
    std::shared_ptr<std::unordered_map<int, std::vector<PtrVMObject>>> ptrMapNodesOutData{ nullptr };

    std::chrono::steady_clock::time_point lastLogInputTime{};  //日志输出最后时间
};

