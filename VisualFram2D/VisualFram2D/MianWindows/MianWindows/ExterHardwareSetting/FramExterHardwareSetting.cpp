#include "FramExterHardwareSetting.h"
#include <QVBoxLayout>

FramExterHardwareSetting::FramExterHardwareSetting(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	Initial();
}

FramExterHardwareSetting::~FramExterHardwareSetting()
{}

void FramExterHardwareSetting::Initial()
{
	InitialFunciton();
}

void FramExterHardwareSetting::InitialFunciton()
{
	framCameraSetParam = new FramCameraSetParam();
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(framCameraSetParam);
	layout->setContentsMargins(5, 5, 5, 5);
	ui.tabCameraDevice->setLayout(layout);

	framCommunicateDevice = new FramCommunicateDevice();
	QVBoxLayout* layout1 = new QVBoxLayout();
	layout1->addWidget(framCommunicateDevice);
	layout1->setContentsMargins(5, 5, 5, 5);
	ui.tabCommunicateDevice->setLayout(layout1);
}

QJsonObject FramExterHardwareSetting::SerializeExterHardware()
{
     auto camera_json = framCameraSetParam->SerializeCameraDevice();
     auto comm_device_json = framCommunicateDevice->SerializeCommunicateDevice();

     QJsonObject hardware_json;
     hardware_json["CamerasSet"] = camera_json;
     hardware_json["CommunicateDevices"] = comm_device_json;

    return hardware_json;
}

void FramExterHardwareSetting::DeserializeExterHardware(QJsonObject const& hardware_json)
{
    auto camera_json = hardware_json["CamerasSet"].toObject();
    auto comm_device_json = hardware_json["CommunicateDevices"].toObject();

    framCameraSetParam->DeserializeCameraDevice(camera_json);
    framCommunicateDevice->DeserializeCommunicateDevice(comm_device_json);
}