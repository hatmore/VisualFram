#pragma once
#include <QJsonObject>
#include <QDialog>
#include "CameraFactory.h"
#include "../HardwareSetting.h"
#include "ui_FramCameraSetParam.h"

typedef  void (*CameraRegister)(CameraFactory*);

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

private:
	void Initial();

	void InitialTableWidget();

	void InitialPushButton();

	void InitialCombox();

	void InitialCamerParam();

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
	void ConnectCamera();
	
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


private:
    StdCameraParam QParam2StdParam (const CameraParam& param) {
        StdCameraParam std_param;
        std_param.cameraType = param.cameraType.toStdString();
        std_param.cameraName = param.cameraName.toStdString();
        std_param.cameraIP = param.cameraIP.toStdString();
        std_param.cameraTriggerMode = param.cameraTriggerMode;
        std_param.cameraExposure = param.cameraExposure;
        std_param.cameraGain = param.cameraGain;
        return std_param;
    }

    CameraParam StdParam2QParam(const StdCameraParam& param) {
        CameraParam q_param;
        q_param.cameraType = QString::fromStdString(param.cameraType);
        q_param.cameraName = QString::fromStdString(param.cameraName);
        q_param.cameraIP = QString::fromStdString(param.cameraIP);
        q_param.cameraTriggerMode = param.cameraTriggerMode;
        q_param.cameraExposure = param.cameraExposure;
        q_param.cameraGain = param.cameraGain;
        return q_param;
    }

signals:
	void SendLogInfoSig(const QString& info, const LOGTYPE type);

private:
	Ui::FramCameraSetParamClass ui;

    CameraFactory::Ptr ptrCameraFactory{nullptr}; //相机工厂指针

	QMap<QString, BaseCamera::Ptr> qMapCameraDevices;  //相机设备相关参数
};
