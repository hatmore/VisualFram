#pragma once
#include <QString>
#include <QtPlugin>
#include <QPixmap>
#include <QIcon>
#include <QDialog>
#include <QtNodes/NodeDelegateModelRegistry.hpp>
using QtNodes::NodeDelegateModelRegistry;

//定义接口
class InterfacePlugin :public QObject
{
    Q_OBJECT
public:
    virtual ~InterfacePlugin() {}
    virtual QString ThisName() = 0;      //控件名称
    virtual QString ThisClass() = 0;  //控件所属
    virtual QString ThisDescribe() = 0;    //控件描述
    virtual QIcon ThisIcon() = 0;     //控件图标
    virtual QIcon ThisClassIcon() = 0;     //空间所属类图标
    virtual QDialog* BuildObject(const QString& meta_name) = 0;     //创建目标
    virtual void NodeModelRegistry(std::shared_ptr<NodeDelegateModelRegistry> &node_rgistry) = 0;     //注册模块
};

#define InterfacePlugin_iid "Plugin.InterfacePlugin"   // 唯一标识符

Q_DECLARE_INTERFACE(InterfacePlugin, InterfacePlugin_iid)
