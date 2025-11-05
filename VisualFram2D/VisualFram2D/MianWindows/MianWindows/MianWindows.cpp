#include "MianWindows.h"
#include <qwidgetaction.h>
#include <QInputDialog>
#include <QLineEdit>
#include <QSettings>
#include <QLabel>
#include <QTimer>
#include <QFileDialog>
#include <QAction>

MianWindows::MianWindows(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    //QFile qss;
    //qss.setFileName(":/MianWindows/Theme/light.qss");
    //qss.open(QFile::ReadOnly);
    //qApp->setStyleSheet(qss.readAll());
    //qss.close();

    this->setWindowTitle("AI行为分析平台2.0");//设置窗口名
    this->setWindowIcon(QIcon(":/MianWindows/Image/Logo.png"));

    ///加载DLL
	PluginsManager::PluginsManagerInstance()->LoadAllPlugins();
    InitialLoadSystemParam();
    InitialMianWindows();
    InitialAllThread();

  //  LoadDockStyle(":/MianWindows/Theme/dock.qss");
}

MianWindows::~MianWindows()
{
    this->DestroyAllObject();
    this->deleteLater();
}

void MianWindows::DestroyAllObject()
{
    ptrAutoRanderVideoStream.reset();
    ptrAutoFlowsLogInfoShowThread.reset();
    ptrAutoCommunicationDeviceWriteThread.reset();
    ptrAutoSignalTriggerFlowsExeThread.reset();
    ptrAutoFlowsExeThread.reset();
    ptrAutoSOPReportShowThread.reset();
}

void MianWindows::InitialMianWindows()
{
    ads::CDockManager::setConfigFlag(ads::CDockManager::OpaqueSplitterResize, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::XmlCompressionEnabled, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::FocusHighlighting, true);
    ads::CDockManager::setAutoHideConfigFlags(ads::CDockManager::DefaultAutoHideConfig);

	InitialCDockManager();
	InitialToolBar();
    InitialDeserializeCompletion();

    this->SetGUIStyle("Settings.ini");

    QString file_name = ptrSystemSetParam->projectPath;
    if (file_name.isEmpty()) {
        return;
    }
    if (QFileInfo::exists(file_name)) {
        DeserializeProject(file_name);
    }
}

void MianWindows::InitialCDockManager()
{
	mDockManager = new ads::CDockManager(this);
    //mDockManager->setDockWidgetToolBarStyle(Qt::ToolButtonIconOnly, ads::CDockWidget::StateFloating);
	FrmLog* ptr_framlog = FrmLog::FrmLogInstance();

	ads::CDockWidget* frmlog_dockwidget = new ads::CDockWidget("日志");
	frmlog_dockwidget->setWidget(ptr_framlog);
	mDockManager->addDockWidget(ads::BottomDockWidgetArea, frmlog_dockwidget);
    frmlog_dockwidget->setIcon(QIcon(":/MianWindows/Image/log.png"));
   // qVsetEnabledAllWidget.append(ptr_framlog);

	framToolsTree = new FramToolsTree();
	ads::CDockWidget* node_tools_sidebar = new ads::CDockWidget("工具集");
	node_tools_sidebar->setWidget(framToolsTree);
	mDockManager->addDockWidget(ads::TopDockWidgetArea, node_tools_sidebar);
    node_tools_sidebar->setIcon(QIcon(":/MianWindows/Image/tools.png"));
    qVsetEnabledAllWidget.append(framToolsTree);

	framTaskFlowViewWidget = new  FramTaskFlowViewWidget();
	ads::CDockWidget* graphics_view = new ads::CDockWidget("流程图");
	graphics_view->setWidget(framTaskFlowViewWidget);
	mDockManager->addDockWidget(ads::TopDockWidgetArea, graphics_view);
    graphics_view->setIcon(QIcon(":/MianWindows/Image/Flow.png"));
    qVsetEnabledAllWidget.append(framTaskFlowViewWidget);

    framImageLayout = new FramImageLayout();
    framImageView = new FramImageView();
    framImageView->SetFramImageLayout(framImageLayout);
    ads::CDockWidget* image_result_view = new ads::CDockWidget("渲染结果");
    image_result_view->setWidget(framImageView);
    mDockManager->addDockWidget(ads::TopDockWidgetArea, image_result_view);
    image_result_view->setIcon(QIcon(":/MianWindows/Image/Show.png"));
    qVsetEnabledAllWidget.append(framImageView);
	
	framTreeWidgetTaskFlowAndProject = new FramTreeWidgetTaskFlowAndProject(framTaskFlowViewWidget);
	ads::CDockWidget* framtree_tflow_project = new ads::CDockWidget("项目流");
	framtree_tflow_project->setWidget(framTreeWidgetTaskFlowAndProject);
	mDockManager->addDockWidget(ads::TopDockWidgetArea, framtree_tflow_project);
    framtree_tflow_project->setIcon(QIcon(":/MianWindows/Image/project.png"));
    qVsetEnabledAllWidget.append(framTreeWidgetTaskFlowAndProject);

    framCameraSetParam = new FramCameraSetParam();
    framCommunicateDevice = new FramCommunicateDevice();

}

