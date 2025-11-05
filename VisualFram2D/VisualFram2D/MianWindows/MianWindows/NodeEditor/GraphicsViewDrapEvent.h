#pragma once
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QGraphicsSceneDragDropEvent>
#include <QtWidgets/QGraphicsView>
#include "BaseNodeModel.h"
#include <QtNodes/BasicGraphicsScene.hpp>
#include <QtNodes/DataFlowGraphModel.hpp>
#include <QtNodes/GraphicsView.hpp>


class GraphicsViewDrapEvent: public QtNodes::GraphicsView
{
public:
	GraphicsViewDrapEvent(QWidget* parent = Q_NULLPTR);
	GraphicsViewDrapEvent(QtNodes::BasicGraphicsScene* scene, QWidget* parent = Q_NULLPTR);


protected:
    /// <summary>
    /// 拖拽接收事件
    /// </summary>
    /// <param name="event"></param>
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
};

