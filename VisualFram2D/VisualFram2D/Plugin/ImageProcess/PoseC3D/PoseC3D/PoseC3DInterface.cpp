#include "PoseC3DInterface.h"
#include "PoseC3DNodeModel.h"
#include <QtDebug>

PoseC3DInterface::PoseC3DInterface(QObject* parent)
{
}

QString PoseC3DInterface::ThisName()
{
	qDebug() << "ThisName: " << "行为识别模型";
	return "行为识别模型";
}

QString PoseC3DInterface::ThisClass()
{
	qDebug() << "ThisClass: " << "图像处理";
	return "图像处理";
}

QString PoseC3DInterface::ThisDescribe()
{
	qDebug() << "ThisDescribe" << "行为识别模型";
	return "行为识别模型";
}

QIcon PoseC3DInterface::ThisIcon()
{
    QIcon icon = QIcon(":/FramPoseC3D/Image/iconRecognation.png");
	return icon;
}

QIcon PoseC3DInterface::ThisClassIcon()
{
	return QIcon();
}

QDialog* PoseC3DInterface::BuildObject(const QString& meta_name)
{
	FramPoseC3D* ptr = new FramPoseC3D(nullptr);
	return ptr;
}

void PoseC3DInterface::NodeModelRegistry(std::shared_ptr<NodeDelegateModelRegistry>& node_rgistry)
{
	node_rgistry->registerModel<PoseC3DNodeModel>("图像处理");
}