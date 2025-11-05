#include "DragTreeWidget.h"
#include <QApplication>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QTreeWidgetItem>
#include <QDropEvent>
#include <QDataStream>
#include <QMimeData>
#include <QDrag>
#include <QPoint>
#include <QDebug>
#include <QPainter>

#define DRAG_DATA QStringLiteral("Drag/DragTree")

DragTreeWidget::DragTreeWidget(QWidget* parent) : QTreeWidget(parent)
{
	this->setDropIndicatorShown(true);
	this->setDragEnabled(true);
}

DragTreeWidget::~DragTreeWidget()
{
}

void DragTreeWidget::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasFormat(DRAG_DATA))
		event->accept();
	else
	{
		event->ignore();
	}
}

void DragTreeWidget::dragMoveEvent(QDragMoveEvent* event)
{
	if (event->mimeData()->hasFormat(DRAG_DATA))
	{
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}
	else{
		event->ignore();
	}
}

void DragTreeWidget::startDrag(Qt::DropActions supportedActions)
{
	QTreeWidgetItem* item = currentItem();
	if (NULL == item || item->childCount())
		return;
	QByteArray itemData;
	QDataStream dataStream(&itemData, QIODevice::WriteOnly);
	QIcon icon = item->icon(0);
	//QSize size = icon.availableSizes().at(0);
	QSize size = QSize(35, 35);
	QPixmap pixmap = icon.pixmap(0, size);
	dataStream << pixmap << item->text(0);
	QMimeData* mimeData = new QMimeData;
	mimeData->setData(DRAG_DATA, itemData);
	QDrag* drag = new QDrag(this);
	QPixmap pixmapDrag = SetDragCurorPixmap(pixmap, item->text(0));
	drag->setMimeData(mimeData);
	drag->setHotSpot(QPoint(pixmapDrag.width() / 2, pixmapDrag.height() / 2));
	drag->setPixmap(pixmapDrag);
	drag->exec(Qt::MoveAction | Qt::CopyAction | Qt::ActionMask);
	//É¾³ý
	delete drag;
	drag = NULL;
}

QPixmap DragTreeWidget::SetDragCurorPixmap(QPixmap pixmap, QString strText)
{
	QPixmap pixmapItem(200, 42);
	pixmapItem.fill(Qt::transparent);
	QPainter painter(&pixmapItem);
	QFont font("Microsoft YaHei");
	font.setPixelSize(18);
	painter.setFont(font);
	painter.setPen("#333333");
	painter.setBrush(QColor("#FFEFD5"));
	painter.drawRect(1, 1, pixmapItem.width() - 2, pixmapItem.height() - 2);
	painter.drawPixmap(10, (pixmapItem.height() - pixmap.height() - 2) / 2, pixmap);
	//»æÖÆÎÄ±¾
	painter.drawText(pixmap.width() + 20, 0, pixmapItem.width() - pixmap.width() - 20, pixmapItem.height(), Qt::AlignVCenter, strText);
	return pixmapItem;
}
