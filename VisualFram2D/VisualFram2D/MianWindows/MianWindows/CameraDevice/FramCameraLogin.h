#pragma once

#include <QDialog>
#include "CameraStruct.h"
#include "ui_FramCameraLogin.h"

class FramCameraLogin : public QDialog
{
	Q_OBJECT

public:
	FramCameraLogin(QWidget *parent = nullptr);
	~FramCameraLogin();
    
   /// <summary>
   /// 获取相机参数信息
   /// </summary>
   const CameraParam GetCameraInfo() const{
        return this->cameraParam;
    }

private:
    void Initail();
    void InitialSetUI();

private:
	Ui::FramCameraLoginClass ui;
    CameraParam cameraParam;
};

