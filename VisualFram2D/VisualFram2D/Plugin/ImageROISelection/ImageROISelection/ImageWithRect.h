#pragma once
#include <QWidget>
#include <QImage>
#include <QRect>
#include <QPoint>
#include <QMouseEvent> 
#include <QCursor>      
#include <QPainter>     
#include"ImageView/BaseItem.h"
#include"qmenu.h"
#include <QGraphicsView> 
#include <QRandomGenerator>

class RectItem : public BaseItem
{
    Q_OBJECT

public:
    RectItem(qreal x, qreal y, qreal width, qreal height, int index, int classId = 0)  // 添加classId参数
        :BaseItem(QPointF(x, y), Rectangle),
        m_rect(0, 0, width, height), m_index(index), m_classId(classId)  // 初始化m_classId
    {
        setPos(x, y);
        // 生成随机颜色
        int r = QRandomGenerator::global()->bounded(256);
        int g = QRandomGenerator::global()->bounded(256);
        int b = QRandomGenerator::global()->bounded(256);

        // 确保颜色足够亮
        if (r + g + b < 300) {
            int channel = QRandomGenerator::global()->bounded(3);
            if (channel == 0) r = 255;
            else if (channel == 1) g = 255;
            else b = 255;
        }

        m_rectColor = QColor(r, g, b);

        initControlItems();
        setFlag(QGraphicsItem::ItemIsMovable);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges);

        // 初始化文本标签 - 显示序号（从1开始）
        m_label = new QGraphicsTextItem(QString::number(index + 1), this);
        m_label->setDefaultTextColor(Qt::white);
        updateLabelPosition();
    }

    void initControlItems();
    void updateControlPositions();
    void updateLabelPosition();

    void setClassId(int classId) { m_classId = classId; }
    int getClassId() const { return m_classId; }

    QRectF boundingRect() const override;
    void GetRect(MRectangle& MRect) const;
    void SetRect(const MRectangle& MRect);

    void setSelected(bool selected);
    bool isSelected() const { return m_selected; }

    QColor getColor() const { return m_rectColor; }
    void setColor(const QColor& color);

    int getIndex() const { return m_index; }
    void setIndex(int index)
    {
        m_index = index;
        if (m_label) {
            m_label->setPlainText(QString::number(index + 1)); // 修改这里：index + 1
            updateLabelPosition();
        }
        update();
    }

signals:
    void geometryChanged(const QRectF& newGeometry);
    void requestDelete(int index);
    void mousePressed(RectItem* item);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    void updateRectFromControls();
    bool UpDate(int index) override;

    QRectF m_rect;
    QGraphicsTextItem* m_label = nullptr;
    int m_index = 0;
    int m_classId = -1;
    QColor m_rectColor;
    bool m_selected = false;
    bool isControlPointMoving = false;
};