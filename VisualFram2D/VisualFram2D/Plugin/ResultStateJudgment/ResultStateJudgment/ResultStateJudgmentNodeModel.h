#pragma once
#pragma execution_character_set("utf-8")
#include <QtCore/QObject>
#include "NodeEditor/BaseNodeModel.h"
#include "NodeEditor/BaseNodeData.h"
#include "NodeEditor/NodeEmbedWidget.h"
#include "FramResultStateJudgment.h"


class ResultStateJudgmentNodeModel :public BaseNodeModel
{
    Q_OBJECT
public:

    ResultStateJudgmentNodeModel();
    ~ResultStateJudgmentNodeModel();

};

