#pragma execution_character_set("utf-8")
#include "FramCameraLogin.h"
#include <QDesktopWidget>
#include <QFileDialog>
#include <QComboBox>
FramCameraLogin::FramCameraLogin(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
    Initail();
}

FramCameraLogin::~FramCameraLogin()
{}

void FramCameraLogin::Initail()
{
    InitialSetUI();
    connect(ui.btnLogin, &QPushButton::clicked, [this]() {
        cameraParam.deviceName = ui.txtCameraName->text();
        cameraParam.strURL = ui.txtCamerURL->text();
        cameraParam.savePath = ui.txtSavePath->text();
        cameraParam.saveTime = ui.spinBoxSaveTime->value();
        cameraParam.isSave = ui.checkBoxSave->isChecked();
        cameraParam.virtualCamera = ui.checkBoxVirtual->isChecked();
        cameraParam.camerIndex = ui.spinBoxCamerIndex->value();
        cameraParam.sampleFrequencyTime = ui.spinBoxSampleTime->value();
        this->close();
        });
}


void FramCameraLogin::InitialSetUI() 
{
    this->setWindowTitle("相机设置");
    this->setWindowIcon(QIcon(":/MianWindows/image/Login.svg"));
    // 去掉问号，只保留关闭
    this->setWindowFlags(Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint);
    this->installEventFilter(this);
    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);
}