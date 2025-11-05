#pragma once
#pragma execution_character_set("utf-8")
#include"FramImageDuration.h"
#include "Toolnterface/InterfacePlugin.h"

class InterfacePlugin;
class ImageDurationInterface : public InterfacePlugin
{
    // programmer.json 插件的信息描述类
    Q_OBJECT
        Q_INTERFACES(InterfacePlugin)
        Q_PLUGIN_METADATA(IID InterfacePlugin_iid FILE "PlugInformation.json") // QT5.0 引入

public:
    explicit ImageDurationInterface(QObject* parent = 0);
    virtual  QString ThisName() override;
    virtual  QString ThisClass() override;
    virtual  QString ThisDescribe() override;
    virtual  QIcon ThisIcon() override;
    virtual  QIcon ThisClassIcon() override;
    virtual  QDialog* BuildObject(const QString& meta_name)  override;     
    virtual void NodeModelRegistry(std::shared_ptr<NodeDelegateModelRegistry>& node_rgistry) override;     
};