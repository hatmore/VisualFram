#pragma once
#include <QObject>
#include <QDialog>
#include <QMap>
#include "../RemoteOperation/RemoteSignalStruct.h"
#include "ExterHardwareSetting/FramExterHardwareSetting.h"
#include "../GeneralStruct.h"
#include "./ThreadPools/ThreadPool.h"

#define MaxThreadNum 1024

class AutoRunOperationThreadPool : public QObject
{
    Q_OBJECT
public:
    AutoRunOperationThreadPool();
    ~AutoRunOperationThreadPool();

    /// <summary>
    /// 获得dialog，相机设置指针
    /// </summary>
    /// <param name="dialog"></param>
    void GetFramExterHardwareSettingDialog(QDialog* dialog);
    /// <summary>
    /// 获得远程数据解析的界面
    /// </summary>
    /// <param name="dialog"></param>
    void GetFramRemoteOperationEvents(QDialog* dialog) { 
        framRemoteOperationEvents = dialog;
    }

    /// <summary>
    /// 创建，现有线程
    /// </summary>
    void BuildCycleThread();

    /// <summary>
    /// 销毁，现有线程
    /// </summary>
    void DestroyCycleThread();

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
    void Initial();

    /// <summary>
    /// 打开获得相机数据
    /// </summary>
    void CycleGetMultiCameraData();

    /// <summary>
    /// 获得单个相机数据
    /// </summary>
    void CycleGetCameraData(const QString& key, const  BaseCamera::Ptr& ptr_camera);

    /// <summary>
    /// 打开获得外部设备线程
    /// </summary>
    void CycleMutilCommunicateDeviceReadData();

    /// <summary>
    /// 解析接收到的外部信息，循环线程
    /// </summary>
    void CycleRunCameraThread();



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
    std::atomic<bool> isThreadPauseFlag{ false };  // 线程暂停标志, true暂停
    bool isThreadStopFlag{ false };   //线程停止标志， ture停止
    std::mutex mutexCycleRun;
    std::condition_variable conditionVariableThread;

    std::unique_ptr<std::thread> cycleRunCameraDataThread{ nullptr };
    
private:
    ThreadPool::Ptr ptrThreadPools;
    QDialog* framCameraSetParam{nullptr};
    QDialog* framCommunicateDevice{nullptr};
    QDialog* framRemoteOperationEvents{nullptr};

};


