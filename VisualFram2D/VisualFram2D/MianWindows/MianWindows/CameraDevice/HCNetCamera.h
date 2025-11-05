#pragma once
#include "Windows.h"
#include "HCNetSDK.h"
#include "plaympeg4.h"
#include "BaseCamera.h"
#include <tbb/concurrent_unordered_map.h>
#include <tbb\concurrent_queue.h>
#include <opencv2\opencv.hpp>
using namespace std;
using namespace cv;

class HCNetCamera : public BaseCamera
{
public:
	using Ptr = std::shared_ptr<HCNetCamera>;
	using UniquePtr = std::unique_ptr<HCNetCamera>;
	HCNetCamera();
	virtual ~HCNetCamera();

	// 打开相机
	bool MvOpenDevice(const CameraParam& camera_param) override;
	// 关闭相机
	void MvCloeDevice() override;
	
	/// <summary>
	/// 开始采集数据
	/// </summary>
	virtual void StartPlay()override;

	/// <summary>
	///暂停采集数据
	/// </summary>
	virtual void StopPlay()override;

	// 判断相机连接状态
	const bool GetCameraConnectState() const override;
	// 获取图像数据
	bool MvGetImage(cv::Mat& image) override;

	/// <summary>
	/// 开始录像
	/// </summary>
	/// <returns></returns>
	virtual bool StartRecord() override;

	/// <summary>
	/// 暂停录像
	/// </summary>
	/// <returns></returns>
	virtual bool StopRecord() override;

	// 可选：重载设备参数设置
	void SetDeviceParam(const CameraParam& camera_param) override {
		this->cameraParam = camera_param;
	}
	const CameraParam GetDeviceParam() const override {
		return this->cameraParam;	
	}


private:
	// 静态回调（用于 SDK）
	static void CALLBACK RealDataCallBack(LONG lRealHandle, DWORD dwDataType, BYTE* pBuffer, DWORD dwBufSize, void* pUser);
	static void CALLBACK DecCBFun(LONG nPort, char* pBuf, long nSize, FRAME_INFO* pFrameInfo, long pUser, long nReserved2);
	static void CALLBACK ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void* pUser);
	static long nStaticUser;  //使用者的IP
	static std::unique_ptr<tbb::concurrent_unordered_map<long,std::shared_ptr<tbb::concurrent_queue<cv::Mat>>>> unPtrMapImageQueue; //图像队列

private:
	LONG lUserID {-1};  //id
	NET_DVR_DEVICEINFO_V30 struDeviceInfo; //参数
	NET_DVR_PREVIEWINFO struPlayInfo = { 0 };
	LONG lRealPlayHandle{-1};
	LONG lRealSaveHandle{ -1 };
	NET_DVR_CLIENTINFO ClientInfo;
	bool mbIsRecording{ false };  //是否正在录像
	LONG lPort; //全局的播放库port号
	std::shared_ptr<tbb::concurrent_queue<cv::Mat>> ptrImageQueue{nullptr};
	long thisUserId{0};  //本节点的相机IP

    int cameraPort = 8000;
};

