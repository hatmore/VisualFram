#pragma once
#include <QJsonObject>
#include <QDialog>
#include "../GeneralStruct.h"
#include "ui_FramCameraSetParam.h"
#include "BaseCamera.h"


class FramCameraSetParam : public QDialog
{
	Q_OBJECT

public:
	FramCameraSetParam(QWidget *parent = nullptr);
	~FramCameraSetParam();

public:
	/// <summary>
	/// 序列化
	/// </summary>
	/// <returns></returns>
	QJsonObject SerializeCameraDevice();
	/// <summary>
	/// 反序列化化
	/// </summary>
	/// <param name="json"></param>
	void DeserializeCameraDevice(const QJsonObject & cameras_json);

    /// <summary>
    /// 获得所有相机的Key
    /// </summary>
    /// <returns></returns>
    const QList<QString> GetAllCameraDeviceName() const {
        return qMapCameraDevices.keys();
    }

    QMap<QString, BaseCamera::Ptr>* GetMapCameraDevices() {
        return &this->qMapCameraDevices;
    }

    std::shared_ptr<QMap<int, BaseCamera::Ptr>> GetMapIndexCameraDevices() {
        return this->ptrMapIndexCameraDevices;
    }


private:
	void Initial();

	void InitialTabWidget();

	void InitialPushButton();

	void InitialCombox();

	void InitialCommunicateDeviceParam();

	void InitialOtherForm();

    void InitialCameraFactory();

private slots:
	/// <summary>
	/// 添加相机
	/// </summary>
	void AddCameraDevice();
	/// <summary>
	/// 相机搜索
	/// </summary>
	void SearchCameraDevice();
	/// <summary>
	/// 链接相机
	/// </summary>
	void AddCamera();
	
	/// <summary>
	/// 断开相机 
	/// </summary>
	void DisConnectCamera();

	/// <summary>
	/// Item值发生改变
	/// </summary>
	/// <param name="row"></param>
	/// <param name="col"></param>
	void TabWidgetDoubleClicked(int row, int col);

    /// <summary>
    /// 单机删除
    /// </summary>
    /// <param name="row"></param>
    /// <param name="col"></param>
    void TabWidgetClicked(int row, int col);


private:


signals:
	void SendLogInfoSynSig(const QString& info, const LOGTYPE type);

private:
	Ui::FramCameraSetParamClass ui;
	QMap<QString, BaseCamera::Ptr> qMapCameraDevices;  //相机设备相关参数
    std::shared_ptr<QMap<int, BaseCamera::Ptr>> ptrMapIndexCameraDevices{nullptr};  //相机设备相关参数
};
