#pragma once
#pragma execution_character_set("utf-8")
#include <QtCore/QObject>
#include "NodeEditor/BaseNodeModel.h"
#include "NodeEditor/BaseNodeData.h"
#include "NodeEditor/NodeEmbedWidget.h"
#include "FramPoseC3D.h"


class PoseC3DNodeModel :public BaseNodeModel
{
    Q_OBJECT
public:

    PoseC3DNodeModel();
    ~PoseC3DNodeModel();

};

