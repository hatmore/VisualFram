#include "YoloV11SegmentInterface.h"
#include "YoloV11SegmentNodeModel.h"
#include <QtDebug>

YoloV11SegmentInterface::YoloV11SegmentInterface(QObject* parent)
{
}

QString YoloV11SegmentInterface::ThisName()
{
	qDebug() << "ThisName: " << "分割检测模型";
	return "分割检测模型";
}

QString YoloV11SegmentInterface::ThisClass()
{
	qDebug() << "ThisClass: " << "图像处理";
	return "图像处理";
}

QString YoloV11SegmentInterface::ThisDescribe()
{
	qDebug() << "ThisDescribe" << "分割检测模型";
	return "分割检测模型";
}

QIcon YoloV11SegmentInterface::ThisIcon()
{
	QIcon icon = QIcon(":/FramYoloV11Segment/Image/YoloSegment.png");
	return icon;
}

QIcon YoloV11SegmentInterface::ThisClassIcon()
{
	return QIcon();
}

QDialog* YoloV11SegmentInterface::BuildObject(const QString& meta_name)
{
	FramYoloV11Segment* ptr = new FramYoloV11Segment(nullptr);
	return ptr;
}

void YoloV11SegmentInterface::NodeModelRegistry(std::shared_ptr<NodeDelegateModelRegistry>& node_rgistry)
{
	node_rgistry->registerModel<YoloV11SegmentNodeModel>("图像处理");
}