#pragma once
#include <QDialog>
#include <QMap> 
#include "CommDevicelnterface/CommDeviceFactory.h"
#include "CommDevicelnterface/CommDeviceInterface.h"
#include "CommDevicelnterface/CommDevicePlugin.h"
#include "../HardwareSetting.h"
#include "ui_FramCommunicateDevice.h"

class CommDevicePlugin;
using PtrCommDevicePlugin = std::shared_ptr<CommDevicePlugin>;

class FramCommunicateDevice : public QDialog
{
	Q_OBJECT

public:
	FramCommunicateDevice(QWidget *parent = nullptr);
	~FramCommunicateDevice();

	/// <summary>
    /// 序列化
    /// </summary>
    /// <returns></returns>
	QJsonObject SerializeCommunicateDevice();
	/// <summary>
	/// 反序列化化
	/// </summary>
	/// <param name="json"></param>
	void DeserializeCommunicateDevice(QJsonObject const& devices_json);

    /// <summary>
    /// 获得所有设备Key
    /// </summary>
    /// <returns></returns>
    const QList<QString> GetAllCommunicateDeviceName() const {
        return mapCommDeviceInterface.keys();
    }

    QMap<QString, CommDeviceInterface*> *GetMapCommDeviceInterface() {
        return &this->mapCommDeviceInterface;
    }


private:
	void Initial();
    /// <summary>
    /// 初始化外部通信插件
    /// </summary>
    void InitialCommDeviceFactory();

	void InitialTableWidget();

	void InitialPushButton();

	void InitialCombox();

	void InitialCamerParam();

	void InitialOtherForm();


private slots:
	/// <summary>
	/// 添加通信设备
	/// </summary>
	void AddCommDevice();
	/// <summary>
	/// 删除通信设备
	/// </summary>
	void DeleteCommDevice();
    /// <summary>
    /// Item值发生改变
    /// </summary>
    /// <param name="row"></param>
    /// <param name="col"></param>
    void TabWidgetDoubleClicked(int row, int col);


signals:
	void SendLogInfoSig(const QString& info, const LOGTYPE type);

private:
	Ui::FramCommunicateDeviceClass ui;
    CommDeviceFactory::Ptr ptrCommDeviceFactory{ nullptr };
    QMap<QString, PtrCommDevicePlugin> mapPtrCommDevicePlugin;
    QMap<QString, CommDeviceInterface*> mapCommDeviceInterface;
};
