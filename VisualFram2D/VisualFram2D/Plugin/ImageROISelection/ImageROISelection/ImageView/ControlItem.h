#pragma once

#include <QObject>
#include <QAbstractGraphicsShapeItem>
#include <QPointF>
#include <QPen>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QCursor>
#include <QKeyEvent>
#include <QList>
#include <QDebug>

class ControlItem : public QObject, public QAbstractGraphicsShapeItem
{
    Q_OBJECT

public:
    explicit ControlItem(QGraphicsItemGroup* parent, QPointF p, int type);
    QPointF GetPoint();
    QPointF GetBefPoint(){return BefPoint;}
    void SetPoint(QPointF p);
    qreal dX(){return dx;};
    qreal dY(){return dy;};
    int index=0;

protected:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;   
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

signals:
    void positionChanged();  // 添加这行信号声明

private:
    QPen myPen = this->pen();
    QPointF point;
    int pointType;
    QRectF MQRect;
    qreal dx;
    qreal dy;
    QPointF BefPoint;
};
