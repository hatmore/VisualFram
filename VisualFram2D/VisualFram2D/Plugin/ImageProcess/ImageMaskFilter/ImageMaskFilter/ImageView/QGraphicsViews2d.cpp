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
    InitailMenu();
    imageItem = new ImageItem(this);
    scene = new QGraphicsScenes(this);
    scene->addItem(imageItem);
    this->setScene(scene);
    drawItem = new DrawItem;
    AddItems(drawItem);
    GrayValue = new QLabel(this); //显示灰度值
    GrayValue->setObjectName("GrayValue_X");
    GrayValue->setStyleSheet("border: 1px solid #44484D;color:#8A6BBE; background-color:rgba(50,50,50,160); \
                                font: Microsoft YaHei;font-weight:Bold; font-size: 16px;");
    GrayValue->setVisible(false);
    GrayValue->setFixedWidth(this->width());
    //显示区域
    fWidget = new QWidget(this);
    fWidget->setFixedHeight(25);
    fWidget->setGeometry(0, 0, this->width(), 25);
    fWidget->setStyleSheet("border:none;background-color:transparent;");
    auto fLayout = new QHBoxLayout(fWidget);
    fLayout->setSpacing(4);
    fLayout->setMargin(0);
    fLayout->addWidget(GrayValue);
    fLayout->addStretch();
    connect(imageItem, &ImageItem::RGBValue, this, [&](QString InfoVal) {
        GrayValue->setText(InfoVal);
        });
    SetType(false, false);
}

void QGraphicsViews2d::InitailMenu()
{
    contextMenu = new QMenu(this);
    //菜单样式
    QString menuStyle(
        "QMenu{"
        " background:transparent;"
        " border:none;"
        " padding:2px;"
        "}"
        "QMenu::separator{"
        " height:1px;"
        " background:rgba(255,255,255,55);"
        " margin:2px 6px;"
        "}"
        "QMenu::item{"
        " background:transparent;"
        "}"
        "QMenu::item:selected{"
        " background:transparent;"
        "}"
    );

    // 半透明按钮：常态低透明，Hover 提升对比
    QString buttonStyle(
        "QPushButton{"
        " color:#F5F5F5;"
        " background:rgba(0,0,0,95);"
        " border:1px solid rgba(255,255,255,40);"
        " text-align:left;"
        " font:10pt 'Microsoft YaHei';"
        " padding:2px 6px;"
        " border-radius:4px;"
        "}"
        "QPushButton:hover{"
        " background:rgba(255,255,255,55);"
        " color:#FFFFFF;"
        "}"
        "QPushButton:pressed{"
        " background:rgba(255,255,255,80);"
        "}"
    );

    contextMenu->setStyleSheet(menuStyle);
    auto createButtonAction = [&](const QString& text, const QIcon& icon, auto slot) {
        QPushButton* btn = new QPushButton(contextMenu);
        btn->setStyleSheet(buttonStyle);
        btn->setText(text);
        btn->setIconSize(QSize(22, 22));
        btn->setIcon(icon);
        QWidgetAction* wa = new QWidgetAction(this);
        wa->setDefaultWidget(btn);
        connect(btn, &QPushButton::clicked, this, slot);
        connect(btn, &QPushButton::clicked, contextMenu, &QMenu::close);
        contextMenu->addAction(wa);
    };
    createButtonAction("放大", QIcon(":/qdialog/image/OuiMagnifyWithPlus.png"), &QGraphicsViews2d::OnZoomInImage);
    contextMenu->addSeparator();
    createButtonAction("缩小", QIcon(":/qdialog/image/OuiMagnifyWithMinus.png"), &QGraphicsViews2d::OnZoomOutImage);
    contextMenu->addSeparator();
    createButtonAction("自适应图像", QIcon(":/qdialog/image/MdiMagnifyScan.png"), &QGraphicsViews2d::OnFitImage);
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
    BaseItem::ItemType item_type = item->GetType();
    switch (item_type) {
    case BaseItem::AutoItem:
        break;
    case BaseItem::Circle:
        break;
    case BaseItem::Ellipse:
        break;
    case BaseItem::Concentric_Circle:
        break;
    case BaseItem::Rectangle:
        break;
    case BaseItem::RectangleR:
        break;
    case BaseItem::Square:
        break;
    case BaseItem::Polygon: {
        //scene->StartCreate();
        connect(scene, SIGNAL(updatePoint(QList<QPointF>, bool)), item, SLOT(pushPoint(QList<QPointF>, bool)));
    }
                          break;
    case BaseItem::LineObj:
        break;
    default:
        break;
    }
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
    foreach(auto item, scene->items()) {
        if (item->type() == 10) {
            scene->removeItem(item);
            delete item;
            item = nullptr;
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
    GrayValue->setFixedWidth(this->width());
    fWidget->setGeometry(0, 0, this->width(), 25);
    QGraphicsView::resizeEvent(event);
}

void QGraphicsViews2d::mouseReleaseEvent(QMouseEvent* event)
{
    QGraphicsView::mouseReleaseEvent(event);
}

void QGraphicsViews2d::mouseDoubleClickEvent(QMouseEvent* event)
{
    // 右键双击：直接恢复自适应
    if (event->button() == Qt::RightButton) {
        SetToFit(ZoomFit);
    }
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
    contextMenu->exec(pos);
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
