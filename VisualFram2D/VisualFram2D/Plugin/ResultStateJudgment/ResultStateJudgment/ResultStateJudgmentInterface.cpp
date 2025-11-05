#include "ResultStateJudgmentInterface.h"
#include "ResultStateJudgmentNodeModel.h"
#include <QtDebug>

ResultStateJudgmentInterface::ResultStateJudgmentInterface(QObject* parent)
{
}

QString ResultStateJudgmentInterface::ThisName()
{
	qDebug() << "ThisName: " << "Âß¼­ÅÐ¶Ï";
	return "Âß¼­ÅÐ¶Ï";
}

QString ResultStateJudgmentInterface::ThisClass()
{
	qDebug() << "ThisClass: " << "Âß¼­ÅÐ¶Ï";
	return "Âß¼­ÅÐ¶Ï";
}

QString ResultStateJudgmentInterface::ThisDescribe()
{
	qDebug() << "ThisDescribe" << "Âß¼­ÅÐ¶Ï";
	return "Âß¼­ÅÐ¶Ï";
}

QIcon ResultStateJudgmentInterface::ThisIcon()
{
	QIcon icon = QIcon(":/FramResultStateJudgment/Image/logicJudgement.png");
	return icon;
}

QIcon ResultStateJudgmentInterface::ThisClassIcon()
{
	return QIcon();
}

QDialog* ResultStateJudgmentInterface::BuildObject(const QString& meta_name)
{
	FramResultStateJudgment* ptr = new FramResultStateJudgment(nullptr);
	return ptr;
}

void ResultStateJudgmentInterface::NodeModelRegistry(std::shared_ptr<NodeDelegateModelRegistry>& node_rgistry)
{
	node_rgistry->registerModel<ResultStateJudgmentNodeModel>("Âß¼­ÅÐ¶Ï");
}