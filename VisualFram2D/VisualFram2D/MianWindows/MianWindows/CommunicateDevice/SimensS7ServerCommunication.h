#pragma once
#include <QObject>
#include <tbb/concurrent_queue.h>
#include "BaseCommunicateDevice.h"
#include "CommunicateStruct.h"
#include "../GeneralStruct.h"

class SimensS7ServerCommunication : public BaseCommunicateDevice
{
    Q_OBJECT
public:
    // 添加客户端区域地址定义
    static const byte S7AreaPE = 0x81;
    static const byte S7AreaPA = 0x82;
    static const byte S7AreaMK = 0x83;
    static const byte S7AreaDB = 0x84;
    static const byte S7AreaCT = 0x1C;
    static const byte S7AreaTM = 0x1D;

    SimensS7ServerCommunication();
    ~SimensS7ServerCommunication() override;
    using Ptr = std::shared_ptr<SimensS7ServerCommunication>;
    using ConstPtr = std::shared_ptr<const SimensS7ServerCommunication>;

    int InitalDevice(const CGDevice::Ptr ptr_device) override;
    int OpenDevice() override;
    int CloseDevice() override;
    int Write(const QByteArray& byte_array) override;
    int Read(QByteArray& byte_array) override;
    bool GetCommDiviceState() override { return commDiviceState; }
    CGDevice::Ptr GetCGDevicePtr() override { return ptrS7Server; }

    // 服务器特有的方法
    int WriteOutputs(const QByteArray& byte_array);
    int WriteInputs(const QByteArray& byte_array);
    int WriteMerkers(const QByteArray& byte_array);

    std::mutex& GetMutex() { return ptrS7Server->mtx; }
    TS7Server* GetS7Server() { return ptrS7Server->serverValue; }
    byte* GetDB1Buffer() { return db1_; }
    byte* GetInputsBuffer() { return inputs_; }
    byte* GetOutputsBuffer() { return outputs_; }
    byte* GetMerkersBuffer() { return merkers_; }

    int GetClientsCount() const {
        return ptrS7Server && ptrS7Server->serverValue ?
            ptrS7Server->serverValue->ClientsCount() : 0;
    }

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
    /// <param name="count"></param>
    //void ClientCountChanged(int count);

private:
    PtrSimensS7Server ptrS7Server{ nullptr };
    QString deviceName;
    QString ipValue;
    int portValue;
    byte* db1_;        // DB1数据区
    byte* inputs_;     // 输入区
    byte* outputs_;    // 输出区
    byte* merkers_;    // 位存储区
    tbb::concurrent_queue<QByteArray> queueReadByteArray;

    static void ServerCallback(void* usrPtr, PSrvEvent Event, int Size);
    static int RWAreaCallback(void* usrPtr, int Sender, int Operation, PS7Tag PTag, void* pUsrData);
};