#pragma once
#include <QObject>
#include <QDataStream>


/// <summary>
/// 函数运行参数
/// </summary>
struct FunctionParam {
    //int RectNums;
    std::vector<cv::Rect>rects;
    std::vector<int>classIdsStream;
    std::vector<int> rectIndices; // 新增：保存矩形序号

    int x{ 0 };
    int y{ 0 };
    int width{ 1 };
    int height{ 1 };
    QStringList inputModelLabels;

    friend QDataStream& operator<<(QDataStream& stream, const FunctionParam& param) 
    {
        stream << static_cast<int>(param.rects.size());
        for (const auto& rect : param.rects) {
            stream << rect.x << rect.y << rect.width << rect.height;
        }

        stream << static_cast<int>(param.classIdsStream.size());  
        for (const auto& id : param.classIdsStream) {            
            stream << id;
        }

        // 保存矩形序号
        stream << static_cast<int>(param.rectIndices.size());
        for (const auto& index : param.rectIndices) {
            stream << index;
        }

        stream << param.x << param.y << param.width << param.height;
        stream << param.inputModelLabels;
        return stream;

    }

    friend QDataStream& operator>>(QDataStream& stream, FunctionParam& param) {
       
        int rectCount;
        stream >> rectCount;

        param.rects.resize(rectCount);
        for (int i = 0; i < rectCount; i++) {
            cv::Rect rect;
            stream >> rect.x >> rect.y >> rect.width >> rect.height;
            param.rects[i] = rect;
        }

        int classIdCount;
        stream >> classIdCount;                    
        param.classIdsStream.resize(classIdCount); 
        for (int i = 0; i < classIdCount; i++) {   
            stream >> param.classIdsStream[i];
        }

        // 读取矩形序号
        int indexCount;
        stream >> indexCount;
        param.rectIndices.resize(indexCount);
        for (int i = 0; i < indexCount; i++) {
            stream >> param.rectIndices[i];
        }

        stream >> param.x >> param.y >> param.width >> param.height;
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