#pragma once
#include <QObject>

/// <summary>
    /// 函数运行参数
    /// </summary>
struct FunctionParam {

    QString pythonFilePath{ "" };
    int screenNum{ 0 };

    friend QDataStream& operator<<(QDataStream& stream, const FunctionParam& param) {
        stream << param.screenNum;
        stream << param.pythonFilePath;
        return stream;
    }
    // 重写QDataStream& operator>>操作符，做数据反序列化操作
    friend QDataStream& operator>>(QDataStream& stream, FunctionParam& param) {
        stream >> param.screenNum;
        stream >> param.pythonFilePath;
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