#include "AutoCommunicationDeviceWriteThread.h"
#include "../StaticGlobalVariable.h"
#include "../Log/frmLog.h"

AutoCommunicationDeviceWriteThread::AutoCommunicationDeviceWriteThread()
{
    isThreadStopFlag = false; // 线程停止标志，默认不停止
    isThreadPauseFlag = false; // 线程暂停标志，默认暂停
    cycleCommDeviceWriteThread = nullptr; // 初始化线程指针为空

    FrmLog* ptr_framlog = FrmLog::FrmLogInstance();
    QObject::connect(this, &AutoCommunicationDeviceWriteThread::SendLogInfoSynSig, ptr_framlog, &FrmLog::LogMassageShowSlot);
    QObject::connect(this, &AutoCommunicationDeviceWriteThread::SendLogInfoAsynSig, ptr_framlog, &FrmLog::LogMassageShowSlot, Qt::BlockingQueuedConnection);
}

AutoCommunicationDeviceWriteThread::~AutoCommunicationDeviceWriteThread()
{
    this->DestroyCycleThread(); // 确保在析构时销毁线程
}

/// <summary>
/// 创建，现有线程
/// </summary>
bool AutoCommunicationDeviceWriteThread::BuildCycleThread()
{
    if (cycleCommDeviceWriteThread != nullptr) {
        emit SendLogInfoSynSig(" 写外部设备线程已经存在，线程创建失败 ", LOGTYPE::ERRORS);
        return false; // 线程已存在
    }

    isThreadStopFlag = true;
    isThreadPauseFlag = true;
    cycleCommDeviceWriteThread = std::make_unique<std::thread>(&AutoCommunicationDeviceWriteThread::CycleRunCommunicationDeviceWriteThread, this);
    emit SendLogInfoSynSig(" 写外部设备线程创建成功 ", LOGTYPE::INFO);
    return true;
}

/// <summary>
/// 销毁，现有线程
/// </summary>
bool AutoCommunicationDeviceWriteThread::DestroyCycleThread()
{
    isThreadStopFlag = false;
    isThreadPauseFlag = false;
    conditionVariableThread.notify_all();

    if (cycleCommDeviceWriteThread != nullptr) {
        if (cycleCommDeviceWriteThread->joinable()) {
            cycleCommDeviceWriteThread->join();
        }
        cycleCommDeviceWriteThread.reset();
        cycleCommDeviceWriteThread = nullptr;
    }
    emit SendLogInfoSynSig(" 写外部设备线程销毁成功 ", LOGTYPE::INFO);
    return true; // 销毁成功
}

/// <summary>
/// 开始解析外部事件线程
/// </summary>
void AutoCommunicationDeviceWriteThread::StartCycleThread()
{
    isThreadStopFlag = true;
    isThreadPauseFlag = true;
    conditionVariableThread.notify_all();
    emit SendLogInfoSynSig(" 写外部设备线程重新开始 ", LOGTYPE::INFO);
}

/// <summary>
/// 暂停解析外部事件线程
/// </summary>
void AutoCommunicationDeviceWriteThread::PauseCycleThread()
{
    isThreadPauseFlag = false;
    emit SendLogInfoSynSig(" 写外部设备线程暂停 ", LOGTYPE::INFO);
}

void AutoCommunicationDeviceWriteThread::CycleRunCommunicationDeviceWriteThread()
{
    while (isThreadStopFlag) {
        if (!isThreadPauseFlag) {
            std::unique_lock<std::mutex> thread_locker(mutexCycleRun);
            if (!isThreadPauseFlag) {
                conditionVariableThread.wait(thread_locker);
            }
            thread_locker.unlock();
        }
        QByteArray byte_array_data;
        if (StaticGlobalVariable::ptrToolNodeDataInteract->ptrQueueExternDeviceWriteData->try_pop(byte_array_data)) {
            ptrCommDeviceInterface->Write(byte_array_data);          
            emit SendLogInfoAsynSig("写外部设备数据: " + QString(byte_array_data), LOGTYPE::INFO);
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 避免忙等待
        }
    }
}