void MianWindows::InitialLoadSystemParam()
{
    applicationPath = qApp->applicationDirPath();
    ptrSystemSetParam = StaticGlobalVariable::ptrSystemSetParam;
    framSystemSetUp = new FramSystemSetUp();
    ptrFolderOperation = std::make_shared<FolderOperation>();
    QTimer* p_timer = new QTimer(this);
    p_timer->start(60 * 60 * 1000);  // 一个小时删除一次
    connect(p_timer, &QTimer::timeout, this, [this]() {
        QString log_path = applicationPath + "//log";
        ptrFolderOperation->FindFileForDelete(log_path, ptrSystemSetParam->saveLogTime);
        });
    /// <summary>
    /// 延迟2000ms后，才是开始运行
    /// </summary>
    connect(this, &MianWindows::DeserializeCompletionSignal, this, [this]() {
        if (ptrSystemSetParam->processSelfStart) {
            QTimer::singleShot(2000, [this]() {
                autoRunAction->triggered(true); 
                autoRunAction->setChecked(true);
                });
        }
        });
}

void MianWindows::InitialAllThread()
{
    ptrAutoRanderVideoStream = std::make_shared <AutoGetAndRanderVideoStream>();
    ptrAutoRanderVideoStream->GetFramCameraDialog(framCameraSetParam);
    ptrAutoRanderVideoStream->GetFramImageView(framImageView);

    ptrAutoSignalTriggerFlowsExeThread = std::make_shared<AutoSignalTriggerFlowsExeThread>();
    ptrAutoSignalTriggerFlowsExeThread->SetBaseCommunicateDevice(framCommunicateDevice->GetBaseCommunicateDevice());
    ptrAutoSignalTriggerFlowsExeThread->SetFramTaskFlowViewWidget(framTaskFlowViewWidget);

    ptrAutoFlowsExeThread = std::make_shared<AutoFlowsExeThread>();
    ptrAutoFlowsExeThread->SetFramTaskFlowViewWidget(framTaskFlowViewWidget);

    ptrAutoFlowsLogInfoShowThread = std::make_shared<AutoFlowsLogInfoShowThread>();  
    ptrAutoFlowsLogInfoShowThread->BuildCycleThread();

    ptrAutoCommunicationDeviceWriteThread = std::make_shared<AutoCommunicationDeviceWriteThread>();
    ptrAutoCommunicationDeviceWriteThread->SetBaseCommunicateDevice(framCommunicateDevice->GetBaseCommunicateDevice());
    ptrAutoCommunicationDeviceWriteThread->BuildCycleThread();

    ptrAutoSOPReportShowThread = std::make_shared<AutoSOPReportShowThread>();
    ptrAutoSOPReportShowThread->GetFramImageView(framImageView);
    ptrAutoSOPReportShowThread->BuildCycleThread();
    ptrAutoSOPReportShowThread->StartCycleThread();
}

void MianWindows::InitialDeserializeCompletion()
{
    connect(this, &MianWindows::DeserializeCompletionSignal, this, [this]() {
    //    framRemoteOperationEvents->GetExterHardwareDialog(framCameraSetParam);
      //  framRemoteOperationEvents->GetFramTaskFlowViewWidget(framTaskFlowViewWidget);
   //     framRemoteOperationEvents->DeserializeRemoteOperationEvents(jsonRemoteOperationEvents);
        });
}

