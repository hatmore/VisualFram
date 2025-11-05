#include "StandardOperatingProcedureReportInterface.h"
#include "FramStandardOperatingProcedureReport.h"
#include "StandardOperatingProcedureReportNodeModel.h"
#include <QtDebug>
#pragma execution_character_set("utf-8")

StandardOperatingProcedureReportInterface::StandardOperatingProcedureReportInterface(QObject* parent)
{
}

QString StandardOperatingProcedureReportInterface::ThisName()
{
    qDebug() << "ThisName: " << "SOP报表";
    return "SOP报表";
}

QString StandardOperatingProcedureReportInterface::ThisClass()
{
    qDebug() << "ThisClass: " << "SOP报表";
    return "SOP报表";
}

QString StandardOperatingProcedureReportInterface::ThisDescribe()
{
    qDebug() << "ThisDescribe" << "SOP报表";
    return "SOP报表";
}

QIcon StandardOperatingProcedureReportInterface::ThisIcon()
{
    QIcon icon = QIcon(":/FramStandardOperatingProcedureReport/Image/SOP.png");
    return icon;
}

QIcon StandardOperatingProcedureReportInterface::ThisClassIcon()
{
    return QIcon();
}

QDialog* StandardOperatingProcedureReportInterface::BuildObject(const QString& meta_name)
{
    FramStandardOperatingProcedureReport* ptr = new FramStandardOperatingProcedureReport(nullptr);
    return ptr;
}

void StandardOperatingProcedureReportInterface::NodeModelRegistry(std::shared_ptr<NodeDelegateModelRegistry>& node_rgistry)
{
    node_rgistry->registerModel<StandardOperatingProcedureReportNodeModel>("SOP报表");
}