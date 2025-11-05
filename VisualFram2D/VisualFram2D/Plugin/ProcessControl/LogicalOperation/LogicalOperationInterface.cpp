#include "LogicalOperationInterface.h"
#include "LogicalOperationNodeModel.h"
#include <QtDebug>

LogicalOperationInterface::LogicalOperationInterface(QObject* parent)
{
}

QString LogicalOperationInterface::ThisName()
{
	qDebug() << "ThisName: " << "逻辑运算";
	return "逻辑运算";
}

QString LogicalOperationInterface::ThisClass()
{
	qDebug() << "ThisClass: " << "控制处理";
	return "控制处理";
}

QString LogicalOperationInterface::ThisDescribe()
{
	qDebug() << "ThisDescribe" << "逻辑运算";
	return "逻辑运算";
}

QIcon LogicalOperationInterface::ThisIcon()
{
	QIcon icon = QIcon(":/FramLogicalOperation/Image/LogicalOperation.png");
	return icon;
}

QIcon LogicalOperationInterface::ThisClassIcon()
{
	return QIcon();
}

QDialog* LogicalOperationInterface::BuildObject(const QString& meta_name)
{
	FramLogicalOperation* ptr = new FramLogicalOperation(nullptr);
	return ptr;
}

void LogicalOperationInterface::NodeModelRegistry(std::shared_ptr<NodeDelegateModelRegistry>& node_rgistry)
{
	node_rgistry->registerModel<LogicalOperationNodeModel>("逻辑运算");
}