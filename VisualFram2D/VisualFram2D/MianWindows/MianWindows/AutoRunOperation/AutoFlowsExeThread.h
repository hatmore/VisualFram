#pragma once
#include <QMap>
#include <QObject>
#include <tbb\concurrent_queue.h>
#include "../ThreadPools/ThreadPool.h"
#include "../CommunicateDevice/BaseCommunicateDevice.h"
#include "../GraphicsViewWidget/GraphicsViewTaskFlowWidget.h"
#include "../GeneralStruct.h"
#include "../PythonScript/CommunicateDevicePythonEngine.h"

class AutoFlowsExeThread : public QObject
{
    Q_OBJECT
public:
    using Ptr = std::shared_ptr<AutoFlowsExeThread>;
    using ConstPtr = std::shared_ptr<const AutoFlowsExeThread>;

    AutoFlowsExeThread();
    ~AutoFlowsExeThread();

    /// <summary>
    /// 获得TaskFlow的流程信息
    /// </summary>
    /// <param name="qwidget"></param>
    void SetFramTaskFlowViewWidget(QWidget* qwidget);

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

private:
    /// <summary>
    /// 初始化
    /// </summary>
    void Initial();


signals:
    /// <summary>
    /// 同步信号
    /// </summary>
    void SendLogInfoSynSig(const QString& info, const LOGTYPE type);
    /// <summary>
    /// 异步信号
    /// </summary>
    void SendLogInfoAsynSig(const QString& info, const LOGTYPE type);


private:
    std::atomic<bool> isThreadPauseFlag{ false };  // 线程暂停标志, fasle暂停
    std::atomic<bool> isThreadStopFlag{ false };   //线程停止标志， false停止
    std::mutex mutexCycleRun;
    std::condition_variable conditionVariableThread;
    ThreadPool::Ptr ptrThreadPools;

private:
    std::shared_ptr<std::map<int, GraphicsViewTaskFlowWidget*>> ptrMapTaskFlowsWidget{nullptr};

};