#pragma once
#include <QDialog>
#include <QMap>
#include <QTimer>
#include <QStringListModel>
#include "BaseCommunicateDevice.h"
#include "CommunicateStruct.h"
#include "../GeneralStruct.h"
#include "SocketTcpServerCommunicate.h"
#include "SimensS7ServerCommunication.h"
#include "SimensS7ClientCommunication.h"
#include "ui_FramCommunicateDevice.h"

enum class CommunicationType {
    SimensS7,       // 0
    TCP_IP,       // 1
};

class FramCommunicateDevice : public QDialog
{
    Q_OBJECT
public:
    FramCommunicateDevice(QWidget* parent = nullptr);
    ~FramCommunicateDevice();

    QJsonObject SerializeCommunicateDevice();
    void DeserializeCommunicateDevice(QJsonObject const& devices_json);

    const BaseCommunicateDevice::Ptr GetBaseCommunicateDevice() const {
        return this->ptrCommDeviceInterface;
    }

private:
    void Initial();
    void InitialUISet();
    void InitialTabWidget();
    void InitialPushButton();
    void InitialCommunicateDeviceParam();
    void InitialInitialBottonMenu();
    void AddSignaleEvnet(const QString& action_text);
    void AddToLogList(const QString& message);

private slots:
    void OpenCommDevice();
    void CloseCommDevice();
    void MenuActionsSlot();
    void OnReadData();
    void OnLogReceived(const QString& info, LOGTYPE type); // 处理日志显示
    void OnClientCountChanged(int count);

signals:
    void SendLogInfoSynSig(const QString& info, const LOGTYPE type);

private:
    // S7 DB区域通信参数获取
    int GetS7DBNumber() const;
    int GetS7Start() const;
    int GetS7Size() const;
    QByteArray GetS7WriteData() const;

    // 数据格式转换
    QString FormatHexData(const QByteArray& data) const;
    bool ValidateWriteData(const QString& hexData) const;

private:
    Ui::FramCommunicateDeviceClass ui;
    BaseCommunicateDevice::Ptr ptrCommDeviceInterface{ nullptr };
    CGDevice::Ptr ptrCGDevice{ nullptr };
    SimensS7ServerCommunication::Ptr ptrS7ServerComm{ nullptr };
    SimensS7ClientCommunication::Ptr ptrS7ClientComm{ nullptr };

    QVector<QString> qvStrCommunicateName;
    QStringListModel* listModel{ nullptr };
};