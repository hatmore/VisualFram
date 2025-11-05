#pragma once
#include <QObject>
#include <QDataStream>

//    函数运行参数
//    </summary>
struct FunctionParam {
    float IouParam = 3.0;

    friend QDataStream& operator<<(QDataStream& stream, const FunctionParam& param) {
        stream << param.IouParam;
        return stream;
    }

    friend QDataStream& operator>>(QDataStream& stream, FunctionParam& param) {
        stream >> param.IouParam;
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