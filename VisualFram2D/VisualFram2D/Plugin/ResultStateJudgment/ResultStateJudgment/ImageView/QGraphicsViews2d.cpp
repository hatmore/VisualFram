#include "QGraphicsViews2d.h"
#include <QScrollBar>
#include <QMenu>
#include "ControlItem.h"
#include "BaseItem.h"
#include <QFileDialog>
#include <QAction>
#include <QLabel>
#include "QGraphicsScenes.h"
#include <QHBoxLayout>
#include <QThread>
#include <QPushButton>
#include <QWidgetAction>
#include "qmutex.h"

QGraphicsViews2d::QGraphicsViews2d(QWidget* parent) : QGraphicsView(parent)
{
	this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);//解决拖动是背景图片残影
	setDragMode(QGraphicsView::ScrollHandDrag);
	drawBg();
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setBackgroundBrush(Qt::gray);
	// 设置场景范围
	setSceneRect(INT_MIN / 2, INT_MIN / 2, INT_MAX, INT_MAX);
	// 反锯齿
	setRenderHints(QPainter::Antialiasing);
	imageItem = new ImageItem(this);
	scene = new QGraphicsScenes(this);
	scene->addItem(imageItem);
	this->setScene(scene);
	drawItem = new DrawItem;
	AddItems(drawItem);
	GrayValue = new QLabel(this); //显示灰度值
	GrayValue->setObjectName("GrayValue_X");
	GrayValue->setStyleSheet("color:rgb(200,255,200); background-color:rgba(50,50,50,160); font: Microsoft YaHei;font-size: 15px;");
	GrayValue->setVisible(false);
	GrayValue->setFixedWidth(800);
	//显示区域
	fWidget = new QWidget(this);
	fWidget->setFixedHeight(25);
	fWidget->setGeometry(0,0,800, 25);
	fWidget->setStyleSheet("background-color:rgba(0,0,0,0);");
	auto fLayout = new QHBoxLayout(fWidget);
	fLayout->setSpacing(0);
	fLayout->setMargin(0);
	fLayout->addWidget(GrayValue);
	fLayout->addStretch();
	connect(imageItem, &ImageItem::RGBValue, this, [&](QString InfoVal) {
		GrayValue->setText(InfoVal);
		});
	SetType(false, false);
}
void QGraphicsViews2d::DispImage(const QImage& Image)
{
	//加锁
	static QMutex mutex;
	QMutexLocker locker(&mutex);
	image = QPixmap::fromImage(Image);
	imageItem->w = image.width();
	imageItem->h = image.height();
	imageItem->setPixmap(image);
	GetFit();
}

void QGraphicsViews2d::DispPoint(QVector<QPointF>& list, QColor color)
{
	drawItem->ClearAll();
	drawItem->AddRegion(list);
}

void QGraphicsViews2d::AddItems(BaseItem* item)
{
	item->scale = &ZoomValue;
	this->scene->addItem(item);
}

//设定信息模块是否显示
void QGraphicsViews2d::SetType(bool InfoFlg, bool GrayValueFlg)
{
	imageItem->setAcceptHoverEvents(!InfoFlg);
	GrayValue->setVisible(!InfoFlg);
}

void QGraphicsViews2d::SetToFit(qreal val)
{
	ZoomFrame(val / ZoomValue);
	ZoomValue = val;
	QScrollBar* pHbar = this->horizontalScrollBar();
	pHbar->setSliderPosition(PixX);
	QScrollBar* pVbar = this->verticalScrollBar();
	pVbar->setSliderPosition(PixY);
}

void QGraphicsViews2d::ClearObj()
{
	foreach(auto imageItem, scene->items())
	{
		if (imageItem->type() == 10)
		{
			scene->removeItem(imageItem);
		}
	}
}

void QGraphicsViews2d::ZoomFrame(double value)
{
	this->scale(value, value);
}

void QGraphicsViews2d::GetFit()
{
	if (this->width() < 1 || image.width() < 1)
	{
		return;
	}
	//图片自适应方法
	double winWidth = this->width();
	double winHeight = this->height();
	double ScaleWidth = (image.width() + 1) / winWidth;
	double ScaleHeight = (image.height() + 1) / winHeight;
	double row1, column1;
	double s = 0;
	if (ScaleWidth >= ScaleHeight)
	{
		row1 = -(1) * ((winHeight * ScaleWidth) - image.height()) / 2;
		column1 = 0;
		s = 1 / ScaleWidth;
	}
	else
	{
		row1 = 0;
		column1 = -(1.0) * ((winWidth * ScaleHeight) - image.width()) / 2;
		s = 1 / ScaleHeight;
	}
	if (ZoomFit != s || PixX != column1 * s)
	{
		ZoomFit = s;
		PixX = column1 * s;
		PixY = row1 * s;
		SetToFit(s);
	}
}

void QGraphicsViews2d::drawBg()
{
	bgPix.fill(color1);
	QPainter painter(&bgPix);
	painter.fillRect(0, 0, 10, 10, color2);
	painter.fillRect(10, 10, 10, 10, color2);
	painter.end();
}

void QGraphicsViews2d::mousePressEvent(QMouseEvent* event)
{
	QGraphicsView::mousePressEvent(event);
}

void QGraphicsViews2d::resizeEvent(QResizeEvent* event)
{
	GetFit();
	QGraphicsView::resizeEvent(event);
}

void QGraphicsViews2d::mouseReleaseEvent(QMouseEvent* event)
{
	QGraphicsView::mouseReleaseEvent(event);
}

