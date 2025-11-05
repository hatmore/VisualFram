#include "ImageWithRect.h"
#include <QPainter>
#include <QCursor>
#include <QtWidgets/qinputdialog.h>
#pragma execution_character_set("utf-8")


QRectF RectItem::boundingRect() const {
    return m_rect;
}

// 获取当前矩形信息
void RectItem::GetRect(MRectangle& MRect) const
{
    QPointF scenePos = pos();  // 获取场景坐标
    MRect.col = scenePos.x();
    MRect.row = scenePos.y();
    MRect.width = m_rect.width();
    MRect.height = m_rect.height();
}

void RectItem::SetRect(const MRectangle& MRect) {
    prepareGeometryChange();
    m_rect = QRectF(0, 0, MRect.width, MRect.height);
    setPos(MRect.col, MRect.row);  // 场景坐标
    update();
    updateControlPositions();
}

QVariant RectItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionHasChanged) 
    {
        m_rect.moveTo(value.toPointF());
        updateControlPositions();
        updateLabelPosition();
        emit geometryChanged(QRectF(pos(), m_rect.size()));  // 发送场景坐标 + 局部大小
    }
    return BaseItem::itemChange(change, value);
}


void RectItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    BaseItem::mouseMoveEvent(event);
    if (isControlPointMoving) {
        updateRectFromControls();
    }
}

void RectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    BaseItem::mouseReleaseEvent(event);
    if (isControlPointMoving) {
        isControlPointMoving = false;
        emit geometryChanged(QRectF(pos(), m_rect.size()));  
    }
}

void RectItem::updateRectFromControls()
{
    ControlItem* control = qobject_cast<ControlItem*>(sender());
    if (!control) return;

    int index = ControlList.indexOf(control);
    if (index == -1) return;

    QPointF newPos = control->pos();  // 控制点的局部坐标
    QRectF newRect = m_rect;

    switch (index) {
    case 0: newRect.setTopLeft(newPos); break;
    case 1: newRect.setTopRight(newPos); break;
    case 2: newRect.setBottomRight(newPos); break;
    case 3: newRect.setBottomLeft(newPos); break;
    case 4: newRect.setTop(newPos.y()); break;
    case 5: newRect.setRight(newPos.x()); break;
    case 6: newRect.setBottom(newPos.y()); break;
    case 7: newRect.setLeft(newPos.x()); break;
    }
    m_rect = newRect;
    updateControlPositions();
    updateLabelPosition();
    emit geometryChanged(QRectF(pos(), m_rect.size()));

}

bool RectItem::UpDate(int index) {
    qDebug() << "RectangleItem updated, index:" << index;
    return true;
}

//void RectItem::paint(QPainter* painter,
//    const QStyleOptionGraphicsItem* option,
//    QWidget* widget) 
//{
//    QPen redPen(Qt::red);
//    //redPen.setWidthF(0.5);
//    painter->setPen(redPen);
//    painter->drawRect(m_rect);
//}

// 更新文本标签位置
void RectItem::updateLabelPosition()
{
    if (m_label) {
        QPointF center = m_rect.center();
        QRectF labelRect = m_label->boundingRect();
        m_label->setPos(center.x() - labelRect.width() / 2,
            center.y() - labelRect.height() / 2);
    }
}


void RectItem::initControlItems()
{
    qDeleteAll(ControlList);
    ControlList.clear();
    const QPointF positions[8] = {
        m_rect.topLeft(),     
        m_rect.topRight(),    
        m_rect.bottomRight(), 
        m_rect.bottomLeft(),  
        QPointF(m_rect.center().x(), m_rect.top()),    
        QPointF(m_rect.right(), m_rect.center().y()), 
        QPointF(m_rect.center().x(), m_rect.bottom()), 
        QPointF(m_rect.left(), m_rect.center().y())    
    };

    for (int i = 0; i < 8; ++i) {
        ControlItem* control = new ControlItem(this, positions[i], i);
        control->setPen(QPen(Qt::red));
        control->setBrush(Qt::white);
        control->setFlag(QGraphicsItem::ItemIsMovable);
        control->setFlag(QGraphicsItem::ItemSendsGeometryChanges);

        // 连接信号
        connect(control, &ControlItem::positionChanged, this, [this]() {
            this->updateRectFromControls();
            });
        ControlList.append(control);
    }  
}

void RectItem::updateControlPositions()
{
    if (ControlList.size() < 8) return;

    const QPointF positions[8] = {
        m_rect.topLeft(),     
        m_rect.topRight(),    
        m_rect.bottomRight(), 
        m_rect.bottomLeft(),  
        QPointF(m_rect.center().x(), m_rect.top()),    
        QPointF(m_rect.right(), m_rect.center().y()),  
        QPointF(m_rect.center().x(), m_rect.bottom()), 
        QPointF(m_rect.left(), m_rect.center().y()) 
    };

    for (int i = 0; i < 8; ++i) {
        ControlList[i]->SetPoint(positions[i]);
    }
}

//void RectItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
//{
//    QGraphicsItem::mouseDoubleClickEvent(event);
//    if (!m_rect.contains(event->pos())) {
//        return;
//    }
//
//    bool ok;
//    int classId = QInputDialog::getInt(
//        nullptr,                     
//        tr("输入类别ID"),           
//        tr("请输入类别id:"),        
//        m_classId,                   
//        0,                           
//        2147483647,                  
//        1,                          
//        &ok                         
//    );
//
//    if (ok) {
//        m_classId = classId;  
//        qDebug() << "矩形" << m_index << "的类别ID已设置为:" << m_classId;
//        if (m_label) {
//            m_label->setPlainText(QString::number(m_index) + " (ID:" + QString::number(m_classId) + ")");
//            updateLabelPosition();
//        }
//        emit classIdChanged(m_index, m_classId);
//    }
//}


// 在 RectItem 的cpp文件中添加
//void RectItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
//{
//    QMenu menu;
//    QAction* deleteAction = menu.addAction("删除");
//
//    QAction* selectedAction = menu.exec(event->screenPos());
//
//    if (selectedAction == deleteAction) {
//        emit requestDelete(m_index); // 发射删除请求信号
//    }
//
//    event->accept();
//}


void RectItem::setSelected(bool selected)
{
    m_selected = selected;
    update(); // 重绘以显示选中状态
}

void RectItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
      // 根据选中状态设置不同的画笔
    QPen pen;
    if (m_selected) {
        pen.setColor(Qt::yellow); // 选中时用黄色边框
        pen.setWidth(3);
    } else {
        pen.setColor(m_rectColor); // 使用随机颜色
        pen.setWidth(2);
    }
    
    // 设置画笔（边缘），不使用画刷（内部不填充）
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush); // 关键：内部不填充
    
    // 绘制矩形边框
    painter->drawRect(m_rect);
    
    // 调用基类的绘制（如果需要）
    BaseItem::paint(painter, option, widget);
}

void RectItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    // 调用基类处理
    BaseItem::mousePressEvent(event);

    // 发射鼠标按下信号
    emit mousePressed(this);

    event->accept();
}

void RectItem::setColor(const QColor& color)
{
    m_rectColor = color;
    update(); // 重绘以显示新颜色
}