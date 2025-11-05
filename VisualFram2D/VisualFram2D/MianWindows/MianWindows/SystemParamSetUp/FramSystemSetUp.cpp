#pragma execution_character_set("utf-8")
#include "FramSystemSetUp.h"
#include <QDesktopWidget>
#include <QFileDialog>
#include <QJsonDocument>
#include <QComboBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QFile>
#include "../Log/frmLog.h"
#include "../StaticGlobalVariable.h"

FramSystemSetUp::FramSystemSetUp(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ptrSystemSetParam = StaticGlobalVariable::ptrSystemSetParam;
	FrmLog* ptr_framlog = FrmLog::FrmLogInstance();
	QObject::connect(this, &FramSystemSetUp::SendLogInfoSig, ptr_framlog, &FrmLog::LogMassageShowSlot);
	Initial();
}

FramSystemSetUp::~FramSystemSetUp()
{}

void FramSystemSetUp::Initial()
{
	applicationPath = qApp->applicationDirPath();
	InitialSetUI();
	InitialSpinBox();
	InitialExserializeUi();

	QString file_name = applicationPath + "//SystemSet//SystemSet.info";
	if (!QFileInfo::exists(file_name)){
		emit SendLogInfoSig(file_name + "文件路径不存在, 加载系统设置数据失败!", LOGTYPE::ERRORS);
	    return;
	}

	QFile file(file_name);
	if (!file.open(QIODevice::ReadOnly)) {
		emit SendLogInfoSig(file_name + "文件打开失败, 加载系统设置数据失败!", LOGTYPE::ERRORS);
		return;
	}
	QByteArray whole_file = file.readAll();
	QJsonObject json_document = QJsonDocument::fromJson(whole_file).object();
	DeserializeProject(json_document["SystemSet"].toObject());
}

void FramSystemSetUp::InitialSetUI()
{
	this->setWindowTitle("软件设置");
	this->setWindowIcon(QIcon(":/MianWindows/image/Set.png"));
	// 去掉问号，只保留关闭
	this->setWindowFlags(Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint);
	this->installEventFilter(this);
	QDesktopWidget* desktop = QApplication::desktop();
	move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);
}

void FramSystemSetUp::InitialSpinBox()
{
	connect(ui.spinSignOutTime, QOverload<int>::of(&QSpinBox::valueChanged), [this](double value) {
		ptrSystemSetParam->signOutTime = value;
	});

	connect(ui.spinLogTime, QOverload<int>::of(&QSpinBox::valueChanged), [this](double value) {
		ptrSystemSetParam->saveLogTime = value;
	});

	ui.checkProgramSelfStart->setChecked(false);
	connect(ui.checkProgramSelfStart, &QCheckBox::stateChanged, this, [this](bool state) {
		ptrSystemSetParam->programSelfStart = state;
		QString app_path = qApp->applicationFilePath();
		if (state) {
			SetProcessAutoRunSelf(app_path);
		}
		else	{
			RemoveProcessAutoRunSelf(app_path);
		}
	});

	ui.checkProcessSelfStart->setChecked(false);
	connect(ui.checkProcessSelfStart, &QCheckBox::stateChanged, this, [this](bool state) {
		ptrSystemSetParam->processSelfStart = state;
	});

	ui.checkForceProgramSelfStart->setChecked(false);
	connect(ui.checkForceProgramSelfStart, &QCheckBox::stateChanged, this, [this](bool state) {
		ptrSystemSetParam->forceProgramSelfStart = state;
	});

    ui.checkBoxProgramTriggerEnable->setChecked(false);
    connect(ui.checkBoxProgramTriggerEnable, &QCheckBox::stateChanged, this, [this](bool state) {
        ptrSystemSetParam->programTriggerEnable = state;
        });

	connect(ui.pushButton, &QPushButton::clicked, this, [this]() {
		QString path = QDir::currentPath();
		QString project_path = QFileDialog::getOpenFileName(this, tr("打开项目文件"), path, "*pro");
		if (false == project_path.isEmpty()) {
			ptrSystemSetParam->projectPath = project_path;
			ui.lineEditLoadProjectPath->setText(ptrSystemSetParam->projectPath);
		}
	});  
}

