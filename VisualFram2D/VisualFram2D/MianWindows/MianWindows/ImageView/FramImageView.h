#pragma once

#include <QtWidgets/QWidget>
#include "ui_FramImageView.h"
#include "QGraphicsViews.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "FramImageLayout.h"

#define MAX_IMAGESHOW_VIEWS 9

class FramImageView : public QWidget
{
    Q_OBJECT

public:
	FramImageView(QWidget* parent = nullptr);
	~FramImageView();
    void SetFramImageLayout(FramImageLayout* frm_image_layout);

private:
	Ui::FramImageViewClass ui;

public:
	/// <summary>
	/// Çå¿ÕÍ¼Ïñ
	/// </summary>
	void RemoveLayout();

public slots:
	void ImageShowSlot1();
	void ImageShowSlot2();
	void ImageShowSlot3();
	void ImageShowSlot4();
	void ImageShowSlot6A();
	void ImageShowSlot6B();
	void ImageShowSlot9();

public slots:
    void DispImage(const int &index, const QImage& qimage);

private:
	void InitialFrmImageView();
	void SwitchImageShowMode(const int mode);

private:
	FramImageLayout* frmImageLayout;

private:		
	//1Í¼Ïñ
	QHBoxLayout* hlay = nullptr;
	//2Í¼Ïñ
	QVBoxLayout* vlay2_1 = nullptr;
	QVBoxLayout* vlay2_2 = nullptr;
	//3Í¼Ïñ
	QVBoxLayout* vlay3_1 = nullptr;
	QVBoxLayout* vlay3_2 = nullptr;
	QHBoxLayout* hlay3_2_1 = nullptr;
	QHBoxLayout* hlay3_2_2 = nullptr;
	//4Í¼Ïñ
	QVBoxLayout* vlay4 = nullptr;
	QHBoxLayout* hlay4_1 = nullptr;
	QHBoxLayout* hlay4_1_1 = nullptr;
	QHBoxLayout* hlay4_1_2 = nullptr;
	QHBoxLayout* hlay4_2 = nullptr;
	QHBoxLayout* hlay4_2_1 = nullptr;
	QHBoxLayout* hlay4_2_2 = nullptr;
	//6Í¼ÏñA	
	QVBoxLayout* vlay6_1 = nullptr;
	QVBoxLayout* vlay6_1_1 = nullptr;
	QVBoxLayout* vlay6_1_2 = nullptr;
	QVBoxLayout* vlay6_2 = nullptr;
	QVBoxLayout* vlay6_2_1 = nullptr;
	QVBoxLayout* vlay6_2_2 = nullptr;
	QVBoxLayout* vlay6_3 = nullptr;
	QVBoxLayout* vlay6_3_1 = nullptr;
	QVBoxLayout* vlay6_3_2 = nullptr;	
	//6Í¼ÏñB				
	QVBoxLayout* vlay6B = nullptr;
	QHBoxLayout* hlay6B_1 = nullptr;
	QVBoxLayout* vlay6B_1_1 = nullptr;
	QVBoxLayout* vlay6B_1_2 = nullptr;
	QHBoxLayout* hlay6B_1_2_1 = nullptr;
	QHBoxLayout* hlay6B_1_2_2 = nullptr;
	QHBoxLayout* hlay6B_2 = nullptr;
	QVBoxLayout* vlay6B_2_1 = nullptr;
	QVBoxLayout* vlay6B_2_2 = nullptr;
	QVBoxLayout* vlay6B_2_3 = nullptr;
	//9Í¼Ïñ
	QVBoxLayout* vlay9 = nullptr;
	QHBoxLayout* hlay9_1 = nullptr;
	QVBoxLayout* vlay9_1_1 = nullptr;
	QVBoxLayout* vlay9_1_2 = nullptr;
	QVBoxLayout* vlay9_1_3 = nullptr;
	QHBoxLayout* hlay9_2 = nullptr;
	QVBoxLayout* vlay9_2_1 = nullptr;
	QVBoxLayout* vlay9_2_2 = nullptr;
	QVBoxLayout* vlay9_2_3 = nullptr;
	QHBoxLayout* hlay9_3 = nullptr;
	QVBoxLayout* vlay9_3_1 = nullptr;
	QVBoxLayout* vlay9_3_2 = nullptr;
	QVBoxLayout* vlay9_3_3 = nullptr;

     QGraphicsViews *ptrQGraphicsViews[MAX_IMAGESHOW_VIEWS];
};
