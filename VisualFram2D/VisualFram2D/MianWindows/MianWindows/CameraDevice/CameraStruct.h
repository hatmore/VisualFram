#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <chrono>
#include <QDataStream>
#include <QString>

struct RtspUrl {
	std::string protocol;
	std::string userName;
	std::string password;
	std::string ip;
	int port = 8000;  // 默认 RTSP 端口
	std::string path;
};

struct CameraParam
{
    QString deviceName{ "HiKang" };
    QString strURL{"rtsp://admin:jee12345$@192.168.250.64:554/h264/ch1/main/av_stream"};   //相机收据的地址
    QString savePath{"E:\\Save"};  //保存地址
    bool isSave{ true };   //是否保存数据
    int camerIndex{ 0 };  //相机Index
    int saveTime{ 500 };   //保存时间
    int sampleFrequencyTime{5}; //采样间隔
    bool virtualCamera{ false }; //是不是虚拟相机

    friend QDataStream& operator<<(QDataStream& stream, const CameraParam& param) {
        stream << param.deviceName;
        stream << param.strURL;
        stream << param.savePath;
        stream << param.isSave;
        stream << param.camerIndex;
        stream << param.saveTime;
        stream << param.virtualCamera;
        stream << param.sampleFrequencyTime;
        return stream;
    }

    friend QDataStream& operator>>(QDataStream& stream, CameraParam& param) {
        stream >> param.deviceName;
        stream >> param.strURL;
        stream >> param.savePath;
        stream >> param.isSave;
        stream >> param.camerIndex;
        stream >> param.saveTime;
        stream >> param.virtualCamera;
        stream >> param.sampleFrequencyTime;
        return stream;
    }
};
