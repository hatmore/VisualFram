#include "ImageSourceInterface.h"
#include "ImageSourceNodeModel.h"
#include <QtDebug>
#pragma execution_character_set("utf-8")

ImageSourceInterface::ImageSourceInterface(QObject* parent)
{
}

QString ImageSourceInterface::ThisName()
{
	qDebug() << "ThisName: " << "图像源";
	return "图像源";
}

QString ImageSourceInterface::ThisClass()
{
	qDebug() << "ThisClass: " << "图像处理";
	return "图像处理";
}

QString ImageSourceInterface::ThisDescribe()
{
	qDebug() << "ThisDescribe" << "图像源";
	return "ThisDescribe";
}

QIcon ImageSourceInterface::ThisIcon()
{
	QIcon icon = QIcon(":/FramImageSource/Image/Camera.png");
	return icon;
}

QIcon ImageSourceInterface::ThisClassIcon()
{
	return QIcon();
}

QDialog* ImageSourceInterface::BuildObject(const QString& meta_name)
{
	FramImageSource* ptr = new FramImageSource(nullptr);
	return ptr;
}

void ImageSourceInterface::NodeModelRegistry(std::shared_ptr<NodeDelegateModelRegistry>& node_rgistry)
{
	node_rgistry->registerModel<ImageSourceNodeModel>("图像处理");
}