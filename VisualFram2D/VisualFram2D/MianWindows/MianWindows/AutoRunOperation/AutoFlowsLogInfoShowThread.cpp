#include "AutoFlowsLogInfoShowThread.h"
#include "../StaticGlobalVariable.h"
#include "../Log/frmLog.h"

AutoFlowsLogInfoShowThread::AutoFlowsLogInfoShowThread()
{
    isThreadStopFlag = false; // 线程停止标志，默认不停止
    isThreadPauseFlag = false; // 线程暂停标志，默认暂停
    cycleRunLogShowThread = nullptr; // 初始化线程指针为空

    FrmLog* ptr_framlog = FrmLog::FrmLogInstance();
    QObject::connect(this, &AutoFlowsLogInfoShowThread::SendLogInfoSynSig, ptr_framlog, &FrmLog::LogMassageShowSlot);
    QObject::connect(this, &AutoFlowsLogInfoShowThread::SendLogInfoAsynSig, ptr_framlog, &FrmLog::LogMassageShowSlot, Qt::BlockingQueuedConnection);
}

AutoFlowsLogInfoShowThread::~AutoFlowsLogInfoShowThread()
{
    this->DestroyCycleThread(); // 确保在析构时销毁线程
}

/// <summary>
/// 创建，现有线程
/// </summary>
bool AutoFlowsLogInfoShowThread::BuildCycleThread()
{
    if (cycleRunLogShowThread != nullptr) {
        emit SendLogInfoSynSig(" 节点显示日志线程创建失败 ", LOGTYPE::ERRORS);
        return false; // 线程已存在
    }

    isThreadStopFlag = true;
    isThreadPauseFlag = true;
    cycleRunLogShowThread = std::make_unique<std::thread>(&AutoFlowsLogInfoShowThread::CycleRunLogShowThread, this);
    emit SendLogInfoSynSig(" 节点显示日志线程创建成功 ", LOGTYPE::INFO);
    return true;
}

/// <summary>
/// 销毁，现有线程
/// </summary>
bool AutoFlowsLogInfoShowThread::DestroyCycleThread()
{
    isThreadStopFlag = false;
    isThreadPauseFlag = false;
    conditionVariableThread.notify_all();

    if (cycleRunLogShowThread != nullptr) {
        if (cycleRunLogShowThread->joinable()) {
            cycleRunLogShowThread->join();
        }
        cycleRunLogShowThread.reset();
        cycleRunLogShowThread = nullptr;
    }
    emit SendLogInfoSynSig(" 节点显示日志线程销毁成功 ", LOGTYPE::INFO);
    return true; // 销毁成功
}

/// <summary>
/// 开始解析外部事件线程
/// </summary>
void AutoFlowsLogInfoShowThread::StartCycleThread()
{
    isThreadStopFlag = true;
    isThreadPauseFlag = true;
    conditionVariableThread.notify_all();
    emit SendLogInfoSynSig(" 节点显示日志线程重新开始 ", LOGTYPE::INFO);
}

/// <summary>
/// 暂停解析外部事件线程
/// </summary>
void AutoFlowsLogInfoShowThread::PauseCycleThread()
{
    isThreadPauseFlag = false;
    emit SendLogInfoSynSig(" 节点显示日志线程暂停 ", LOGTYPE::INFO);
}

void AutoFlowsLogInfoShowThread::CycleRunLogShowThread()
{
    while (isThreadStopFlag) {
        if (!isThreadPauseFlag) {
            std::unique_lock<std::mutex> thread_locker(mutexCycleRun);
            if (!isThreadPauseFlag) {
                conditionVariableThread.wait(thread_locker);
            }
            thread_locker.unlock();
        }
        // 这里可以添加日志显示的逻辑
        std::pair<LOGTYPE, QString>  log_data_item;
        if(StaticGlobalVariable::ptrToolNodeDataInteract->ptrQueueNodeLogData->try_pop(log_data_item)) {        
            emit SendLogInfoAsynSig(log_data_item.second, log_data_item.first);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 避免忙等待
        }
    }
}
