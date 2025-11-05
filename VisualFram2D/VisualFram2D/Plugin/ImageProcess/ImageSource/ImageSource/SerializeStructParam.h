#pragma once
#include <QObject>

enum IMAGESOURCE {
    SourceLocalImage = 0,
    SourceLocalImageDir = 1,
    SourceImageCamera = 2,
};
/// <summary>
    /// 函数运行参数
    /// </summary>
struct FunctionParam {
    IMAGESOURCE sourceImagType;
    QString sourceImagePath;  //单张图像路径
    QString sourceImageDir;    //文件夹路径

    QString IP{ "192.1680.0.0" };
    QString cameraName{ "HaiKang" };
    int timeOut{ 1000 * 10 }; //10s超时时间

    // 重写QDataStream& operator<<操作符，做数据序列化操作
    friend QDataStream& operator<<(QDataStream& stream, const FunctionParam& param) {
        stream << param.sourceImagType;
        stream << param.sourceImagePath;
        stream << param.sourceImageDir;
        stream << param.IP;
        stream << param.cameraName;
        stream << param.timeOut; 
        return stream;
    }
    // 重写QDataStream& operator>>操作符，做数据反序列化操作
    friend QDataStream& operator>>(QDataStream& stream, FunctionParam& param) {
        stream >> param.sourceImagType;
        stream >> param.sourceImagePath;
        stream >> param.sourceImageDir;
        stream >> param.IP;
        stream >> param.cameraName;
        stream >> param.timeOut;
        return stream;
    }
};
/// <summary>
/// UI设计参数
/// </summary>
struct  UiParam {
    int color{ 0 };

    friend QDataStream& operator<<(QDataStream& stream, const UiParam& param) {
        stream << param.color;

        return stream;
    }
    friend QDataStream& operator>>(QDataStream& stream, UiParam& param) {
        stream >> param.color;

        return stream;
    }
};