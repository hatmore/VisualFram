#pragma once
#include <string>
#include <direct.h>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <regex>
#include <tbb\concurrent_queue.h>
#include "CameraStruct.h"


class BaseCamera
{
public:
    BaseCamera() = default;
    virtual ~BaseCamera() = default;
    using Ptr = std::shared_ptr<BaseCamera>;
    using ConstPtr = std::shared_ptr<const BaseCamera>;

    /// <summary>
    /// 打开对应相机
    /// </summary>
    /// <param name="index"> 由idex 控制</param>
    /// <returns></returns>
    virtual bool MvOpenDevice(const  CameraParam& camera_param) = 0;

    /// <summary>
    /// 关闭相机
    /// </summary>
    virtual void MvCloeDevice() = 0;

    /// <summary>
    /// 开始采集数据
    /// </summary>
    virtual void StartPlay() = 0;

	/// <summary>
    ///暂停采集数据
    /// </summary>
	virtual void StopPlay() = 0;

    /// <summary>
    /// 设定设备名称
    /// </summary>
    /// <returns></returns>
    virtual void SetDeviceParam(const  CameraParam &camera_param)  {
        this->cameraParam = camera_param;
    }

    virtual const CameraParam GetDeviceParam() const{
        return this->cameraParam;
    }
    /// <summary>
    ///判断相机的链接状态 
    /// </summary>
    /// <returns></returns>
    virtual const bool GetCameraConnectState() const = 0;

	/// <summary>
	/// 获得图像数据
	/// </summary>
	/// <param name=""></param>
	virtual bool MvGetImage(cv::Mat&) = 0;

    /// <summary>
    /// 开始录像
    /// </summary>
    /// <returns></returns>
    virtual bool StartRecord() = 0;

    /// <summary>
    /// 暂停录像
    /// </summary>
    /// <returns></returns>
    virtual bool StopRecord() = 0;


protected:
    // 创建目录，如果目录不存在则创建
    bool CreateDirectory(const std::string& path) {
#if defined(_WIN32)
        return _mkdir(path.c_str()) == 0;
#else
        return mkdir(path.c_str(), 0733) == 0;
#endif
    }

    // 生成带时间戳的文件名
    std::string GenerateFilename(const std::string& output_dir) {
        time_t now = time(nullptr);
        struct tm localTime;
        localtime_s(&localTime, &now);  // ✅ Windows 安全版本
        std::stringstream ss;
        ss << output_dir << "/";
        ss << std::put_time(&localTime, "%Y-%m-%d-%H_%M_%S");
        ss <<".mp4";
        return ss.str();
    }

	RtspUrl ParseRtspUrl(const std::string& url) {
		// 正则：协议://用户名:密码@IP:端口/路径
		std::regex re(R"(^(rtsp)://([^:]+):([^@]+)@([^:/]+):?(\d*)(/.*)$)");
		std::smatch match;
		RtspUrl result;
		if (std::regex_match(url, match, re)) {
			result.protocol = match[1];
			result.userName = match[2];
			result.password = match[3];
			result.ip = match[4];
			if (match[5].matched && !match[5].str().empty()) {
				result.port = std::stoi(match[5]);
			}
			result.path = match[6];
		}
		else {
			throw std::runtime_error("Invalid RTSP URL format: " + url);
		}

		return result;
	}


protected:
    CameraParam cameraParam;   //相机参数
	std::atomic_bool atomicIsConnect{false};   //判断相机是否成功链接
    tbb::concurrent_queue<cv::Mat> ptrQueueCameraSaveData;
};

