#pragma once

#include <QObject>
#include "ControlItem.h"
#include <QList>

//矩形
struct MRectangle
{
	MRectangle()
	{
	}

	MRectangle(float x_, float y_, float width_, float height_)
	{
		x = x_;
		y = y_;
		width = width_;
		height = height_;
	}
	float x;
	float y;
	float width;
	float height;
};

//旋转矩形
struct MRotatedRect
{
	MRotatedRect()
	{
	}

	MRotatedRect(float x_, float y_, float Phi, float width_, float height_)
	{
		x = x_;
		y = y_;
		phi = Phi;
		width = width_;
		height = height_;
	}
	float x;
	float y;
	float phi;
	float width;
	float height;
};

//圆
struct MCircle
{
	MCircle()
	{
	}

	MCircle(float x_, float y_, float Radius)
	{
		x = x_;
		y = y_;
		radius = Radius;
	}
	float x;
	float y;
	float radius;
};

//多边形
struct MPolygon
{
	MPolygon()
	{
	}

	MPolygon(QList<QPointF> points)
	{
		listPoint = points;
	}
	QList<QPointF> listPoint;
};

//同心圆
struct CCircle
{
	CCircle()
	{
	}

	CCircle(float x_, float y_, float small_radius, float big_radius)
	{
		x = x_;
		y = y_;
		radiusMin = small_radius;
		radiusMax = big_radius;
	}
	float x;
	float y;
	float radiusMin;
	float radiusMax;
};

//卡尺
struct CaliperP
{
	CaliperP()
	{
	}

	CaliperP(float x, float y, float Len1, float Len2, float Angle, qreal X1, qreal Y1, qreal X2, qreal Y2, qreal Height, QPointF PP1, QPointF PP2)
	{
		row = y;
		col = x;
		len1 = Len1;
		len2 = Len2;
		angle = Angle;
		x1 = X1;
		y1 = Y1;
		x2 = X2;
		y2 = Y2;
		height = Height;
		pp1 = PP1;
		pp2 = PP2;
	}
	float row;
	float col;
	float len1;
	float len2;
	float angle;
	qreal x1;
	qreal y1;
	qreal x2;
	qreal y2;
	qreal height;
	QPointF pp1;
	QPointF pp2;
};

//******基类******
class BaseItem : public QObject, public QGraphicsItemGroup
{
	Q_OBJECT

public:
	enum ItemType
	{
		AutoItem = 0,        // 自定义类型
		Circle,              // 圆
		Ellipse,             // 椭圆
		Concentric_Circle,   // 同心圆			
		Rectangle,           // 矩形mw
		RectangleR,          // 旋转矩形
		Square,              // 正方形
		Polygon,             // 多边形		
		LineObj              // 直线
	};

	void SetIndex(int num);
	virtual bool UpDate(int index) = NULL;
	static void SetScale(double value);
	QList<ControlItem* > ControlList;
	qreal* scale;
	qreal scaler;
	static qreal ContrSize;
	void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget) override;
	/// <summary>
	/// 获得类型
	/// </summary>
	/// <returns></returns>
	const ItemType GetType() const { return types; };

protected:
	BaseItem(QPointF center, ItemType type);
	virtual void focusInEvent(QFocusEvent* event) override;
	virtual void focusOutEvent(QFocusEvent* event) override;

protected:
	QPointF center;
	ItemType types;
	QPen isSelected;
	QPen noSelected;
	QColor fillColor;
	QPen thisPen;
	qreal lineWidth = 2;  //控制点初始尺寸
	QString itemDiscrib = QString::fromLocal8Bit("");
};

//******绘制结果数据******
class DrawItem : public BaseItem
{
	Q_OBJECT

public:
	DrawItem();
	void AddLine(QVector<QPointF>& Plist) { LineList.push_back(Plist); };
	void AddRegion(QVector<QPointF>& Plist) { ROIList.push_back(Plist); };
	void ClearAll() { LineList.clear(); ROIList.clear(); };
	void Refresh();
	void SetDispROI(QRectF& Rec) { returnRec = Rec; };

protected:
	int type() const override { return 100; };
	bool UpDate(int index) { return true; };
	virtual QRectF boundingRect() const override;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

private:
	QPen DrawPen;
	QRectF returnRec;
	QVector<QVector<QPointF>> LineList;
	QVector<QVector<QPointF>> ROIList;
};

