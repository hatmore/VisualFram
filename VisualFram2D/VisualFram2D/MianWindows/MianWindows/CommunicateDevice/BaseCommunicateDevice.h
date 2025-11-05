#pragma once
#include "CommunicateStruct.h"
#include <QWidget>
#include <QJsonObject>

class BaseCommunicateDevice: public QObject
{
    Q_OBJECT
public:
    BaseCommunicateDevice() = default;
    virtual ~BaseCommunicateDevice() = default;
    using Ptr = std::shared_ptr<BaseCommunicateDevice>;
    using ConstPtr = std::shared_ptr<const BaseCommunicateDevice>;

    /// <summary>
    /// 初始化设备
    /// </summary>
    /// <returns></returns>
    virtual int InitalDevice(const CGDevice::Ptr) = 0;

    /// <summary>
    /// 打开设备
    /// </summary>
    /// <returns></returns>
    virtual int OpenDevice() = 0;

    /// <summary>
    /// 断开设备
    /// </summary>
    /// <returns></returns>
    virtual int CloseDevice() = 0;

    /// <summary>
    /// 写数据
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    virtual int Write(const QByteArray&) = 0;

    /// <summary>
    /// 读数据
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    virtual int Read(QByteArray&) = 0;

    /// <summary>
    /// 获得设备状态，0表示正常打开，-1为未打开，
    /// </summary>
    /// <returns></returns>
    virtual bool GetCommDiviceState() = 0;

    /// <summary>
    /// 返回设备指针
    /// </summary>
    /// <returns></returns>
    virtual  CGDevice::Ptr GetCGDevicePtr() = 0;


protected:
    bool commDiviceState{ false};
};


