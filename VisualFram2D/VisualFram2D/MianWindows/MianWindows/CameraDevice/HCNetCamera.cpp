#include "HCNetCamera.h"

long HCNetCamera::nStaticUser = 0;
std::unique_ptr<tbb::concurrent_unordered_map<long, std::shared_ptr<tbb::concurrent_queue<cv::Mat>>>>  HCNetCamera::unPtrMapImageQueue = 
                                                                  std::make_unique<tbb::concurrent_unordered_map<long, std::shared_ptr<tbb::concurrent_queue<cv::Mat>>>>();

/// 实时流回调（静态）
void CALLBACK HCNetCamera::RealDataCallBack(LONG, DWORD dwDataType, BYTE* pBuffer, DWORD dwBufSize, void* puser)
{
	if (dwDataType != NET_DVR_STREAMDATA) return;
	HCNetCamera* hc_net_camera = (HCNetCamera*)puser;
	if (dwBufSize > 0 && hc_net_camera->lPort != -1) {
		// 缓冲区可能满，短暂重试几次
		int retry = 0;
		while (!PlayM4_InputData(hc_net_camera->lPort, pBuffer, dwBufSize) && retry < 3) {
			std::this_thread::sleep_for(std::chrono::microseconds(200));
			++retry;
		}
	}
}

/// 解码回调 视频为YUV数据(YV12)，音频为PCM数据（静态）
void CALLBACK HCNetCamera::DecCBFun(LONG nPort, char* pBuf, long nSize, FRAME_INFO* pFrameInfo, long user, long nReserved2)
{
	if (!pFrameInfo)
		return;
	long lFrameType = pFrameInfo->nType;
	if (lFrameType == T_YV12) {
		Mat YUVImage(pFrameInfo->nHeight + pFrameInfo->nHeight / 2, pFrameInfo->nWidth, CV_8UC1, (unsigned char*)pBuf);
		cv::Mat rgb_image;
		cvtColor(YUVImage, rgb_image, COLOR_YUV2BGR_YV12);
		//std::shared_ptr<tbb::concurrent_queue<cv::Mat>> ptrImageQueue = (*HCNetCamera::unPtrMapImageQueue)[user];
		auto it = HCNetCamera::unPtrMapImageQueue->find(user);
		if (it != HCNetCamera::unPtrMapImageQueue->end()) {
			it->second->push(rgb_image.clone());
		}
	}
}

/// 异常回调（静态）
void CALLBACK HCNetCamera::ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void* pUser)
{
	char tempbuf[256] = { 0 };
	switch (dwType)	{
	case EXCEPTION_RECONNECT:    //预览时重连
		printf("----------reconnect--------%d\n", time(NULL));
		break;
	default:
		break;
	}
}

HCNetCamera::HCNetCamera()
{
	NET_DVR_Init();
	HCNetCamera::nStaticUser = HCNetCamera::nStaticUser  + 1;
	thisUserId = nStaticUser;
	ptrImageQueue = std::make_shared<tbb::concurrent_queue<cv::Mat>>();
	HCNetCamera::unPtrMapImageQueue->insert({ thisUserId, ptrImageQueue });
}

HCNetCamera::~HCNetCamera()
{
	MvCloeDevice();
	auto it = HCNetCamera::unPtrMapImageQueue->find(thisUserId);
	if (it != HCNetCamera::unPtrMapImageQueue->end()) {
		HCNetCamera::unPtrMapImageQueue->unsafe_erase(it);
	}
}

bool HCNetCamera::MvOpenDevice(const CameraParam& camera_param)
{
	cameraParam = camera_param;
	std::string url = cameraParam.strURL.toStdString();
	RtspUrl rtsp_url = ParseRtspUrl(url);

	//char ip[] = "192.168.250.64";
	//char user[] = "admin";
	//char pwd[] = "jee12345$";
	//int port = 8000;
	//lUserID = NET_DVR_Login_V30(ip, port, user, pwd, &struDeviceInfo);

	NET_DVR_USER_LOGIN_INFO struLoginInfo = { 0 };
	struLoginInfo.bUseAsynLogin = 0; //同步登录方式
	struLoginInfo.wPort = 8000; //设备服务端口
    strcpy_s(struLoginInfo.sDeviceAddress, sizeof(struLoginInfo.sDeviceAddress), rtsp_url.ip.c_str());
    strcpy_s(struLoginInfo.sUserName, sizeof(struLoginInfo.sUserName), rtsp_url.userName.c_str());
    strcpy_s(struLoginInfo.sPassword, sizeof(struLoginInfo.sPassword), rtsp_url.password.c_str());
	//设备信息, 输出参数
	NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = { 0 };
	lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);


	ClientInfo.hPlayWnd = NULL;
	ClientInfo.lChannel = 1;
	ClientInfo.lLinkMode = 0;
	ClientInfo.sMultiCastIP = NULL;

	if (lUserID < 0) {
		printf("Login error, %d\n", NET_DVR_GetLastError());
		NET_DVR_Cleanup();
		return false;
	}
	//设置异常消息回调函数
	NET_DVR_SetExceptionCallBack_V30(0, NULL, HCNetCamera::ExceptionCallBack, this);

	if (!PlayM4_GetPort(&lPort)) {
		std::cout << "fail get port" << std::endl;
		return false;
	}

	if (!PlayM4_SetStreamOpenMode(lPort, STREAME_REALTIME)) {
		std::cout << "fail set open mode" << std::endl;
		return false;
	}

	if (!PlayM4_OpenStream(lPort, NULL, 0, 1024 * 1024)) {
		std::cout << "fail open stream" << std::endl;
		return false;
	}

	if (!PlayM4_SetDecCallBackExMend(lPort, HCNetCamera::DecCBFun, NULL, 0, thisUserId)) {
		std::cout << "fail set callback" << std::endl;
		return false;
	}

	if (!PlayM4_Play(lPort, NULL)) {
		std::cout << "fail play" << std::endl;
		return false;
	}

	atomicIsConnect = true;
	return true;
}