void MianWindows::InitialToolBar()
{
	QAction* openproject_action = new QAction(QIcon(":/MianWindows/Image/open.png"), "打开项目", this);
	QAction* saveproject_action = new QAction(QIcon(":/MianWindows/Image/save.png"), "保存项目", this);
	QAction* saveasproject_action = new QAction(QIcon(":/MianWindows/Image/SaveAs.png"), "项目另存为", this);
	QAction* camera_action = new QAction(QIcon(":/MianWindows/Image/Hardware.png"), "相机设备", this);
    QAction* communicate_action = new QAction(QIcon(":/MianWindows/Image/communicate.png"), "通信设备", this);
    QAction* image_layout_action = new QAction(QIcon(":/MianWindows/Image/CanvasSetting.png"), "画布设置", this);
    QAction* trigger_run_action = new QAction(QIcon(":/MianWindows/Image/Auto2.png"), "自动运行", this);
    autoRunAction = trigger_run_action;
    QAction* restore_state_action = new QAction(QIcon(":/MianWindows/Image/Restate.png"), "重设界面", this);
    QAction* runset_action = new QAction(QIcon(":/MianWindows/Image/Setting.png"), "软件设置", this);


    qVsetEnabledAllAction.append(openproject_action);
    qVsetEnabledAllAction.append(saveproject_action);
    qVsetEnabledAllAction.append(communicate_action);
    qVsetEnabledAllAction.append(saveasproject_action);
    qVsetEnabledAllAction.append(image_layout_action);
    qVsetEnabledAllAction.append(runset_action);
    qVsetEnabledAllAction.append(camera_action);
    qVsetEnabledAllAction.append(restore_state_action);
    qVsetEnabledAllAction.append(trigger_run_action);


	ui.mainToolBar->addAction(openproject_action);
	ui.mainToolBar->addSeparator();
	ui.mainToolBar->addAction(saveproject_action);
	ui.mainToolBar->addSeparator();
	ui.mainToolBar->addAction(saveasproject_action);
	ui.mainToolBar->addSeparator();
	ui.mainToolBar->addAction(camera_action);
    ui.mainToolBar->addSeparator();
    ui.mainToolBar->addAction(communicate_action);
	ui.mainToolBar->addSeparator();
   // ui.mainToolBar->addAction(remote_operation_action);
   // ui.mainToolBar->addSeparator();
    ui.mainToolBar->addAction(image_layout_action);
    ui.mainToolBar->addSeparator();
    ui.mainToolBar->addAction(trigger_run_action);
    ui.mainToolBar->addSeparator();
    ui.mainToolBar->addAction(restore_state_action);
    ui.mainToolBar->addSeparator();
    ui.mainToolBar->addAction(runset_action);
	ui.mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

	QObject::connect(openproject_action, &QAction::triggered, [this]() {
		QString fileName = QFileDialog::getOpenFileName(nullptr, tr("打开项目文件"),
            QDir::homePath(),tr("打开格式 (*.pro)"));
		if (!QFileInfo::exists(fileName))
			return false;
	        DeserializeProject(fileName);
		});

	QObject::connect(saveasproject_action, &QAction::triggered, [this]() {
		QString fileName = QFileDialog::getSaveFileName(nullptr,
			tr("项目另存为"),
			QDir::homePath(),
			tr("保存格式 (*.pro)"));
		if (!fileName.isEmpty()) {
			if (!fileName.endsWith("pro", Qt::CaseInsensitive))
				fileName += ".pro";
            SerializeProject(fileName);
		}
		});

	QObject::connect(saveproject_action, &QAction::triggered, [this]() {
        QString file_name = ptrSystemSetParam->projectPath;
		if (!file_name.isEmpty()) {
            SerializeProject(file_name);
		}
		});

	QObject::connect(camera_action, &QAction::triggered, [this]() {
		framCameraSetParam->exec();
		});

    QObject::connect(communicate_action, &QAction::triggered, [this]() {
        framCommunicateDevice->exec();
        });


 /*   QObject::connect(remote_operation_action, &QAction::triggered, [this]() {
        framRemoteOperationEvents->exec();
        });*/

    QObject::connect(restore_state_action, &QAction::triggered, [this](bool checked) {
        this->SetGUIStyle("ReSettings.ini");
        });

    QObject::connect(image_layout_action, &QAction::triggered, [this]() {
        framImageLayout->exec();
        });

    QObject::connect(runset_action, &QAction::triggered, [this](bool checked) {
        framSystemSetUp->exec();
        });

    trigger_run_action->setCheckable(true);
    trigger_run_action->setChecked(false);
    QObject::connect(trigger_run_action, &QAction::triggered, [this, trigger_run_action](bool checked) {
        bool res = checked;
         AutoRunFunction(trigger_run_action, res);
        });
}

void MianWindows::AutoRunFunction(QAction* action, const bool& state)
{
    if (state == true) {
        action->setText("正在自动运行");
        action->setIcon(QIcon(":/MianWindows/Image/Auto1.png"));
        //运行线程
        AutoRunExThread();
        foreach(auto widget, qVsetEnabledAllWidget) {
            widget->setEnabled(false);
        }
        foreach(auto qaction, qVsetEnabledAllAction) {
            if (qaction == autoRunAction) {
                continue;
            }
            qaction->setEnabled(false);
        }
        mDockManager->setEnabled(false); 
    }
    else {
        action->setText("自动运行暂停");
        action->setIcon(QIcon(":/MianWindows/Image/Auto2.png"));
        //暂停线程
        PauseExThread();
        foreach(auto widget, qVsetEnabledAllWidget) {
            widget->setEnabled(true);
        }
        foreach(auto qaction, qVsetEnabledAllAction) {
            qaction->setEnabled(true);
        }
        mDockManager->setEnabled(true);
    }
}

