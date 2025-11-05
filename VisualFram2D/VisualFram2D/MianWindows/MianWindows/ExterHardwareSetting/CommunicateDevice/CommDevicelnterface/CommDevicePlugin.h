#pragma once
#include "CommDeviceFactory.h"
#include <QString>
#include <QtPlugin>
#include <QPixmap>
#include <QIcon>
#include <QWidget>

//定义接口
class CommDevicePlugin :public QObject
{
    Q_OBJECT
public:
    virtual ~CommDevicePlugin() {}
    virtual QString ThisName() = 0;      //控件名称
    virtual QString ThisDescribe() = 0;    //控件描述
    virtual void  CommDeviceRegistry(CommDeviceFactory::Ptr & device_rgistry) = 0;     //注册模块
};

#define InterfacePlugin_iid "CommDevicePlugin"   // 唯一标识符

Q_DECLARE_INTERFACE(CommDevicePlugin, InterfacePlugin_iid)
