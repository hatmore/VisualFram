#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MianWindows.h"

#include "Toolnterface/ToolInterface.h"
#include "PlugiManager/PluginsManager.h"
#include "Log/frmLog.h"

#include "DockManager.h"
#include "DockAreaWidget.h"
#include "DockWidget.h"

#include "GeneralStruct.h"

#include "DragTreeWidget/FramToolsTree.h"
#include "TreeTaskFlowAndProject/FramTreeWidgetTaskFlowAndProject.h"
#include "GraphicsViewWidget/FramTaskFlowViewWidget.h"
#include "CameraDevice/FramCameraSetParam.h"
#include "CommunicateDevice/FramCommunicateDevice.h"
//#include "RemoteOperation/FramRemoteOperationEvents.h"
#include "ImageView/FramImageLayout.h"
#include "ImageView/FramImageView.h"
#include "StaticGlobalVariable.h"

#include "Other/FolderOperation.h"
#include "SystemParamSetUp/FramSystemSetUp.h"

#include "AutoRunOperation/AutoGetAndRanderVideoStream.h"
#include "AutoRunOperation/AutoSignalTriggerFlowsExeThread.h"
#include "AutoRunOperation/AutoFlowsLogInfoShowThread.h"
#include "AutoRunOperation/AutoCommunicationDeviceWriteThread.h"
#include "AutoRunOperation/AutoFlowsExeThread.h"
#include "AutoRunOperation/AutoSOPReportShowThread.h"



class MianWindows : public QMainWindow
{
    Q_OBJECT

public:
    MianWindows(QWidget *parent = nullptr);
    ~MianWindows();

private:
    void InitialMianWindows();
    /// <summary>
    /// Dock相关初始化
    /// </summary>
    void InitialCDockManager();
    /// <summary>
    /// toolBar初始化
    /// </summary>
    void InitialToolBar();

    /// <summary>
    /// 序列化，完成信号，初始化
    /// </summary>
    void InitialDeserializeCompletion();
    
    /// <summary>
    ///初始化参数设置 
    /// </summary>
    void InitialLoadSystemParam();

    /// <summary>
    /// 序列化流程
    /// </summary>
    void SerializeProject(const QString& save_path);
    /// <summary>
    /// 反序列化流程
    /// </summary>
    void DeserializeProject(const QString& save_path);

    /// <summary>
    /// 销毁对象，主要针对线程
    /// </summary>
    void DestroyAllObject();

private:
    /// <summary>
    /// 初始化线程相关的参数
    /// </summary>
    void InitialAllThread();
    /// <summary>
    /// 自动运行的一些函数参数
    /// </summary>
    void AutoRunFunction(QAction* action, const bool& state);

    /// <summary>
    /// 线程自动运行，开启线程不同，根据设置
    /// 可分为：外部信号触发，流程执行；和自动运行两种
    /// </summary>
    void AutoRunExThread();

    /// <summary>
    /// 暂停自动运行的线程，暂停处理视频运行的线程 
    /// </summary>
    void PauseExThread();

    /// <summary>
    /// 界面重新设定
    /// </summary>
    void SetGUIStyle(const QString& style);

    /// <summary>
    /// 加载DOCK的qss
    /// </summary>
    /// <param name="filePath"></param>
    void LoadDockStyle(const QString& filePath);


protected:
    virtual void closeEvent(QCloseEvent* event) override;

signals:
    void DeserializeCompletionSignal();

private:
    /// <summary>
    /// 运行路径
    /// </summary>
    QString applicationPath;

private:
    /// <summary>
    /// 视频显示和渲染
    /// </summary>
    AutoGetAndRanderVideoStream::Ptr ptrAutoRanderVideoStream{nullptr};    
    
    /// <summary>
    /// 自动日志相关的线程
    /// </summary>
    AutoFlowsLogInfoShowThread::Ptr ptrAutoFlowsLogInfoShowThread{ nullptr };

    /// <summary>
    /// 外部设备通信线程，通信接收相关线程
    /// </summary>
    AutoCommunicationDeviceWriteThread::Ptr ptrAutoCommunicationDeviceWriteThread{ nullptr };

    /// <summary>
    ///  外部触发线程运行，接收外部数据线程
    /// </summary>
    AutoSignalTriggerFlowsExeThread::Ptr ptrAutoSignalTriggerFlowsExeThread{ nullptr };

    /// <summary>
    /// 线程自动运行，开启就运行
    /// </summary>
    AutoFlowsExeThread::Ptr ptrAutoFlowsExeThread{nullptr};

    /// <summary>
    /// 自动显示SOP报告线程
    /// </summary>
    AutoSOPReportShowThread::Ptr ptrAutoSOPReportShowThread{ nullptr };

private:
    Ui::MianWindowsClass ui;

    ads::CDockManager* mDockManager;
    FramToolsTree* framToolsTree{ nullptr };
    FramTreeWidgetTaskFlowAndProject* framTreeWidgetTaskFlowAndProject{ nullptr };
    GraphicsViewTaskFlowWidget* graphicsViewTaskFlowWidget{ nullptr };
    FramTaskFlowViewWidget* framTaskFlowViewWidget{ nullptr };

    FramCameraSetParam* framCameraSetParam{nullptr};

    /// <summary>
    /// 通信设备
    /// </summary>
    FramCommunicateDevice* framCommunicateDevice{ nullptr };
    /// <summary>
    /// 设置界面
    /// </summary>
    FramSystemSetUp* framSystemSetUp{ nullptr };

    /// <summary>
    /// 显示部分
    /// </summary>
    FramImageLayout* framImageLayout{nullptr};
    FramImageView* framImageView{nullptr};

 private:
    QAction* autoRunAction{ nullptr }; //自动运动Action
    QVector<QWidget*> qVsetEnabledAllWidget; //用于设置所有控件状态
    QVector<QAction*> qVsetEnabledAllAction; //用于设置所有控件状态

private:
    PtrSystemSetParam ptrSystemSetParam{ nullptr }; // 系统参数
    PtrFolderOperation ptrFolderOperation{ nullptr };  //智能指针操作文件夹

    QJsonObject jsonRemoteOperationEvents;
};

