#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <chrono>
#include <QDataStream>
#include <QString>

/// <summary>
/// 相机相关参数
/// </summary>
struct StdCameraParam
{
    std::string cameraType{};  //相机类型
    std::string cameraName{ "CameraDemo2D" };  //相机名称
    std::string cameraIP{ "" };                           //相机IP
    int cameraTriggerMode{ 0 };   //0，1，2，连续触发，软触发，硬触发
    float cameraExposure;   //曝光
    float cameraGain;   //增益
};


/// <summary>
/// 相机相关参数
/// </summary>
struct CameraParam
{
	QString cameraType{};  //相机类型
	QString cameraName{ "CameraDemo2D" };  //相机名称
	QString cameraIP{""};                           //相机IP
	int cameraTriggerMode{ 0 };   //0，1，2，连续触发，软触发，硬触发
	float cameraExposure;   //曝光
	float cameraGain;   //增益

	// 重写QDataStream& operator<<操作符，做数据序列化操作
	friend QDataStream& operator<<(QDataStream& stream, const CameraParam& param) {
		stream << param.cameraType;
		stream << param.cameraName;
		stream << param.cameraIP;
		stream << param.cameraTriggerMode;
		stream << param.cameraExposure;
		stream << param.cameraGain;
		return stream;
	}
	// 重写QDataStream& operator>>操作符，做数据反序列化操作
	friend QDataStream& operator>>(QDataStream& stream, CameraParam& param) {
		stream >> param.cameraType;
		stream >> param.cameraName;
		stream >> param.cameraIP;
		stream >> param.cameraTriggerMode;
		stream >> param.cameraExposure;
    	stream >> param.cameraGain;
		return stream;
	}
};
