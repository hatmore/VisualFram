#pragma once

#include <QDialog>
#include "ui_FramImageLayout.h"
#include <QButtonGroup>
//#include "mytitlebar.h"

class FramImageLayout : public QDialog
{
	Q_OBJECT

public:
	FramImageLayout(QWidget* parent = nullptr);
	~FramImageLayout();

	using Ptr = std::shared_ptr<FramImageLayout>;
	using ConstPtr = std::shared_ptr<const FramImageLayout>;

private:
	Ui::FramImageLayoutClass ui;

private:
	void initTitleBar();
	void paintEvent(QPaintEvent* event);	

signals:
	void ImageShowModeSig(int mode);  //0/1/2/3/4/5/6

private slots:
	void onButtonCloseClicked();
	void ImageSlot(bool);

private:	
	QButtonGroup* btnGroupRadio;	
};
