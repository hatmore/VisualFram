#include "GraphicsViewOverride.h"

#include "QtNodes/BasicGraphicsScene.hpp"
#include "QtNodes/ConnectionGraphicsObject.hpp"
#include "QtNodes/NodeGraphicsObject.hpp"
#include "QtNodes/StyleCollection.hpp"
#include "OverrideDataFlowGraphicsScene.h"

#include <QtWidgets/QGraphicsScene>

#include <QtGui/QBrush>
#include <QtGui/QPen>

#include <QtWidgets/QMenu>

#include <QtCore/QDebug>
#include <QtCore/QPointF>
#include <QtCore/QRectF>
#include <QtWidgets>

#include <cmath>
#include <iostream>


#define DRAG_DATA QStringLiteral("Drag/DragTree")

using QtNodes::BasicGraphicsScene;
using QtNodes::GraphicsViewOverride;

GraphicsViewOverride::GraphicsViewOverride(QWidget* parent)
    : QGraphicsView(parent)
    , _clearSelectionAction(Q_NULLPTR)
    , _deleteSelectionAction(Q_NULLPTR)
    , _duplicateSelectionAction(Q_NULLPTR)
    , _copySelectionAction(Q_NULLPTR)
    , _pasteAction(Q_NULLPTR)
{
    setDragMode(QGraphicsView::ScrollHandDrag);
    setRenderHint(QPainter::Antialiasing);

    auto const& flowViewStyle = StyleCollection::flowViewStyle();

    setBackgroundBrush(flowViewStyle.BackgroundColor);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    setScaleRange(0.3, 2);

    // Sets the scene rect to its maximum possible ranges to avoid autu scene range
    // re-calculation when expanding the all QGraphicsItems common rect.
    int maxSize = 32767;
    setSceneRect(-maxSize, -maxSize, (maxSize * 2), (maxSize * 2));
}

GraphicsViewOverride::GraphicsViewOverride(BasicGraphicsScene* scene, QWidget* parent)
    : QGraphicsView(parent)
{
    setScene(scene);
}

QAction* GraphicsViewOverride::clearSelectionAction() const
{
    return _clearSelectionAction;
}

QAction* GraphicsViewOverride::deleteSelectionAction() const
{
    return _deleteSelectionAction;
}

void GraphicsViewOverride::setScene(BasicGraphicsScene* scene)
{
    QGraphicsView::setScene(scene);

    {
        // setup actions
        delete _clearSelectionAction;
        _clearSelectionAction = new QAction(QStringLiteral("Clear Selection"), this);
        _clearSelectionAction->setShortcut(Qt::Key_Escape);

        connect(_clearSelectionAction, &QAction::triggered, scene, &QGraphicsScene::clearSelection);

        addAction(_clearSelectionAction);
    }

    {
        delete _deleteSelectionAction;
        _deleteSelectionAction = new QAction(QStringLiteral("Delete Selection"), this);
        _deleteSelectionAction->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);
        _deleteSelectionAction->setShortcut(QKeySequence(QKeySequence::Delete));
        connect(_deleteSelectionAction,
            &QAction::triggered,
            this,
            &GraphicsViewOverride::onDeleteSelectedObjects);

        addAction(_deleteSelectionAction);
    }

    {
        delete _duplicateSelectionAction;
        _duplicateSelectionAction = new QAction(QStringLiteral("Duplicate Selection"), this);
        _duplicateSelectionAction->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);
        _duplicateSelectionAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));
        connect(_duplicateSelectionAction,
            &QAction::triggered,
            this,
            &GraphicsViewOverride::onDuplicateSelectedObjects);

        addAction(_duplicateSelectionAction);
    }

    {
        delete _copySelectionAction;
        _copySelectionAction = new QAction(QStringLiteral("Copy Selection"), this);
        _copySelectionAction->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);
        _copySelectionAction->setShortcut(QKeySequence(QKeySequence::Copy));
        connect(_copySelectionAction,
            &QAction::triggered,
            this,
            &GraphicsViewOverride::onCopySelectedObjects);

        addAction(_copySelectionAction);
    }

    {
        delete _pasteAction;
        _pasteAction = new QAction(QStringLiteral("Copy Selection"), this);
        _pasteAction->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);
        _pasteAction->setShortcut(QKeySequence(QKeySequence::Paste));
        connect(_pasteAction, &QAction::triggered, this, &GraphicsViewOverride::onPasteObjects);

        addAction(_pasteAction);
    }

    auto undoAction = scene->undoStack().createUndoAction(this, tr("&Undo"));
    undoAction->setShortcuts(QKeySequence::Undo);
    addAction(undoAction);

    auto redoAction = scene->undoStack().createRedoAction(this, tr("&Redo"));
    redoAction->setShortcuts(QKeySequence::Redo);
    addAction(redoAction);
}

