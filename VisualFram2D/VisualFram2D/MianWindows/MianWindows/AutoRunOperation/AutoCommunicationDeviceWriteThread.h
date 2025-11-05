#pragma once
#include <QObject>
#include <QDialog>
#include <QMap>
#include "../CommunicateDevice/BaseCommunicateDevice.h"
#include "../GeneralStruct.h"

class AutoCommunicationDeviceWriteThread : public QObject
{
    Q_OBJECT
public:
    using Ptr = std::shared_ptr<AutoCommunicationDeviceWriteThread>;
    using ConstPtr = std::shared_ptr<const AutoCommunicationDeviceWriteThread>;

    AutoCommunicationDeviceWriteThread();
    ~AutoCommunicationDeviceWriteThread() override;

    /// <summary>
    /// 获得通信的指针，方便开线程，直接使用
    /// </summary>
    /// <param name="ptr"></param>
    void SetBaseCommunicateDevice(const BaseCommunicateDevice::Ptr& ptr) {
        ptrCommDeviceInterface = ptr;
    }

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
    ///写入设备线程函数
    /// </summary>
    void CycleRunCommunicationDeviceWriteThread();

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
    std::atomic<bool> isThreadPauseFlag{ false };  // 线程暂停标志, true运行，false暂停
    bool isThreadStopFlag{ false };   //true表示存在，false表示不存在
    std::mutex mutexCycleRun;
    std::condition_variable conditionVariableThread;
    std::unique_ptr<std::thread> cycleCommDeviceWriteThread{ nullptr };

    BaseCommunicateDevice::Ptr ptrCommDeviceInterface{ nullptr };
};