//******矩形******
class RectangleItem : public BaseItem
{
	Q_OBJECT

public:
	RectangleItem(qreal x, qreal y, qreal width, qreal height);
	void SetRect(MRectangle MRect);
	void GetRect(MRectangle& MRect) const;

protected:
	virtual QRectF boundingRect() const override;
	bool UpDate(int index) override;
	virtual void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget) override;
};

//******旋转矩形******
class RectangleRItem : public BaseItem
{
	Q_OBJECT

public:
	RectangleRItem(qreal x, qreal y, qreal Lenth1, qreal Lenth2, qreal Pi);
	void GetRotatedRect(MRotatedRect& MRRect) const;

protected:
	virtual QRectF boundingRect() const override;
	bool UpDate(int index) override;
	virtual void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget) override;

protected:
	qreal angle = 0;
	qreal lenth1 = 0;
	qreal lenth2 = 0;
	QPointF Pa1;
	QPointF Pa2;
	QPointF Pa3;
	QPointF Pa4;
	QPointF PArrow;
};

//******多边形******
class PolygonItem :public BaseItem
{
	Q_OBJECT

public:
	PolygonItem();
	PolygonItem(QList<QPointF> list);
	QPointF getCentroid(QList<QPointF> list);
	void getMaxLength();
	void GetPolygon(MPolygon& mpolygon) const;
    /** 结束绘制、创建可拖拽的顶点控制点、闭合多边形 */
    void FinishCreate();

public slots:
	void pushPoint(QList<QPointF> list, bool isCenter);

protected:
	virtual QRectF boundingRect() const override;
	bool UpDate(int index) override;
	virtual void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget) override;

protected:
	qreal Radius;
	bool Finished;
	QList<QPointF> init_points;
};

//******椭圆******
class EllipseItem : public RectangleRItem
{
	Q_OBJECT

public:
	EllipseItem(qreal x, qreal y, qreal Lenth1, qreal Lenth2, qreal Pi);

protected:
	virtual void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget) override;
};

//******圆******
class CircleItem :public BaseItem
{
	Q_OBJECT

public:
	CircleItem(qreal x, qreal y, qreal R);
	void GetCircle(MCircle& Cir) const;

protected:
	virtual QRectF boundingRect() const override;
	bool UpDate(int index) override;
	virtual void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget) override;
	qreal Radius;
};

//******同心圆******
class ConcentricCircleItem :public BaseItem
{
	Q_OBJECT

public:	
	ConcentricCircleItem(qreal x, qreal y, qreal RadiusMin, qreal RadiusMax);
	void GetConcentricCircle(CCircle& CCir);
	bool concentric_circle_init_state = false;	
	int segment_line_num = 0;
	std::vector<QPointF> ring_small_points = std::vector<QPointF>(1000);
	std::vector<QPointF> ring_big_points = std::vector<QPointF>(1000);	

protected:
	virtual QRectF boundingRect() const override;
	bool UpDate(int index) override;
	virtual void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget) override;
	qreal radiusMin;
	qreal radiusMax;		
};

//******直线******
class LineItem :public BaseItem
{
	Q_OBJECT

public:
	LineItem(qreal x1, qreal y1, qreal x2, qreal y2);

protected:
	bool UpDate(int index) override;
	virtual void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget) override;
	QPointF P1;
	QPointF P2;
};

//******卡尺******
class Caliper :public BaseItem
{
	Q_OBJECT

public:
	Caliper(qreal x1, qreal y1, qreal x2, qreal y2, qreal height);	
	void SetCaliper(QPointF p1, QPointF p2);
	void GetCaliper(CaliperP& CP);
	bool caliper_init_state = false;
	int segment_line_num = 0;
	std::vector<QPointF> line_small_points = std::vector<QPointF>(1000);
	std::vector<QPointF> line_big_points = std::vector<QPointF>(1000);

protected:
	virtual QRectF boundingRect() const override;
	bool UpDate(int index) override;
	virtual void paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget) override;	
	QPointF P1;
	QPointF P2;
	qreal Height;
	qreal angle;
	qreal Lenth;
};
