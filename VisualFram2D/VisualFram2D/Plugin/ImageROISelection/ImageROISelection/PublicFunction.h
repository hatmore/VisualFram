#pragma once
#include <QJsonObject>
#include <QImage>
#include "NodeEditor/BaseNodeModel.h"
#include "NodeEditor/BaseNodeData.h"
#include "GeneralStruct.h"


class PublicFunction
{
public:
    PublicFunction() {}
    ~PublicFunction() {}

    template<class T>
    inline void SerializeTParament(QJsonObject& json_object, const QString& name, const T& param)
    {
        QByteArray data_array;
        QDataStream stream(&data_array, QIODevice::WriteOnly);
        stream << param;   
        QString str = QString(data_array.toHex());
        json_object[name] = str; 
    }

    template<class T>
    inline void ExserializeTFunciton(const QString& json, T& param)
    {
        QByteArray by = QByteArray::fromHex(json.toLatin1());       
        QDataStream stream(&by, QIODevice::ReadOnly);  
        stream >> param;    
    }

 
    QImage CvMat2QImage(const cv::Mat& mat)
    {
        QImage image;
        switch (mat.type())
        {
        case CV_8UC1:
            // QImage构造：数据，宽度，高度，每行多少字节，存储结构
            image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
            break;
        case CV_8UC3:
            image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
            image = image.rgbSwapped(); // BRG转为RGB
            // Qt5.14增加了Format_BGR888
            // image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.cols * 3, QImage::Format_BGR888);
            break;
        case CV_8UC4:
            image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
            break;
        case CV_16UC4:
            image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGBA64);
            image = image.rgbSwapped(); // BRG转为RGB
            break;
        }
        return image;
    }

};



