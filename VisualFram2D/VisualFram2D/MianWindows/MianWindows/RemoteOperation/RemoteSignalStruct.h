#pragma once

#include "../GeneralStruct.h"
/// <summary>
/// 信号数据类型
/// </summary>
enum SignalType {
    SIGLEINT = 0,
    SIGFLOT = 1,
    SIGSTRING = 2,
};

/// <summary>
/// 信号属性
/// </summary>
enum SignalProperty {
    SIGDATA = 0,                    //信号为数据
    SIGCAMERA = 1,         //信号为相机软触发
    SIGFLOW = 2,             //信号触发流程
};

/// <summary>
/// 发来的信号相关参数
/// </summary>
struct SignalParseParam
{
    QString signalName{};   //信号名称
    SignalType signalType{ SignalType::SIGLEINT };  //信号类型
    SignalProperty signalProperty{ SignalProperty::SIGDATA };  //信号作用
    QString triggerObject{ "" };   //触发目标
    QString triggerChar{" "};  //触发字符

    friend QDataStream& operator<<(QDataStream& stream, const SignalParseParam& param) {
        stream << param.signalName;
        stream << param.signalType;
        stream << param.signalProperty;
        stream << param.triggerObject;
        stream << param.triggerChar;
        return stream;
    }

    friend QDataStream& operator>>(QDataStream& stream, SignalParseParam& param) {
        stream >> param.signalName;
        stream >> param.signalType;
        stream >> param.signalProperty;
        stream >> param.triggerObject;
        stream >> param.triggerChar;
        return stream;
    }
};

/// <summary>
/// 远程事件，相关参数
/// </summary>
struct  RemoteOpterateParam
{
    QString remoteOpeterName;     //远程操作名称   
    QString commDeviceName;       //通信设备名称
    QString  remoteCodeAddress{ "" };   //指令码地址
    SignalType remoteType{ SignalType::SIGLEINT };  //指令码类型
    QString separatorType{ ";" };   //分隔符
    int textLength{ 1024 };     //长度
    bool isDetectLength{ false };    //是否进行长度检查
    QVector<SignalParseParam> qvSignalParseParam;    //信号类型

    friend QDataStream& operator<<(QDataStream& stream, const RemoteOpterateParam& param) {
        stream << param.remoteOpeterName;
        stream << param.commDeviceName;
        stream << param.remoteCodeAddress;
        stream << param.remoteType;
        stream << param.separatorType;
        stream << param.textLength;
        stream << param.isDetectLength;

        return stream;
    }

    friend QDataStream& operator>>(QDataStream& stream, RemoteOpterateParam& param) {
        stream >> param.remoteOpeterName;
        stream >> param.commDeviceName;
        stream >> param.remoteCodeAddress;
        stream >> param.remoteType;
        stream >> param.separatorType;
        stream >> param.textLength;
        stream >> param.isDetectLength;
        return stream;
    }

};
