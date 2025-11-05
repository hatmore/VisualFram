#pragma once
#include <QDialog>
#include <Qvector>
#include <QCloseEvent>
#include "../SystemParamStruct.h"
#include "../GeneralStruct.h"
#include "ui_FramSystemSetUp.h"

class FramSystemSetUp : public QDialog
{
	Q_OBJECT

public:
	FramSystemSetUp(QWidget *parent = nullptr);
	~FramSystemSetUp();

	/// <summary>
	/// 序列化
	/// </summary>
	/// <returns></returns>
	QJsonObject SerializeProject();
	/// <summary>
	/// 反序列化
	/// </summary>
	/// <param name="json_project"></param>
	void  DeserializeProject(const QJsonObject& json_project);

private:
	void Initial();
	void InitialSetUI();
	void InitialExserializeUi();
	void InitialSpinBox();

private:
	//设置自启动
	void SetProcessAutoRunSelf(const QString &app_path);
	//关闭自启动
	void RemoveProcessAutoRunSelf(const QString& app_path);


signals:
	void SendLogInfoSig(const QString& info, const LOGTYPE type);
	/// <summary>
	/// 序列化完成标志
	/// </summary>
	void SendExserializeFinishSig();

protected:
	virtual void closeEvent(QCloseEvent* event) override;

private:
	Ui::FramSystemSetUpClass ui;
	PtrSystemSetParam ptrSystemSetParam{nullptr};
	QString applicationPath;
};

