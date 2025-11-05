#pragma once
#include <QObject>
#include <tbb\concurrent_queue.h>
#include "BaseCommunicateDevice.h"
#include "CommunicateStruct.h"
#include "../GeneralStruct.h"

class SocketTcpServerCommunicate : public BaseCommunicateDevice
{
    Q_OBJECT
public:
    SocketTcpServerCommunicate();
    virtual ~SocketTcpServerCommunicate();
    using Ptr = std::shared_ptr<SocketTcpServerCommunicate>;
    using ConstPtr = std::shared_ptr<const SocketTcpServerCommunicate>;

    /// <summary>
    /// 初始化设备
    /// </summary>
    /// <returns></returns>
    virtual int InitalDevice(const CGDevice::Ptr ptr_device);

    /// <summary>
    /// 打开设备
    /// </summary>
    /// <returns></returns>
    virtual int OpenDevice();

    /// <summary>
    /// 断开设备
    /// </summary>
    /// <returns></returns>
    virtual int CloseDevice();

    /// <summary>
    /// 写数据
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    virtual int Write(const QByteArray& byte_array);

    /// <summary>
    /// 读数据
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    virtual int Read(QByteArray& byte_array);

    /// <summary>
    /// 获得设备状态，0表示正常打开，-1为未打开，
    /// </summary>
    /// <returns></returns>
    virtual bool GetCommDiviceState() {
        return commDiviceState;
    }

    /// <summary>
    /// 返回设备指针
    /// </summary>
    /// <returns></returns>
    virtual  CGDevice::Ptr GetCGDevicePtr() {\
        return this->ptrSocketTcpServer;
    }
signals:

    void SendLogInfoSynSig(const QString& info, const LOGTYPE type);

private:
    PtrSocketTcpServer ptrSocketTcpServer{nullptr};
    QTcpServer* tcpSocketSever{nullptr};
    QString deviceName;
    QString ipValue;
    int portValue;
    tbb::concurrent_queue<QByteArray> queueReadByteArray;
};

 