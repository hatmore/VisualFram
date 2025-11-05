#pragma once
#pragma execution_character_set("utf-8")
#include <QtCore/QObject>
#include "NodeEditor/BaseNodeModel.h"
#include "NodeEditor/BaseNodeData.h"
#include "NodeEditor/NodeEmbedWidget.h"
#include "FramExternalDeviceWrite.h"


class ExternalDeviceWriteNodeModel :
    public BaseNodeModel
{
    Q_OBJECT
public:
    ExternalDeviceWriteNodeModel() {
        toolInterface = new FramExternalDeviceWrite();
        nodeEmbedWidget = new NodeEmbedWidget();
        FramExternalDeviceWrite* framExternalDeviceWrite = dynamic_cast<FramExternalDeviceWrite*>(toolInterface);
        mapCGObjectFlowInNodeAndName = framExternalDeviceWrite->mapCGObjectFlowInNodeAndName;

        connect(framExternalDeviceWrite, &FramExternalDeviceWrite::SendNodeRuningStateAsychSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot, Qt::BlockingQueuedConnection);
        connect(framExternalDeviceWrite, &FramExternalDeviceWrite::SendNodeRuningStateSyncSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot);

        QIcon icon = QIcon(":/FramExternalDeviceWrite/Image/write.png");
        nodeEmbedWidget->SetImageNode(icon);

        thisName = "写外部通讯";
        thisCaption = "写外部通讯";
        outPortNumber = 0;
        inPortNumber = mapCGObjectFlowInNodeAndName.size();
    }

    ~ExternalDeviceWriteNodeModel() { ; }

public:



private:
};

