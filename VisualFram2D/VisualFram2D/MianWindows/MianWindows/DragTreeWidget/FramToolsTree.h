#pragma once

#include <QWidget>
#include "DragTreeWidget.h"
#include "../PlugiManager/PluginsManager.h"

class FramToolsTree : public QWidget
{
    Q_OBJECT

public:
    FramToolsTree(QWidget *parent = nullptr);
    ~FramToolsTree();

private:
    void InitialFramToolsTree();

private:

    DragTreeWidget* toolTreeWidget{nullptr};
};
