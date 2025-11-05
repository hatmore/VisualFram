#pragma once
#pragma execution_character_set("utf-8")
#include <QDialog>
#include <Qvector>
#include "../GeneralStruct.h"
#include "ui_FramUserLogin.h"

class FramUserLogin : public QDialog
{
	Q_OBJECT

public:
	FramUserLogin(QWidget *parent = nullptr);
	~FramUserLogin();
	/// <summary>
	/// 用户权限等级
	/// </summary>
	enum USERLEVEL {
		generalLevel = 0,
		highLevel = 1,
		highestLevel = 2,
	};

	struct UserLoginParam {
		QString userName;
		QString password;
		USERLEVEL userLevel{ USERLEVEL::generalLevel };
	};

private:
	void Initial();
	void InitialSetUI();
	void InitailPushButton();
	void DeserializeUserLoginParam();


signals:
	void SendLogInfoSig(const QString& info, const LOGTYPE type);

private:
	Ui::FramUserLoginClass ui;
	QVector<UserLoginParam>qVUserLoginParam;
};