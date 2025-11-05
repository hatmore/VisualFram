#pragma once
#include <QWidget>
#include <QJsonObject>

class CommDeviceInterface : public QWidget
{
    Q_OBJECT
public:
    CommDeviceInterface(QWidget* parent = nullptr) :QWidget(parent) {};
    virtual ~CommDeviceInterface() {};
    /// <summary>
    /// 初始化设备
    /// </summary>
    /// <returns></returns>
    virtual int InitalDevice() = 0;
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
    const int GetCommDiviceState() const {
        return this->commDiviceState;
    }
    /// <summary>
    /// 序列化
    /// </summary>
    /// <returns></returns>
    virtual QJsonObject SerializeDevice() = 0;
    /// <summary>
    /// 反序列化
    /// </summary>
    /// <param name="json"></param>
    virtual void ExserializeDevice(const QJsonObject& json) = 0;
    /// <summary>
    /// 设备名称
    /// </summary>
    /// <param name="name"></param>
    void SetDeviceName(const QString& name) {
        this->selfDeviceName = name;
    }
    /// <summary>
    /// 获得设备名称
    /// </summary>
    /// <returns></returns>
    const QString GetDeviceName() const {
        return this->selfDeviceName;
    }

protected:
    QString selfDeviceName{ "SerialPort" };
    int commDiviceState{ -1 };
};


