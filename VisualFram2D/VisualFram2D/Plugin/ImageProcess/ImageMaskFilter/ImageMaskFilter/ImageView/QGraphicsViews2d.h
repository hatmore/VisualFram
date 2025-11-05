#pragma once
#pragma execution_character_set("utf-8")

#include <QGraphicsView>
#include <QScrollBar>
#include <QPixmap>
#include "ImageItem.h"
#include "QGraphicsScenes.h"
#include "BaseItem.h"

class QLabel;
class QGraphicsScenes;
class QMouseEvent;

class QGraphicsViews2d : public QGraphicsView
{
    Q_OBJECT
public:
    QGraphicsViews2d(QWidget* parent = nullptr);
    void SetToFit(qreal val);
    void ClearObj();
    QGraphicsScenes* scene;
    DrawItem* drawItem;

private:
    void InitailMenu();

public slots:
    void DispImage(const QImage& Image); //显示图片
    void DispPoint(QVector<QPointF>& list, QColor color = QColor(0, 255, 0));//显示点集合
    void AddItems(BaseItem* item); //添加ROI	
    void SetType(bool InfoFlg, bool GrayValueFlg);//设定显示信息

protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void drawBackground(QPainter* painter, const QRectF& rect) override;
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void contextMenuEvent(QContextMenuEvent* event) override;   //右键菜单	

private:
    void OnZoomInImage();
    void OnZoomOutImage();
    void ZoomFrame(double value);
    void OnFitImage();
    void GetFit();
    QScrollBar* pHbar;
    QScrollBar* pVbar;
    double ZoomValue = 1;
    double ZoomFit = 1;
    double PixX = 0;
    double PixY = 0;
    QPixmap image;
    ImageItem* imageItem{ nullptr };
    QWidget* fWidget{ nullptr };
    QLabel* GrayValue{ nullptr };//显示灰度值

    QMenu* contextMenu{ nullptr };

private:
    int MRSize = 8; //拖拽区域大小
    QColor color1 = QColor(54, 54, 54);           //颜色1
    QColor color2 = QColor(69, 69, 69, 255);      //颜色2
    QPixmap bgPix = QPixmap(20, 20);              //背景图片
    void drawBg();
};

