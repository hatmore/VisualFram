#pragma once
#include <QObject>
#include <QDataStream>

//<summary>
//    函数运行参数
//    </summary>
struct FunctionParam {
    //动作总数
    int actionNum = 8;
    //类别总数
    int classNum = 5;
    //类别ID
    QList<int> modelClassId = {0, 1, 2, 3, 4};
    //类别名称
    QStringList modelClassName;
    //模型存储路径
    QString modelFilePath = "";
    //模型推理参数
    int kInputH = 640;
    int kInputW = 640;
    float kConfThresh = 0.45f;
    QStringList inputModelLabels;

    // 重写QDataStream& operator<<操作符，做数据序列化操作
    friend QDataStream& operator<<(QDataStream& stream, const FunctionParam& param) {
        stream << param.actionNum;
        stream << param.classNum;
        stream << param.modelClassId;
        stream << param.modelClassName;
        stream << param.modelFilePath;
        stream << param.kInputH;
        stream << param.kInputW;
        stream << param.kConfThresh;
        stream << param.inputModelLabels;
        return stream;
    }
    // 重写QDataStream& operator>>操作符，做数据反序列化操作
    friend QDataStream& operator>>(QDataStream& stream, FunctionParam& param) {
        stream >> param.actionNum;
        stream >> param.classNum;
        stream >> param.modelClassId;
        stream >> param.modelClassName;
        stream >> param.modelFilePath;
        stream >> param.kInputH;
        stream >> param.kInputW;
        stream >> param.kConfThresh;
        stream >> param.inputModelLabels;
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