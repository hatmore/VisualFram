#pragma once
#pragma execution_character_set("utf-8")
#include <QtCore/QObject>
#include "NodeEditor/BaseNodeModel.h"
#include "NodeEditor/BaseNodeData.h"
#include "NodeEditor/NodeEmbedWidget.h"
#include"FramImageActionOrder.h"


class ImageActionOrderNodeModel :public BaseNodeModel
{
    Q_OBJECT
public:

    ImageActionOrderNodeModel();
    ~ImageActionOrderNodeModel();

};

