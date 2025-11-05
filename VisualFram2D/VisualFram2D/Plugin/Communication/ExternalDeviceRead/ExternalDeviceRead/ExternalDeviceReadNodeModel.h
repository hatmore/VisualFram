#pragma once
#pragma execution_character_set("utf-8")
#include <QtCore/QObject>
#include "NodeEditor/BaseNodeModel.h"
#include "NodeEditor/BaseNodeData.h"
#include "NodeEditor/NodeEmbedWidget.h"
#include "FramExternalDeviceRead.h"


class ExternalDeviceReadNodeModel :public BaseNodeModel
{
    Q_OBJECT
public:
    ExternalDeviceReadNodeModel() {
        toolInterface = new FramExternalDeviceRead();
        nodeEmbedWidget = new NodeEmbedWidget();
        FramExternalDeviceRead* framImageSource = dynamic_cast<FramExternalDeviceRead*>(toolInterface);
        mapCGObjectFlowOutNodeAndName = framImageSource->mapCGObjectFlowOutNodeAndName;

        connect(framImageSource, &FramExternalDeviceRead::SendNodeRuningStateAsychSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot, Qt::BlockingQueuedConnection);
        connect(framImageSource, &FramExternalDeviceRead::SendNodeRuningStateSyncSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot);

        QIcon icon = QIcon(":/FramExternalDeviceRead/Image/read.png");
        nodeEmbedWidget->SetImageNode(icon);

        thisName = "外部设备通讯";
        thisCaption = "外部设备通讯";
        inPortNumber = 0;
        outPortNumber = mapCGObjectFlowOutNodeAndName.size();
    }

    ~ExternalDeviceReadNodeModel() { ; }

};