#pragma once

#include <QDialog>
#include "ui_FramSoftRegister.h"

class FramSoftRegister : public QDialog
{
	Q_OBJECT

public:
	FramSoftRegister(QWidget *parent = nullptr);
	~FramSoftRegister();

private:
	Ui::FramSoftRegisterClass ui;
};

