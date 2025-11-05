#include "ImageMaskFilterInterface.h"
#include "ImageMaskFilterNodeModel.h"
#include <QtDebug>

ImageMaskFilterInterface::ImageMaskFilterInterface(QObject* parent)
{
}

QString ImageMaskFilterInterface::ThisName()
{
	qDebug() << "ThisName: " << "Í¼ÏñÑÚÄ¤";
	return "Í¼ÏñÑÚÄ¤";
}

QString ImageMaskFilterInterface::ThisClass()
{
	qDebug() << "ThisClass: " << "Í¼Ïñ´¦Àí";
	return "Í¼Ïñ´¦Àí";
}

QString ImageMaskFilterInterface::ThisDescribe()
{
	qDebug() << "ThisDescribe" << "Í¼ÏñÑÚÄ¤";
	return "Í¼ÏñÑÚÄ¤";
}

QIcon ImageMaskFilterInterface::ThisIcon()
{
	QIcon icon = QIcon(":/FramImageMaskFilter/Image/ImageMaskFilter.png");
	return icon;
}

QIcon ImageMaskFilterInterface::ThisClassIcon()
{
	return QIcon();
}

QDialog* ImageMaskFilterInterface::BuildObject(const QString& meta_name)
{
	FramImageMaskFilter* ptr = new FramImageMaskFilter(nullptr);
	return ptr;
}

void ImageMaskFilterInterface::NodeModelRegistry(std::shared_ptr<NodeDelegateModelRegistry>& node_rgistry)
{
	node_rgistry->registerModel<ImageMaskFilterNodeModel>("Í¼ÏñÑÚÄ¤");
}