void FramSystemSetUp::InitialExserializeUi()
{
	connect(this, &FramSystemSetUp::SendExserializeFinishSig, this, [this]() {
		ui.checkProgramSelfStart->setChecked(ptrSystemSetParam->programSelfStart);
        ui.checkBoxProgramTriggerEnable->setChecked(ptrSystemSetParam->programTriggerEnable);
		ui.checkProcessSelfStart->setChecked(ptrSystemSetParam->processSelfStart);
		ui.checkForceProgramSelfStart->setChecked(ptrSystemSetParam->forceProgramSelfStart);
		ui.spinSignOutTime->setValue(ptrSystemSetParam->signOutTime);
		ui.spinLogTime->setValue(ptrSystemSetParam->saveLogTime);
		ui.lineEditLoadProjectPath->setText(ptrSystemSetParam->projectPath);
	});
}

QJsonObject FramSystemSetUp::SerializeProject()
{
	QJsonObject system_json;
	system_json["programSelfStart"] = ptrSystemSetParam->programSelfStart;
    system_json["programTriggerEnable"] = ptrSystemSetParam->programTriggerEnable;
	system_json["processSelfStart"] = ptrSystemSetParam->processSelfStart;
	system_json["forceProgramSelfStart"] = ptrSystemSetParam->forceProgramSelfStart;
	system_json["signOutTime"] = ptrSystemSetParam->signOutTime;
	system_json["saveLogTime"] = ptrSystemSetParam->saveLogTime;
	system_json["projectPath"] = ptrSystemSetParam->projectPath;
    system_json["imageShowMode"] = ptrSystemSetParam->imageShowMode;
	return system_json;
}

void FramSystemSetUp::DeserializeProject(const QJsonObject& json_project)
{
	ptrSystemSetParam->programSelfStart = json_project["programSelfStart"].toBool();
	ptrSystemSetParam->programTriggerEnable = json_project["programTriggerEnable"].toBool();
    ptrSystemSetParam->processSelfStart = json_project["processSelfStart"].toBool();
	ptrSystemSetParam->forceProgramSelfStart = json_project["forceProgramSelfStart"].toBool();
    ptrSystemSetParam->signOutTime = json_project["signOutTime"].toInt();
	ptrSystemSetParam->saveLogTime = json_project["saveLogTime"].toInt();
	ptrSystemSetParam->projectPath = json_project["projectPath"].toString();
    ptrSystemSetParam->imageShowMode = json_project["imageShowMode"].toInt();
	emit SendExserializeFinishSig();
}

void FramSystemSetUp::closeEvent(QCloseEvent* event)
{
	QString file_name = applicationPath + "//SystemSet//SystemSet.info";
	if (file_name.isEmpty()) {
		emit SendLogInfoSig(file_name + "文件路径不存在, 保存系统设置数据失败!" , LOGTYPE::ERRORS );
		return;
	}
	QFile file(file_name);
	if (file.open(QIODevice::WriteOnly)) {
        QJsonObject project_json;
		QJsonObject json = SerializeProject();
		project_json["SystemSet"] = json;
		file.write(QJsonDocument(project_json).toJson());
	}
	else{
		emit SendLogInfoSig(file_name + "文件打开失败, 保存系统设置数据失败!", LOGTYPE::ERRORS);
	}
}

void FramSystemSetUp::SetProcessAutoRunSelf(const QString& app_path)
{
	//注册表路径需要使用双反斜杠，如果是32位系统，要使用QSettings::Registry32Format
	QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::Registry64Format);
	//以程序名称作为注册表中的键，根据键获取对应的值(程序路径)
	QFileInfo fInfo(app_path);
	QString name = fInfo.baseName();
	QString path = settings.value(name).toString();
	QString newPath = QDir::toNativeSeparators(app_path);
	if (path != newPath){
		settings.setValue(name, newPath);
	}
}

void FramSystemSetUp::RemoveProcessAutoRunSelf(const QString& app_path)
{
	//注册表路径需要使用双反斜杠，如果是32位系统，要使用QSettings::Registry32Format
	QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::Registry64Format);
	//以程序名称作为注册表中的键，根据键获取对应的值(程序路径)
	QFileInfo fInfo(app_path);
	QString name = fInfo.baseName();
	settings.remove(name);
}