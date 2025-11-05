#pragma once
#include <QMap>
#include <QObject>
#include <tbb\concurrent_queue.h>
#include "../ThreadPools/ThreadPool.h"
#include "../CommunicateDevice/BaseCommunicateDevice.h"
#include "../GraphicsViewWidget/GraphicsViewTaskFlowWidget.h"
#include "../GeneralStruct.h"
#include "../PythonScript/CommunicateDevicePythonEngine.h"


class AutoSignalTriggerFlowsExeThread : public QObject
{
    Q_OBJECT
public:
    using Ptr = std::shared_ptr<AutoSignalTriggerFlowsExeThread>;
    using ConstPtr = std::shared_ptr<const AutoSignalTriggerFlowsExeThread>;

    AutoSignalTriggerFlowsExeThread();
    ~AutoSignalTriggerFlowsExeThread();

    /// <summary>
    /// 获得通信的指针，方便开线程，直接使用
    /// </summary>
    /// <param name="ptr"></param>
    void SetBaseCommunicateDevice(const BaseCommunicateDevice::Ptr &ptr) {
        ptrCommDeviceInterface = ptr;
    }

    /// <summary>
    /// 获得TaskFlow的流程信息
    /// </summary>
    /// <param name="qwidget"></param>
    void SetFramTaskFlowViewWidget(QWidget * qwidget);

    /// <summary>
    /// 创建，现有线程
    /// </summary>
    bool BuildCycleThread();

    /// <summary>
    /// 销毁，现有线程
    /// </summary>
    bool DestroyCycleThread();

    /// <summary>
    /// 开始解析外部事件线程
    /// </summary>
    void StartCycleThread();

    /// <summary>
    /// 暂停解析外部事件线程
    /// </summary>
    void PauseCycleThread();

    /// <summary>
    /// 线程是否运行
    /// </summary>
    /// <returns></returns>
    const bool IsRun() { return this->isThreadPauseFlag; }

    /// <summary>
    /// 线程是否创建
    /// </summary>
    /// <returns></returns>
    const bool IsCreateThread() { return this->isThreadStopFlag; }

    /// <summary>
    /// 清空所有设备队列
    /// </summary>
    void ClearAllDeviceQueues();

private:
    /// <summary>
    /// 初始化
    /// </summary>
    void Initial();

    /// <summary>
    /// 循环运行
    /// </summary>
    void CycleGetReviceDataThread();

    /// <summary>
    /// 流程处理操作
    /// </summary>
    /// <param name="task_flow"></param>
    /// <param name="order"></param>
    void TaskFlowProcess(GraphicsViewTaskFlowWidget* task_flow, const int order);

signals:
    /// <summary>
    /// 同步信号
    /// </summary>
    void SendLogInfoSynSig(const QString& info, const LOGTYPE type);
    /// <summary>
    /// 异步信号
    /// </summary>
    void SendLogInfoAsynSig(const QString& info, const LOGTYPE type);
    /// <summary>
    /// 
    /// </summary>
    /// <param name="map_order"></param>
    void SendCommunicateOrderSig(QMap<QString, int> &map_order);

private:
    std::atomic<bool> isThreadPauseFlag{ false };  // 线程暂停标志, fasle暂停
    std::atomic<bool> isThreadStopFlag{ false };   //线程停止标志， false停止
    std::mutex mutexCycleRun;
    std::condition_variable conditionVariableThread;
    /// <summary>
    /// 数据处理线程
    /// </summary>
    std::unique_ptr<std::thread> cycleRunReviceDataThread{ nullptr };
    ThreadPool::Ptr ptrThreadPools;

private:
    //搜索类型
    std::string searchType;
    ///智能指针，主要作用接收外部命令，然后对流程进行操作
    std::unique_ptr<tbb::concurrent_queue<std::unordered_map<std::string, int>>> uptrQueueCommunicateOrder{nullptr};

private:
    BaseCommunicateDevice::Ptr ptrCommDeviceInterface{ nullptr };
   CommunicateDevicePythonEngine* ptrCommunicateDevicePythonEngine{nullptr};

   QWidget* framTaskFlowViewWidget{nullptr};
};

