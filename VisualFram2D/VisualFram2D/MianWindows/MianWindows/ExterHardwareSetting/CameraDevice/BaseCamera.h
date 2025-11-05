#pragma once
#include "CameraStruct.h"

class BaseCamera
{
public:
	BaseCamera() = default;
	~BaseCamera() = default;
	using Ptr = std::shared_ptr<BaseCamera>;
	using ConstPtr = std::shared_ptr<const BaseCamera>;

	/// <summary>
	/// 初始化
	/// </summary>
	/// <returns></returns>
	virtual int MvInitialize(std::vector<std::string>& device_list) = 0;

	/// <summary>
	/// 打开对应相机
	/// </summary>
	/// <param name="index"> 由idex 控制</param>
	/// <returns></returns>
	virtual int MvOpenDevice(const std::string& name) = 0;

	/// <summary>
	/// 关闭相机
	/// </summary>
	virtual void MvCloeDevice() = 0;

	/// <summary>
	/// 设置相机参数
	/// </summary>
	/// <returns></returns>
	virtual int MvSetConfig(const StdCameraParam& cam_param) = 0;

	/// <summary>
	/// 获取相机参数
	/// </summary>
	/// <returns></returns>
	virtual const StdCameraParam& MvGetConfig() {
		return this->cameraParam;
	}

	/// <summary>
	/// 获得设备名称
	/// </summary>
	/// <returns></returns>
	virtual const std::string GetDeviceName() const {
		return this->deviceName;
	}

	/// <summary>
	/// 获得相机状态
	/// </summary>
	/// <returns></returns>
	virtual const bool GetCameraState() const {
		bool state = atomicCameraState.load();
		return state;
	}

	/// <summary>
	///判断相机的链接状态 
	/// </summary>
	/// <returns></returns>
	virtual const bool GetCameraConnectState() const {
		bool state = atomicIsConnect.load();
		return state;
	}


	/// <summary>
	/// 获得图像数据
	/// </summary>
	/// <param name=""></param>
	virtual void MvGetImage(cv::Mat&) = 0;

	/// <summary>
	/// 软触发一次
	/// </summary>
	/// <returns></returns>
	virtual int MvSoftTrigger() = 0;

protected:
	std::string deviceName;
    StdCameraParam cameraParam;                             //相机参数
	std::atomic<bool> atomicCameraState{ false };  //获得相机当前状态，false不能拍摄，true，可以拍摄
	std::atomic_bool atomicIsConnect{false};   //判断相机是否成功链接
};

