#pragma once
#pragma execution_character_set("utf-8")

#include <QTreeWidget>

class DragTreeWidget : public QTreeWidget
{
	Q_OBJECT

public:
	explicit DragTreeWidget(QWidget* parent = 0);
	~DragTreeWidget();

private:
	QPixmap SetDragCurorPixmap(QPixmap pixmap, QString strText);

protected:
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dragMoveEvent(QDragMoveEvent* event) override;
	void startDrag(Qt::DropActions supportedActions) override;
};
