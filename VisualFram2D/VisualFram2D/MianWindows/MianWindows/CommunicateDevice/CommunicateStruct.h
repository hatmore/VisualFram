#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QObject>
#include <mutex>
#include "snap7.h"

/// <summary>
/// 通信类型
/// </summary>
enum ConnectState {
    connectState = 0,          //正在链接
    disconnectState = 1,     //断开链接
};

/// <summary>
/// 通信类型
/// </summary>
enum DeviceMode {
    nullPtr = -1,
    tcpIP = 0,
    tcpServer = 1,
    tcpClient = 2,
    serialPort = 3,
    S7Server = 4,
    S7Client = 4,
};

//根目录，设备根目录
class CGDevice :public QObject
{
public:
    using Ptr = std::shared_ptr<CGDevice>;
    CGDevice() : deviceMode(DeviceMode::nullPtr) { ; }
    CGDevice(const DeviceMode& device_mode) : deviceMode(device_mode) { ; }

    QString deviceName;           //devive名称
    int deviceId;        //设备ID
    ConnectState connectState;   //链接状态
    QString remarkValue;
    std::mutex mtx;                  //设备锁
    const DeviceMode deviceMode;
};
using PtrCGDevice = std::shared_ptr<CGDevice>;

class SocketTcpServer : public CGDevice //TCP/IP服务器
{
public:
    using Ptr = std::shared_ptr<SocketTcpServer>;
    SocketTcpServer() : CGDevice(DeviceMode::tcpServer) { ; }
    ~SocketTcpServer() {
        if (serverValue != nullptr) {
            if (serverValue->isListening()) {
                serverValue->close();
                for (auto socket : qVclientValue)  {
                    socket->close();
                    socket->deleteLater();
                }
                qVclientValue.clear();
                serverValue->deleteLater();
            }
        }
    }
   void  Close() {
        if (serverValue != nullptr) {
            if (serverValue->isListening()) {
                serverValue->close();
                for (auto socket : qVclientValue) {
                    socket->close();
                    socket->deleteLater();
                }
                qVclientValue.clear();
                serverValue->deleteLater();
                serverValue = nullptr;
            }
        }
    }
    QTcpServer* serverValue{ nullptr };
    QTcpSocket* clientValue{ nullptr };
    QVector<QTcpSocket*> qVclientValue;
    QString ipValue;
    int portValue;
};
using PtrSocketTcpServer = std::shared_ptr<SocketTcpServer>;

class SimensS7Server : public CGDevice
{
public:
    using Ptr = std::shared_ptr<SimensS7Server>;
    SimensS7Server() : CGDevice(DeviceMode::S7Server) { }
    ~SimensS7Server() {
        if (serverValue != nullptr) {
            serverValue->Stop();
            delete serverValue;
            serverValue = nullptr;
        }
    }
    void Close() {
        if (serverValue != nullptr) {
            serverValue->Stop();
            delete serverValue;
            serverValue = nullptr;
        }
    }

    TS7Server* serverValue{ nullptr };
    QString ipValue;
    int portValue;
};
using PtrSimensS7Server = std::shared_ptr<SimensS7Server>;

class SimensS7Client : public CGDevice
{
public:
    using Ptr = std::shared_ptr<SimensS7Client>;

    SimensS7Client() : CGDevice(DeviceMode::S7Client) { }

    ~SimensS7Client() {
        if (clientValue != nullptr) {
            clientValue->Disconnect();
            delete clientValue;
            clientValue = nullptr;
        }
    }

    void Close() {
        if (clientValue != nullptr) {
            clientValue->Disconnect();
        }
        connectState = disconnectState;
    }

    TS7Client* clientValue{ nullptr };
    QString ipValue;
    int portValue{ 102 };
    int rackValue{ 0 };
    int slotValue{ 1 };
};
using PtrSimensS7Client = std::shared_ptr<SimensS7Client>;