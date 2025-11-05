#include "YoloV11PoseInterface.h"
#include "YoloV11PoseNodeModel.h"
#include <QtDebug>

YoloV11PoseInterface::YoloV11PoseInterface(QObject* parent)
{
}

QString YoloV11PoseInterface::ThisName()
{
	qDebug() << "ThisName: " << "关键点检测模型";
	return "关键点检测模型";
}

QString YoloV11PoseInterface::ThisClass()
{
	qDebug() << "ThisClass: " << "图像处理";
	return "图像处理";
}

QString YoloV11PoseInterface::ThisDescribe()
{
	qDebug() << "ThisDescribe" << "关键点检测模型";
	return "关键点检测模型";
}

QIcon YoloV11PoseInterface::ThisIcon()
{
	QIcon icon = QIcon(":/FramYoloV11Pose/Image/YoloPose.png");
	return icon;
}

QIcon YoloV11PoseInterface::ThisClassIcon()
{
	return QIcon();
}

QDialog* YoloV11PoseInterface::BuildObject(const QString& meta_name)
{
	FramYoloV11Pose* ptr = new FramYoloV11Pose(nullptr);
	return ptr;
}

void YoloV11PoseInterface::NodeModelRegistry(std::shared_ptr<NodeDelegateModelRegistry>& node_rgistry)
{
	node_rgistry->registerModel<YoloV11PoseNodeModel>("图像处理");
}