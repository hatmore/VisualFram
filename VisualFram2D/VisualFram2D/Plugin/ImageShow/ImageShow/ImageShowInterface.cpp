#include "ImageShowInterface.h"
#include "ImageShowNodeModel.h"
#include <QtDebug>

ImageShowInterface::ImageShowInterface(QObject* parent)
{
}

QString ImageShowInterface::ThisName()
{
	qDebug() << "ThisName: " << "ÕºœÒœ‘ æ";
	return "ÕºœÒœ‘ æ";
}

QString ImageShowInterface::ThisClass()
{
	qDebug() << "ThisClass: " << "ÕºœÒ¥¶¿Ì";
	return "ÕºœÒ¥¶¿Ì";
}

QString ImageShowInterface::ThisDescribe()
{
	qDebug() << "ThisDescribe" << "ÕºœÒœ‘ æ";
	return "ÕºœÒœ‘ æ";
}

QIcon ImageShowInterface::ThisIcon()
{
	QIcon icon = QIcon(":/FramImageShow/Image/ImageShow.png");
	return icon;
}

QIcon ImageShowInterface::ThisClassIcon()
{
	return QIcon();
}

QDialog* ImageShowInterface::BuildObject(const QString& meta_name)
{
	FramImageShow* ptr = new FramImageShow(nullptr);
	return ptr;
}

void ImageShowInterface::NodeModelRegistry(std::shared_ptr<NodeDelegateModelRegistry>& node_rgistry)
{
	node_rgistry->registerModel<ImageShowNodeModel>("ÕºœÒœ‘ æ");
}