#include "AutoFlowsExeThread.h"
#include "../StaticGlobalVariable.h"
#include "../Log/frmLog.h"
#include "../GraphicsViewWidget/FramTaskFlowViewWidget.h"
#include <QDateTime>
#include <QDate>
#define MaxThreadNum 1024

AutoFlowsExeThread::AutoFlowsExeThread()
{
    Initial();
    FrmLog* ptr_framlog = FrmLog::FrmLogInstance();
    QObject::connect(this, &AutoFlowsExeThread::SendLogInfoSynSig, ptr_framlog, &FrmLog::LogMassageShowSlot);
    QObject::connect(this, &AutoFlowsExeThread::SendLogInfoAsynSig, ptr_framlog, &FrmLog::LogMassageShowSlot, Qt::BlockingQueuedConnection);
}

AutoFlowsExeThread::~AutoFlowsExeThread()
{
    this->DestroyCycleThread();
}

void AutoFlowsExeThread::Initial()
{
    ptrThreadPools = std::make_shared<ThreadPool>(MaxThreadNum);
    ptrThreadPools->Init();
}

void AutoFlowsExeThread::SetFramTaskFlowViewWidget(QWidget* qwidget)
{
    FramTaskFlowViewWidget* task_flows_view_widget = (FramTaskFlowViewWidget*)qwidget;
    ptrMapTaskFlowsWidget = task_flows_view_widget->GetMapGraphicsViewTaskFlows();
}


bool AutoFlowsExeThread::BuildCycleThread()
{
    if(isThreadPauseFlag == true) {
        emit SendLogInfoSynSig(" 自动运行流线程已存在，请检查后开启 ", LOGTYPE::ERRORS);
        return false;
    }

    if (ptrMapTaskFlowsWidget == nullptr) {
        emit SendLogInfoSynSig(" 自动运行流的指针为空，请检测后重启流程 ", LOGTYPE::ERRORS);
        return false;
    }

    isThreadStopFlag = true;
    isThreadPauseFlag = true;

    for (auto iter = ptrMapTaskFlowsWidget->begin(); iter != ptrMapTaskFlowsWidget->end(); iter++) {
        GraphicsViewTaskFlowWidget* task_flow = iter->second;
        if (task_flow == nullptr) {
            emit SendLogInfoSynSig("流程指针为空，请检查原因!!", LOGTYPE::ERRORS);
            continue;
        }

        auto is_cycle_run = task_flow->GetTaskFlowRunState();
        QString flow_name = task_flow->GetTaskFlowName();
        if (is_cycle_run) {
            emit SendLogInfoSynSig(flow_name + "处于运行状态，请检查后流程状态 ", LOGTYPE::ERRORS);
        }
        else {
            emit SendLogInfoSynSig(flow_name + " 正在被打开运行。", LOGTYPE::INFO);
            ptrThreadPools->Submit([this, task_flow]() {
                task_flow->TaskFlowCycleExecutor();
                emit SendLogInfoAsynSig(task_flow->GetTaskFlowName() + "流程，运行结束! ", LOGTYPE::INFO);
                });
        }
    }
    return true;
}

bool AutoFlowsExeThread::DestroyCycleThread()
{
  /*  for (auto iter = ptrMapTaskFlowsWidget->begin(); iter != ptrMapTaskFlowsWidget->end(); iter++) {
        GraphicsViewTaskFlowWidget* task_flow = iter->second;
        if (task_flow == nullptr) {
            emit SendLogInfoSynSig("自动运行流程暂停流程时，流程指针为空，请检查原因!!", LOGTYPE::ERRORS);
            continue;
        }
        task_flow->TaskFlowStopExecutor();
    }*/

    ptrThreadPools->Shutdown();
    isThreadStopFlag = false;
    isThreadPauseFlag = false;
    emit SendLogInfoSynSig(" 自动运行流线程销毁成功 ", LOGTYPE::INFO);
    return true;
}

void AutoFlowsExeThread::StartCycleThread()
{
    this->BuildCycleThread();
}

void AutoFlowsExeThread::PauseCycleThread()
{
    for (auto iter = ptrMapTaskFlowsWidget->begin(); iter != ptrMapTaskFlowsWidget->end(); iter++) {
        GraphicsViewTaskFlowWidget* task_flow = iter->second;
        if (task_flow == nullptr) {
            emit SendLogInfoSynSig("自动运行流程暂停流程时，流程指针为空，请检查原因!!", LOGTYPE::ERRORS);
            continue;
        }
        task_flow->TaskFlowStopExecutor();
    }
  //  ptrThreadPools->Shutdown();
    emit SendLogInfoSynSig(" 自动运行流线程暂停成功 ", LOGTYPE::INFO);
    isThreadPauseFlag = false;
}

