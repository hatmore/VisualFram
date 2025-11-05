#pragma once
#include <QObject>
#include <QDataStream>

struct VideoInfo
{
    float IouValue=3;      //交并比
    int MaxMissFrames=5;   //允许连续丢失检测的最大帧数
    float VideoFps=30;     //视频帧率

    // 序列化（写入到 QDataStream）
    friend QDataStream& operator<<(QDataStream& stream, const VideoInfo& param) {
        stream << param.IouValue;
        stream << param.MaxMissFrames;
        stream << param.VideoFps;
        return stream;
    }
    // 反序列化（从 QDataStream 读取）
    friend QDataStream& operator>>(QDataStream& stream, VideoInfo& param) {
        stream >> param.IouValue;
        stream >> param.MaxMissFrames;
        stream >> param.VideoFps;
        return stream;
    }
};


// 函数运行参数
struct FunctionParam 
{
    VideoInfo VideoParam;
    // 序列化操作符
    friend QDataStream& operator<<(QDataStream& stream, const FunctionParam& param) {
        stream << param.VideoParam;
        return stream;
    }

    // 反序列化操作符
    friend QDataStream& operator>>(QDataStream& stream, FunctionParam& param) {
        stream >> param.VideoParam;
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