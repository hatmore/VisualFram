#pragma once
#include <QObject>
#include <QDataStream>


 //<summary>
 //    函数运行参数
 //    </summary>
struct FunctionParam { 
    int screenNum{0};

    friend QDataStream& operator<<(QDataStream& stream, const FunctionParam& param) {
        stream << param.screenNum;
        return stream;
    }

    friend QDataStream& operator>>(QDataStream& stream, FunctionParam& param) {
        stream >> param.screenNum;
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