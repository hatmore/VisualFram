#pragma once
#include <QObject>
#include <tbb/concurrent_queue.h>
#include "BaseCommunicateDevice.h"
#include "CommunicateStruct.h"
#include "../GeneralStruct.h"

// S7客户端通信类
class SimensS7ClientCommunication : public BaseCommunicateDevice
{
    Q_OBJECT
public:
    SimensS7ClientCommunication();
    ~SimensS7ClientCommunication() override;
    using Ptr = std::shared_ptr<SimensS7ClientCommunication>;
    using ConstPtr = std::shared_ptr<const SimensS7ClientCommunication>;

    int InitalDevice(const CGDevice::Ptr ptr_device) override;
    int OpenDevice() override;
    int CloseDevice() override;
    int Write(const QByteArray& byte_array) override;
    int Read(QByteArray& byte_array) override;
    bool GetCommDiviceState() override { return commDiviceState; }
    CGDevice::Ptr GetCGDevicePtr() override { return ptrS7Client; }

    // 客户端特有的读写方法
    int ReadDB(int dbNumber, int start, int size, QByteArray& data);
    int WriteDB(int dbNumber, int start, const QByteArray& data);
    int ReadInputs(int start, int size, QByteArray& data);
    int WriteOutputs(int start, const QByteArray& data);
    int ReadMerkers(int start, int size, QByteArray& data);
    int WriteMerkers(int start, const QByteArray& data);

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
    PtrSimensS7Client ptrS7Client{ nullptr };
    QString deviceName;
    QString ipValue;
    int portValue;
    int rackValue;
    int slotValue;
};