void QGraphicsViews2d::mouseDoubleClickEvent(QMouseEvent* event)
{
	SetToFit(ZoomFit);
	QGraphicsView::mouseDoubleClickEvent(event);
}

void QGraphicsViews2d::mouseMoveEvent(QMouseEvent* event)
{
	QGraphicsView::mouseMoveEvent(event);
}

void QGraphicsViews2d::wheelEvent(QWheelEvent* event)
{
	if ((event->delta() > 0) && (ZoomValue >= 50)) //最大放大到原始图像的50倍
	{
		return;
	}
	else if ((event->delta() < 0) && (ZoomValue <= 0.02))
	{
		return;
	}
	else
	{
		if (event->delta() > 0) //鼠标滚轮向前滚动
		{
			ZoomValue *= 1.1; //每次放大10%
			ZoomFrame(1.1);
		}
		else
		{
			ZoomFrame(0.9);
			ZoomValue *= 0.9; //每次缩小10%
		}
	}
}

void QGraphicsViews2d::drawBackground(QPainter* painter, const QRectF& rect)
{
	Q_UNUSED(rect);
	painter->drawPixmap(QPoint(), image);
}

void QGraphicsViews2d::paintEvent(QPaintEvent* event)
{
	QPainter paint(this->viewport());
	paint.drawTiledPixmap(QRect(QPoint(0, 0), QPoint(this->width(), this->height())), bgPix);  //绘制背景
	QGraphicsView::paintEvent(event);
}

//右键菜单
void QGraphicsViews2d::contextMenuEvent(QContextMenuEvent* event)
{
	QPoint pos = event->pos();
	pos = this->mapToGlobal(pos);
	QMenu* menu = new QMenu(this);
	//菜单样式
	QString menuStyle(
		"QMenu::item{"
		"color: rgb(0, 0, 0);"
		"}"
		"QMenu::item:hover{"
		"background-color: rgb(176, 190, 225);"
		"}"
		"QMenu::item:selected{"
		"background-color: rgb(176, 190, 225);"
		"}"
	);
	QString buttonStyle(
		"QPushButton{"
		"color: rgb(0, 0, 0);"
		"border: none;"
		"text-align: left;"
		"font: 9pt Microsoft YaHei;"
		"padding-left: 5px;"
		"padding-right: 5px;"
		"}"
		"QPushButton::hover{"
		"background-color: rgba(176, 190, 225, 200);"
		"}"
		"QPushButton::selected{"
		"background-color: rgba(176, 190, 255, 255);"
		"}"
	);
	menu->setStyleSheet(menuStyle);
	QPushButton* zoomInAction_buf = new QPushButton(menu);
	zoomInAction_buf->setStyleSheet(buttonStyle);
	zoomInAction_buf->setText("放大");
	zoomInAction_buf->setIconSize(QSize(23, 23));
	zoomInAction_buf->setIcon(QIcon(":/resource/zoom.png"));
	QWidgetAction* zoomInAction = new QWidgetAction(this);
	zoomInAction->setDefaultWidget(zoomInAction_buf);
	connect(zoomInAction_buf, &QPushButton::clicked, this, &QGraphicsViews2d::OnZoomInImage);
	connect(zoomInAction_buf, &QPushButton::clicked, menu, &QMenu::close);
	menu->addAction(zoomInAction);
	menu->addSeparator();
	QPushButton* zoomOutAction_buf = new QPushButton(menu);
	zoomOutAction_buf->setStyleSheet(buttonStyle);
	zoomOutAction_buf->setText("缩小");
	zoomOutAction_buf->setIconSize(QSize(23, 23));
	zoomOutAction_buf->setIcon(QIcon(":/resource/zoom_out.png"));
	QWidgetAction* zoomOutAction = new QWidgetAction(this);
	zoomOutAction->setDefaultWidget(zoomOutAction_buf);
	connect(zoomOutAction_buf, &QPushButton::clicked, this, &QGraphicsViews2d::OnZoomOutImage);
	connect(zoomOutAction_buf, &QPushButton::clicked, menu, &QMenu::close);
	menu->addAction(zoomOutAction);
	menu->addSeparator();
	QPushButton* presetAction_buf = new QPushButton(menu);
	presetAction_buf->setStyleSheet(buttonStyle);
	presetAction_buf->setText("自适应图像");
	presetAction_buf->setIconSize(QSize(22, 22));
	presetAction_buf->setIcon(QIcon(":/resource/fit.png"));
	QWidgetAction* presetAction = new QWidgetAction(this);
	presetAction->setDefaultWidget(presetAction_buf);
	connect(presetAction_buf, &QPushButton::clicked, this, &QGraphicsViews2d::OnFitImage);
	connect(presetAction_buf, &QPushButton::clicked, menu, &QMenu::close);
	menu->addAction(presetAction);
	menu->exec(pos);
}

void QGraphicsViews2d::OnZoomInImage()
{
	double tmp_buf = ZoomValue;
	tmp_buf *= 1.1;
	double tmp = tmp_buf / ZoomValue;
	ZoomValue *= tmp;
	BaseItem::SetScale(ZoomValue);
	this->scale(tmp, tmp);
}

void QGraphicsViews2d::OnZoomOutImage()
{
	double tmp_buf = ZoomValue;
	tmp_buf *= 0.9;
	double tmp = tmp_buf / ZoomValue;
	ZoomValue *= tmp;
	BaseItem::SetScale(ZoomValue);
	this->scale(tmp, tmp);
}

void QGraphicsViews2d::OnFitImage()
{
	SetToFit(ZoomFit);
}
