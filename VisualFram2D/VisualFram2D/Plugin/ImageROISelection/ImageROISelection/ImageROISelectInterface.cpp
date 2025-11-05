#include "ImageROISelectInterface.h"
#include "ImageROISelectNodeModel.h"
#include <QtDebug>

ImageROISelectInterface::ImageROISelectInterface(QObject* parent)
{
}

QString ImageROISelectInterface::ThisName()
{
    qDebug() << "ThisName: " << "Í¼ÏñROI";
    return "Í¼ÏñROI";
}

QString ImageROISelectInterface::ThisClass()
{
    qDebug() << "ThisClass: " << "Í¼Ïñ´¦Àí";
    return "Í¼Ïñ´¦Àí";
}

QString ImageROISelectInterface::ThisDescribe()
{
    qDebug() << "ThisDescribe" << "Í¼ÏñROI";
    return "Í¼ÏñROI";
}

QIcon ImageROISelectInterface::ThisIcon()
{
    QIcon icon = QIcon(":/ImageROISelection/Image/region.png");
    return icon;
}

QIcon ImageROISelectInterface::ThisClassIcon()
{
    return QIcon();
}

QDialog* ImageROISelectInterface::BuildObject(const QString& meta_name)
{
    FramImageROISelection* ptr = new FramImageROISelection(nullptr);
    return ptr;
}

void ImageROISelectInterface::NodeModelRegistry(std::shared_ptr<NodeDelegateModelRegistry>& node_rgistry)
{
    node_rgistry->registerModel<ImageROISelectNodeModel>("Í¼Ïñ´¦Àí");
}