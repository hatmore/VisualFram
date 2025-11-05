#include "ExternalDeviceReadInterface.h"
#include "FramExternalDeviceRead.h"
#include "ExternalDeviceReadNodeModel.h"
#include <QtDebug>
#pragma execution_character_set("utf-8")

ExternalDeviceReadInterface::ExternalDeviceReadInterface(QObject* parent)
{
}

QString ExternalDeviceReadInterface::ThisName()
{
    qDebug() << "ThisName: " << "外部设备通讯";
    return "外部设备通讯";
}

QString ExternalDeviceReadInterface::ThisClass()
{
    qDebug() << "ThisClass: " << "外部设备通讯";
    return "外部设备通讯";
}

QString ExternalDeviceReadInterface::ThisDescribe()
{
    qDebug() << "ThisDescribe" << "外部设备通讯";
    return "外部设备通讯";
}

QIcon ExternalDeviceReadInterface::ThisIcon()
{
    QIcon icon = QIcon(":/FramExternalDeviceRead/Image/read.png");
    return icon;
}

QIcon ExternalDeviceReadInterface::ThisClassIcon()
{
    return QIcon();
}

QDialog* ExternalDeviceReadInterface::BuildObject(const QString& meta_name)
{
    FramExternalDeviceRead* ptr = new FramExternalDeviceRead(nullptr);
    return ptr;
}

void ExternalDeviceReadInterface::NodeModelRegistry(std::shared_ptr<NodeDelegateModelRegistry>& node_rgistry)
{
    node_rgistry->registerModel<ExternalDeviceReadNodeModel>("外部设备通讯");
}