#include "ExternalDeviceWriteInterface.h"
#include "FramExternalDeviceWrite.h"
#include "ExternalDeviceWriteNodeModel.h"
#include <QtDebug>
#pragma execution_character_set("utf-8")

ExternalDeviceWriteInterface::ExternalDeviceWriteInterface(QObject * parent)
{
}

QString ExternalDeviceWriteInterface::ThisName()
{
    qDebug() << "ThisName: " << "写外部通讯";
    return "写外部通讯";
}

QString ExternalDeviceWriteInterface::ThisClass()
{
    qDebug() << "ThisClass: " << "写外部通讯";
    return "写外部通讯";
}

QString ExternalDeviceWriteInterface::ThisDescribe()
{
    qDebug() << "ThisDescribe" << "写外部通讯";
    return "写外部通讯";
}

QIcon ExternalDeviceWriteInterface::ThisIcon()
{
    QIcon icon = QIcon(":/FramExternalDeviceWrite/Image/write.png");
    return icon;
}

QIcon ExternalDeviceWriteInterface::ThisClassIcon()
{
    return QIcon();
}

QDialog* ExternalDeviceWriteInterface::BuildObject(const QString& meta_name)
{
    FramExternalDeviceWrite* ptr = new FramExternalDeviceWrite(nullptr);
    return ptr;
}

void ExternalDeviceWriteInterface::NodeModelRegistry(std::shared_ptr<NodeDelegateModelRegistry>& node_rgistry)
{
  node_rgistry->registerModel<ExternalDeviceWriteNodeModel>("写外部通讯");
}