void GraphicsViewOverride::centerScene()
{
    if (scene()) {
        scene()->setSceneRect(QRectF());

        QRectF sceneRect = scene()->sceneRect();

        if (sceneRect.width() > this->rect().width() || sceneRect.height() > this->rect().height()) {
            fitInView(sceneRect, Qt::KeepAspectRatio);
        }

        centerOn(sceneRect.center());
    }
}

void QtNodes::GraphicsViewOverride::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat(DRAG_DATA)) {
        QPoint scene_pos = QCursor::pos();
        event->acceptProposedAction();
    }
}

void QtNodes::GraphicsViewOverride::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasFormat(DRAG_DATA)) {
        QByteArray pieceData = event->mimeData()->data(DRAG_DATA);
        QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
        QPixmap pixmap;
        QString str_text;
        dataStream >> pixmap >> str_text;


        BasicGraphicsScene* scene = this->nodeScene();
        NodeId  nodeId = scene->graphModel().addNode(str_text);
        if (nodeId != InvalidNodeId) {
            auto const scene_pos = mapToScene(event->pos());
            scene->graphModel().setNodeData(nodeId, NodeRole::Position, scene_pos);
        }
    }
}

void QtNodes::GraphicsViewOverride::dragMoveEvent(QDragMoveEvent* event)
{
    QPoint scene_pos = QCursor::pos();
    event->accept();
}

void GraphicsViewOverride::contextMenuEvent(QContextMenuEvent* event)
{
    if (itemAt(event->pos())) {
        QGraphicsView::contextMenuEvent(event);
        return;
    }

    auto const scenePos = mapToScene(event->pos());

    QMenu* menu = nodeScene()->createSceneMenu(scenePos);

    if (menu) {
        menu->exec(event->globalPos());
    }
}

void GraphicsViewOverride::wheelEvent(QWheelEvent* event)
{
    QPoint delta = event->angleDelta();

    if (delta.y() == 0) {
        event->ignore();
        return;
    }

    double const d = delta.y() / std::abs(delta.y());

    if (d > 0.0)
        scaleUp();
    else
        scaleDown();
}

double GraphicsViewOverride::getScale() const
{
    return transform().m11();
}

void GraphicsViewOverride::setScaleRange(double minimum, double maximum)
{
    if (maximum < minimum)
        std::swap(minimum, maximum);
    minimum = std::max(0.0, minimum);
    maximum = std::max(0.0, maximum);

    _scaleRange = { minimum, maximum };

    setupScale(transform().m11());
}

void GraphicsViewOverride::setScaleRange(ScaleRange range)
{
    setScaleRange(range.minimum, range.maximum);
}

void GraphicsViewOverride::scaleUp()
{
    double const step = 1.2;
    double const factor = std::pow(step, 1.0);

    if (_scaleRange.maximum > 0) {
        QTransform t = transform();
        t.scale(factor, factor);
        if (t.m11() >= _scaleRange.maximum) {
            setupScale(t.m11());
            return;
        }
    }

    scale(factor, factor);
    Q_EMIT scaleChanged(transform().m11());
}

void GraphicsViewOverride::scaleDown()
{
    double const step = 1.2;
    double const factor = std::pow(step, -1.0);

    if (_scaleRange.minimum > 0) {
        QTransform t = transform();
        t.scale(factor, factor);
        if (t.m11() <= _scaleRange.minimum) {
            setupScale(t.m11());
            return;
        }
    }

    scale(factor, factor);
    Q_EMIT scaleChanged(transform().m11());
}