void HCNetCamera::MvCloeDevice()
{
	// 停止播放库
	if (lPort != -1) {
		PlayM4_StopSound();
		PlayM4_Stop(lPort);
		PlayM4_CloseStream(lPort);
		PlayM4_FreePort(lPort);
		lPort = -1;
	}

	// 停止预览
	if (lRealPlayHandle != -1) {
		NET_DVR_StopRealPlay(lRealPlayHandle);
		lRealPlayHandle = -1;
	}

	// 停止保存
	if (lRealSaveHandle != -1) {
		NET_DVR_StopRealPlay(lRealSaveHandle);
		lRealSaveHandle = -1;
	}

	if (lUserID != -1) {
		NET_DVR_Logout_V30(lUserID);
		lUserID = -1;
	}

	NET_DVR_Cleanup();
	atomicIsConnect = false;
}

void HCNetCamera::StartPlay()
{
	if (lRealPlayHandle != -1) {
		return;
	}
	lRealPlayHandle = NET_DVR_RealPlay_V30(lUserID, &ClientInfo, HCNetCamera::RealDataCallBack, this, TRUE);
	if (-1 == lRealPlayHandle){
		DWORD err = NET_DVR_GetLastError();
		printf("NET_DVR_RealPlay_V30 failed, error code: %d\n", err);
	}
}

void HCNetCamera::StopPlay()
{
	if (lRealPlayHandle != -1) {
		if (mbIsRecording)  //正在录像，先停止
		{
			StopRecord();
		}
		NET_DVR_StopRealPlay(lRealPlayHandle);
		lRealPlayHandle = -1;
	}
}

const bool HCNetCamera::GetCameraConnectState() const
{
	return atomicIsConnect;
}

bool HCNetCamera::MvGetImage(cv::Mat& image)
{
	if (!ptrImageQueue->empty()) {
		if (ptrImageQueue->try_pop(image)) {
			return true;
		}
	}
	std::this_thread::sleep_for(std::chrono::microseconds(40));
	return false;
}

bool HCNetCamera::StartRecord()
{
	if (lRealSaveHandle != -1)
		return false;

	NET_DVR_LOCAL_GENERAL_CFG struGeneralCfg = { 0 }; // 初始化结构体，确保所有成员清零
	struGeneralCfg.byNotSplitRecordFile = 0; // 设置不切片
	struGeneralCfg.i64FileSize = cameraParam.saveTime * 1024 * 1024U;

	// 调用 NET_DVR_SetSDKLocalCfg 进行设置
	if (!NET_DVR_SetSDKLocalCfg(NET_DVR_LOCAL_CFG_TYPE_GENERAL, &struGeneralCfg)) {
		DWORD dwError = NET_DVR_GetLastError();
		// 处理错误，打印或记录错误码 dwError
		printf("NET_DVR_SetSDKLocalCfg failed, error code: %d\n", dwError);
	}

	lRealSaveHandle = NET_DVR_RealPlay_V30(lUserID, &ClientInfo,NULL, NULL, TRUE);
	if (-1 == lRealSaveHandle) {
		DWORD err = NET_DVR_GetLastError();
		printf("NET_DVR_RealPlay_V30 failed, error code: %d\n", err);
	}

	QString save_path = cameraParam.savePath;
	std::string output_file = GenerateFilename(save_path.toStdString());
	char output_file_arr[256] = { 0 }; // 根据实际需要分配长度
    strncpy_s(output_file_arr, sizeof(output_file_arr), output_file.c_str(), _TRUNCATE);

	if (!NET_DVR_SaveRealData(lRealSaveHandle, output_file_arr)){
		return false;
	}
	return true;
}

bool HCNetCamera::StopRecord()
{
	if (!NET_DVR_StopSaveRealData(lRealSaveHandle)) {
		return false;
	}
	NET_DVR_StopRealPlay(lRealSaveHandle);
	lRealSaveHandle = -1;
	mbIsRecording = false;
	return true;
}