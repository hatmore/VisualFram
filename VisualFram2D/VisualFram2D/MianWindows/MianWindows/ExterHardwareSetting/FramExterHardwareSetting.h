#pragma once
#include <QDialog>
#include "CameraDevice/FramCameraSetParam.h"
#include "CommunicateDevice/FramCommunicateDevice.h"
#include "ui_FramExterHardwareSetting.h"

class FramExterHardwareSetting : public QDialog
{
	Q_OBJECT

public:
	FramExterHardwareSetting(QWidget *parent = nullptr);
	~FramExterHardwareSetting();

    /// <summary>
    /// 序列化
    /// </summary>
    /// <returns></returns>
    QJsonObject SerializeExterHardware();
    /// <summary>
    /// 反序列化化
    /// </summary>
    /// <param name="json"></param>
    void DeserializeExterHardware(QJsonObject const& hardware_json);

    /// <summary>
    /// 获得所有相机的Key
    /// </summary>
    /// <returns></returns>
    const QList<QString> GetAllCameraDeviceName() const {
        return framCameraSetParam->GetAllCameraDeviceName();
    }

    /// <summary>
    /// 获得所有设备Key
    /// </summary>
    /// <returns></returns>
    const QList<QString> GetAllCommunicateDeviceName() const {
        return framCommunicateDevice->GetAllCommunicateDeviceName();
    }

     QDialog* GetFramCameraSetParam() {
        return this->framCameraSetParam;
    }

     QDialog* GetFramCommunicateDevice() {
        return this->framCommunicateDevice;
    }

private:
	void Initial();
	void InitialFunciton();

private:
	Ui::FramExterHardwareSettingClass ui;

	FramCameraSetParam* framCameraSetParam{nullptr};
	FramCommunicateDevice* framCommunicateDevice{nullptr};
};
