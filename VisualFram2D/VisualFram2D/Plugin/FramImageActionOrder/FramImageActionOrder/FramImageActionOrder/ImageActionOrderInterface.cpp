#include"ImageActionOrderInterface.h"
#include"ImageActionOrderNodeModel.h"
#include <QtDebug>

ImageActionOrderInterface::ImageActionOrderInterface(QObject* parent)
{
}

QString ImageActionOrderInterface::ThisName()
{
    qDebug() << "ThisName: " << "动作顺序";
    return "动作顺序";
}

QString ImageActionOrderInterface::ThisClass()
{
    qDebug() << "ThisClass: " << "图像处理";
    return "图像处理";
}

QString ImageActionOrderInterface::ThisDescribe()
{
    qDebug() << "ThisDescribe" << "动作顺序";
    return "动作顺序";
}

QIcon ImageActionOrderInterface::ThisIcon()
{
    QIcon icon = QIcon(":/FramImageActionOrder/Image/actionOrder.png");
    return icon;
}

QIcon ImageActionOrderInterface::ThisClassIcon()
{
    return QIcon();
}

QDialog* ImageActionOrderInterface::BuildObject(const QString& meta_name)
{   
    FramImageActionOrder* ptr = new FramImageActionOrder(nullptr);
    return ptr;
}

void ImageActionOrderInterface::NodeModelRegistry(std::shared_ptr<NodeDelegateModelRegistry>& node_rgistry)
{
    node_rgistry->registerModel<ImageActionOrderNodeModel>("图像处理");
}