void MianWindows::AutoRunExThread()
{
    //视频渲染和显示线程
    if (ptrAutoRanderVideoStream->IsCreateThread()) {
        ptrAutoRanderVideoStream->StartCycleThread();
    }
    else {
        ptrAutoRanderVideoStream->BuildCycleThread();
    }

    if (ptrSystemSetParam->programTriggerEnable) {
        //触发线程
        if (ptrAutoSignalTriggerFlowsExeThread->IsCreateThread()) {
            ptrAutoSignalTriggerFlowsExeThread->StartCycleThread();
        }
        else {
            ptrAutoSignalTriggerFlowsExeThread->BuildCycleThread();
        }
    }
    else{
        //自动运行
        if (ptrAutoFlowsExeThread->IsCreateThread()) {
            ptrAutoFlowsExeThread->StartCycleThread();
        }
        else {
            ptrAutoFlowsExeThread->BuildCycleThread();
        }
    }  
}

void MianWindows::PauseExThread()
{
    ptrAutoRanderVideoStream->PauseCycleThread();
    ptrAutoSignalTriggerFlowsExeThread->PauseCycleThread();
    ptrAutoFlowsExeThread->PauseCycleThread();
}

void MianWindows::SetGUIStyle(const QString& style)
{
    applicationPath = qApp->applicationDirPath();
    QSettings Settings(applicationPath + "//Settings//" + style, QSettings::IniFormat);
    this->restoreGeometry(Settings.value("mainWindow/Geometry").toByteArray());
    this->restoreState(Settings.value("mainWindow/State").toByteArray());
    mDockManager->restoreState(Settings.value("mainWindow/DockingState").toByteArray());
}

void MianWindows::closeEvent(QCloseEvent* event)
{
	QString applicationPath = qApp->applicationDirPath();
	QSettings Settings(applicationPath + "//Settings//Settings.ini", QSettings::IniFormat);
	Settings.setValue("mainWindow/Geometry", this->saveGeometry());
	Settings.setValue("mainWindow/State", this->saveState());
	Settings.setValue("mainWindow/DockingState", mDockManager->saveState());
	mDockManager->deleteLater();

    QString file_name = ptrSystemSetParam->projectPath;
	if (!file_name.isEmpty()) {
        SerializeProject(file_name);
	}

	QMainWindow::closeEvent(event);
}

void MianWindows::LoadDockStyle(const QString& filePath)
{
    QFile f(filePath);
    if (!f.exists()) {
        qWarning() << "Dock QSS not found:" << filePath;
        return;
    }
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Open dock QSS failed:" << filePath;
        return;
    }
    QString qss = QString::fromUtf8(f.readAll());
    f.close();
    // 只对 DockManager 及其子层级生效
    mDockManager->setStyleSheet(qss);
}

void MianWindows::SerializeProject(const QString& save_path)
{
    QJsonObject project_json;
    QJsonObject json_flow = framTreeWidgetTaskFlowAndProject->SerializeProject();
    QJsonObject json_device = framCameraSetParam->SerializeCameraDevice();
    QJsonObject  json_conmm_device=  framCommunicateDevice->SerializeCommunicateDevice();
   // QJsonObject json_remote = framRemoteOperationEvents->SerializeRemoteOperationEvents();

    project_json["Project"] = json_flow;
    project_json["Device"] = json_device;
    project_json["ConmmDevice"] = json_conmm_device;
 //   project_json["RemoteOperationEvent"] = json_remote;

    QFile file(save_path);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(project_json).toJson());
    }
}

void MianWindows::DeserializeProject(const QString& save_path)
{
    QFile file(save_path);
    if (!file.open(QIODevice::ReadOnly)) {
        FrmLog* ptr_framlog = FrmLog::FrmLogInstance();
        ptr_framlog->LogMassageShowSlot("序列化文件打开失败！", LOGTYPE::ERRORS);
        QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "序列化文件打开失败！");
        return;
    }

    QByteArray whole_file = file.readAll();
    QJsonObject json_document = QJsonDocument::fromJson(whole_file).object();

    QJsonObject json_flow = json_document["Project"].toObject();
    QJsonObject json_device = json_document["Device"].toObject();
    QJsonObject  json_conmm_device = json_document["ConmmDevice"].toObject();

    jsonRemoteOperationEvents = json_document["RemoteOperationEvent"].toObject();

    framTreeWidgetTaskFlowAndProject->DeserializeProject(json_flow);
    framCameraSetParam->DeserializeCameraDevice(json_device);
    framCommunicateDevice->DeserializeCommunicateDevice(json_conmm_device);

    emit DeserializeCompletionSignal();
}

