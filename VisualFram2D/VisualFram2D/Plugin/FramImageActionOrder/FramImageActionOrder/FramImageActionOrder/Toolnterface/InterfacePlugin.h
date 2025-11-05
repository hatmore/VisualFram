#pragma once
#include <QString>
#include <QtPlugin>
#include <QPixmap>
#include <QIcon>
#include <QDialog>
#include <QtNodes/NodeDelegateModelRegistry.hpp>
using QtNodes::NodeDelegateModelRegistry;

class InterfacePlugin :public QObject
{
    Q_OBJECT
public:
    virtual ~InterfacePlugin() {}
    virtual QString ThisName() = 0;    
    virtual QString ThisClass() = 0;  
    virtual QString ThisDescribe() = 0; 
    virtual QIcon ThisIcon() = 0;  
    virtual QIcon ThisClassIcon() = 0;  
    virtual QDialog* BuildObject(const QString& meta_name) = 0; 
    virtual void NodeModelRegistry(std::shared_ptr<NodeDelegateModelRegistry> &node_rgistry) = 0; 
};

#define InterfacePlugin_iid "Plugin.InterfacePlugin"  

Q_DECLARE_INTERFACE(InterfacePlugin, InterfacePlugin_iid)