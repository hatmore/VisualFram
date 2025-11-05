#pragma once
#pragma execution_character_set("utf-8")
#include <QtCore/QObject>
#include "NodeEditor/BaseNodeModel.h"
#include "NodeEditor/BaseNodeData.h"
#include "NodeEditor/NodeEmbedWidget.h"
#include "FramStandardOperatingProcedureReport.h"


class StandardOperatingProcedureReportNodeModel :
    public BaseNodeModel
{
    Q_OBJECT
public:
    StandardOperatingProcedureReportNodeModel() {
        toolInterface = new FramStandardOperatingProcedureReport();
        nodeEmbedWidget = new NodeEmbedWidget();
        FramStandardOperatingProcedureReport* framStandardOperatingProcedureReport = dynamic_cast<FramStandardOperatingProcedureReport*>(toolInterface);
        mapCGObjectFlowInNodeAndName = framStandardOperatingProcedureReport->mapCGObjectFlowInNodeAndName;

        connect(framStandardOperatingProcedureReport, &FramStandardOperatingProcedureReport::SendNodeRuningStateAsychSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot, Qt::BlockingQueuedConnection);
        connect(framStandardOperatingProcedureReport, &FramStandardOperatingProcedureReport::SendNodeRuningStateSyncSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot);

        QIcon icon = QIcon(":/FramStandardOperatingProcedureReport/Image/SOP.png");
        nodeEmbedWidget->SetImageNode(icon);

        thisName = "SOP报表";
        thisCaption = "SOP报表";
        outPortNumber = 0;
        inPortNumber = mapCGObjectFlowInNodeAndName.size();
    }

    ~StandardOperatingProcedureReportNodeModel() { ; }

public:



private:
};