void GraphicsViewOverride::setupScale(double scale)
{
    scale = std::max(_scaleRange.minimum, std::min(_scaleRange.maximum, scale));

    if (scale <= 0)
        return;

    if (scale == transform().m11())
        return;

    QTransform matrix;
    matrix.scale(scale, scale);
    setTransform(matrix, false);

    Q_EMIT scaleChanged(scale);
}

void GraphicsViewOverride::onDeleteSelectedObjects()
{
    //nodeScene()->undoStack().push(new DeleteCommand(nodeScene()));
}

void GraphicsViewOverride::onDuplicateSelectedObjects()
{
    QPointF const pastePosition = scenePastePosition();

    //nodeScene()->undoStack().push(new CopyCommand(nodeScene()));
    //nodeScene()->undoStack().push(new PasteCommand(nodeScene(), pastePosition));
}

void GraphicsViewOverride::onCopySelectedObjects()
{
    //nodeScene()->undoStack().push(new CopyCommand(nodeScene()));
}

void GraphicsViewOverride::onPasteObjects()
{
    QPointF const pastePosition = scenePastePosition();
    //nodeScene()->undoStack().push(new PasteCommand(nodeScene(), pastePosition));
}

void GraphicsViewOverride::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Shift:
        setDragMode(QGraphicsView::RubberBandDrag);
        break;

    default:
        break;
    }

    QGraphicsView::keyPressEvent(event);
}

void GraphicsViewOverride::keyReleaseEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Shift:
        setDragMode(QGraphicsView::ScrollHandDrag);
        break;

    default:
        break;
    }
    QGraphicsView::keyReleaseEvent(event);
}

void GraphicsViewOverride::mousePressEvent(QMouseEvent* event)
{
    QGraphicsView::mousePressEvent(event);
    if (event->button() == Qt::LeftButton) {
        _clickPos = mapToScene(event->pos());
    }
}

void GraphicsViewOverride::mouseMoveEvent(QMouseEvent* event)
{
    QGraphicsView::mouseMoveEvent(event);
    if (scene()->mouseGrabberItem() == nullptr && event->buttons() == Qt::LeftButton) {
        // Make sure shift is not being pressed
        if ((event->modifiers() & Qt::ShiftModifier) == 0) {
            QPointF difference = _clickPos - mapToScene(event->pos());
            setSceneRect(sceneRect().translated(difference.x(), difference.y()));
        }
    }
}

void GraphicsViewOverride::drawBackground(QPainter* painter, const QRectF& r)
{
    QGraphicsView::drawBackground(painter, r);

    auto drawGrid = [&](double gridStep) {
        QRect windowRect = rect();
        QPointF tl = mapToScene(windowRect.topLeft());
        QPointF br = mapToScene(windowRect.bottomRight());

        double left = std::floor(tl.x() / gridStep - 0.5);
        double right = std::floor(br.x() / gridStep + 1.0);
        double bottom = std::floor(tl.y() / gridStep - 0.5);
        double top = std::floor(br.y() / gridStep + 1.0);

        // vertical lines
        for (int xi = int(left); xi <= int(right); ++xi) {
            QLineF line(xi * gridStep, bottom * gridStep, xi * gridStep, top * gridStep);

            painter->drawLine(line);
        }

        // horizontal lines
        for (int yi = int(bottom); yi <= int(top); ++yi) {
            QLineF line(left * gridStep, yi * gridStep, right * gridStep, yi * gridStep);
            painter->drawLine(line);
        }
    };

    auto const& flowViewStyle = StyleCollection::flowViewStyle();

    QPen pfine(flowViewStyle.FineGridColor, 1.0);

    painter->setPen(pfine);
    drawGrid(15);

    QPen p(flowViewStyle.CoarseGridColor, 1.0);

    painter->setPen(p);
    drawGrid(150);
}

void GraphicsViewOverride::showEvent(QShowEvent* event)
{
    QGraphicsView::showEvent(event);

    centerScene();
}

BasicGraphicsScene* GraphicsViewOverride::nodeScene()
{
    return dynamic_cast<BasicGraphicsScene*>(scene());
}

QPointF GraphicsViewOverride::scenePastePosition()
{
    QPoint origin = mapFromGlobal(QCursor::pos());

    QRect const viewRect = rect();
    if (!viewRect.contains(origin))
        origin = viewRect.center();

    return mapToScene(origin);
}
