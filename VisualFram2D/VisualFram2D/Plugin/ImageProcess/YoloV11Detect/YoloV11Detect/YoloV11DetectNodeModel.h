#pragma once
#pragma execution_character_set("utf-8")
#include <QtCore/QObject>
#include "NodeEditor/BaseNodeModel.h"
#include "NodeEditor/BaseNodeData.h"
#include "NodeEditor/NodeEmbedWidget.h"
#include "FramYoloV11Detect.h"


class YoloV11DetectNodeModel :public BaseNodeModel
{
    Q_OBJECT
public:

    YoloV11DetectNodeModel();
    ~YoloV11DetectNodeModel();

};

