#include "FramUserLogin.h"
#include <QDesktopWidget>
#include <QFileDialog>
#include <QComboBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVariant>
#include <QMessageBox>
#include <QJsonArray>
#include "../Log/frmLog.h"
#include "../StaticGlobalVariable.h"

FramUserLogin::FramUserLogin(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	Initial();

    FrmLog* ptr_framlog = FrmLog::FrmLogInstance();
	QObject::connect(this, &FramUserLogin::SendLogInfoSig, ptr_framlog, &FrmLog::LogMassageShowSlot);
}

FramUserLogin::~FramUserLogin()
{}

void FramUserLogin::Initial()
{
	InitialSetUI();
	InitailPushButton();
	DeserializeUserLoginParam();
	
}

void FramUserLogin::InitialSetUI()
{
	this->setWindowTitle("用户登录");
	this->setWindowIcon(QIcon(":/MianWindows/image/Login.svg"));
	// 去掉问号，只保留关闭
	this->setWindowFlags(Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint);
	this->installEventFilter(this);
	QDesktopWidget* desktop = QApplication::desktop();
	move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);
}

void FramUserLogin::InitailPushButton()
{

}

void FramUserLogin::DeserializeUserLoginParam()
{
	QString application_path = qApp->applicationDirPath();

	QString file_name = application_path + "//userlogin//user.cfg";
	if (!QFileInfo::exists(file_name)) {
		emit SendLogInfoSig("用户登录文件丢失，登录失败! ", LOGTYPE::ERRORS);
		return;
	}

	QFile file(file_name);
	if (!file.open(QIODevice::ReadOnly)) {
		emit SendLogInfoSig("用户登录序列化文件打开失败！", LOGTYPE::ERRORS);
		QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "序列化文件打开失败！");
		return;
	}
	QByteArray whole_file = file.readAll();
	QJsonObject json_document = QJsonDocument::fromJson(whole_file).object();
	QJsonArray user_array = json_document["UserLogin"].toArray();
	
	for (QJsonValue user : user_array) {
		UserLoginParam user_param;
		user_param.userName = user["userName"].toString();
		user_param.password = user["password"].toString();
		user_param.userLevel = USERLEVEL( user["userLevel"].toInt());

		qVUserLoginParam.push_back(user_param);
	}
}


