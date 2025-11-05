#include"ImageDurationInterface.h"
#include"ImageDurationNodeModel.h"
#include <QtDebug>


ImageDurationInterface::ImageDurationInterface(QObject* parent)
{
}

QString ImageDurationInterface::ThisName()
{
    qDebug() << "ThisName: " << "持续时间";
    return "持续时间";
}

QString ImageDurationInterface::ThisClass()
{
    qDebug() << "ThisClass: " << "图像处理";
    return "图像处理";
}

QString ImageDurationInterface::ThisDescribe()
{
    qDebug() << "ThisDescribe" << "持续时间";
    return "持续时间";
}

QIcon ImageDurationInterface::ThisIcon()
{
    QIcon icon = QIcon(":/FramImageDuration/Image/filter.png");
    return icon;
}

QIcon ImageDurationInterface::ThisClassIcon()
{
    return QIcon();
}

QDialog* ImageDurationInterface::BuildObject(const QString& meta_name)
{
    FramImageDuration* ptr = new FramImageDuration(nullptr);
    return ptr;
}

void ImageDurationInterface::NodeModelRegistry(std::shared_ptr<NodeDelegateModelRegistry>& node_rgistry)
{   
    node_rgistry->registerModel<ImageDurationNodeModel>("图像处理");
}