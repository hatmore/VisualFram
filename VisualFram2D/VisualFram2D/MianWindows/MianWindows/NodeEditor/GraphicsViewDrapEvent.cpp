#include "GraphicsViewDrapEvent.h"

#define DRAG_DATA QStringLiteral("Drag/DragTree")

GraphicsViewDrapEvent::GraphicsViewDrapEvent(QWidget* parent)
    : GraphicsView(parent)
{

}

GraphicsViewDrapEvent::GraphicsViewDrapEvent(QtNodes::BasicGraphicsScene* scene, QWidget* parent) :
    GraphicsView(scene, parent)
{
}

void GraphicsViewDrapEvent::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat(DRAG_DATA)) {
        QPoint scene_pos = QCursor::pos();
        event->acceptProposedAction();
    }
}

void GraphicsViewDrapEvent::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasFormat(DRAG_DATA)) {
        QByteArray pieceData = event->mimeData()->data(DRAG_DATA);
        QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
        QPixmap pixmap;
        QString str_text;
        dataStream >> pixmap >> str_text;

        QtNodes::BasicGraphicsScene* scene = this->nodeScene();
        QtNodes::NodeId  nodeId = scene->graphModel().addNode(str_text);
        if (nodeId != QtNodes::InvalidNodeId) {
            auto const scene_pos = mapToScene(event->pos());
            scene->graphModel().setNodeData(nodeId, QtNodes::NodeRole::Position, scene_pos);
        }
    }
}

void GraphicsViewDrapEvent::dragMoveEvent(QDragMoveEvent* event)
{
    QPoint scene_pos = QCursor::pos();
    event->accept();
}