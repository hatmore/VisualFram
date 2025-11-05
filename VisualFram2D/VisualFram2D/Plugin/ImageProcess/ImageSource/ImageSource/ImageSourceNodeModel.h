#pragma once
#pragma execution_character_set("utf-8")
#include <QtCore/QObject>
#include "NodeEditor/BaseNodeModel.h"
#include "NodeEditor/BaseNodeData.h"
#include "NodeEditor/NodeEmbedWidget.h"
#include "FramImageSource.h"

class ImageSourceNodeModel :public BaseNodeModel
{
	Q_OBJECT
public:
	ImageSourceNodeModel() {
        toolInterface = new FramImageSource();
        nodeEmbedWidget = new NodeEmbedWidget();
        FramImageSource* framImageSource = dynamic_cast<FramImageSource*>(toolInterface);
        mapCGObjectFlowOutNodeAndName =  framImageSource->mapCGObjectFlowOutNodeAndName;

        connect(framImageSource, &FramImageSource::SendNodeRuningStateAsychSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot, Qt::BlockingQueuedConnection);
        connect(framImageSource, &FramImageSource::SendNodeRuningStateSyncSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot);

        QIcon icon = QIcon(":/FramImageSource/Image/DLCW2.png");
        nodeEmbedWidget->SetImageNode(icon);

        thisName = "Í¼ÏñÔ´";
        thisCaption = "Í¼ÏñÔ´";
        inPortNumber = 0;
        outPortNumber = mapCGObjectFlowOutNodeAndName.size();
	}

	~ImageSourceNodeModel() { ; }

public:



private:



};

