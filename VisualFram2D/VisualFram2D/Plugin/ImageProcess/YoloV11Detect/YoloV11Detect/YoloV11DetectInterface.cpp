#include "YoloV11DetectInterface.h"
#include "YoloV11DetectNodeModel.h"
#include <QtDebug>

YoloV11DetectInterface::YoloV11DetectInterface(QObject* parent)
{
}

QString YoloV11DetectInterface::ThisName()
{
	qDebug() << "ThisName: " << "目标检测模型";
	return "目标检测模型";
}

QString YoloV11DetectInterface::ThisClass()
{
	qDebug() << "ThisClass: " << "图像处理";
	return "图像处理";
}

QString YoloV11DetectInterface::ThisDescribe()
{
	qDebug() << "ThisDescribe" << "目标检测模型";
	return "目标检测模型";
}

QIcon YoloV11DetectInterface::ThisIcon()
{
	QIcon icon = QIcon(":/FramYoloV11Detect/Image/YoloDetect.png");
	return icon;
}

QIcon YoloV11DetectInterface::ThisClassIcon()
{
	return QIcon();
}

QDialog* YoloV11DetectInterface::BuildObject(const QString& meta_name)
{
	FramYoloV11Detect* ptr = new FramYoloV11Detect(nullptr);
	return ptr;
}

void YoloV11DetectInterface::NodeModelRegistry(std::shared_ptr<NodeDelegateModelRegistry>& node_rgistry)
{
	node_rgistry->registerModel<YoloV11DetectNodeModel>("图像处理");
}