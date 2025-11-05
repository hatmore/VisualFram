#include "AutoSignalTriggerFlowsExeThread.h"
#include "../StaticGlobalVariable.h"
#include "../Log/frmLog.h"
#include "../GraphicsViewWidget/FramTaskFlowViewWidget.h"
#include <QDateTime>
#include <QDate>
#include <algorithm>

#define MaxThreadNum 1024

namespace {
    // 简单的限频日志辅助（可扩展为按 key）
    class RateLimiter {
    public:
        bool allow(std::chrono::milliseconds interval) {
            auto now = std::chrono::steady_clock::now();
            if (now - last_ >= interval) {
                last_ = now;
                return true;
            }
            return false;
        }
    private:
        std::chrono::steady_clock::time_point last_{};
    };
}

AutoSignalTriggerFlowsExeThread::AutoSignalTriggerFlowsExeThread()
{
    Initial();
    FrmLog* ptr_framlog = FrmLog::FrmLogInstance();
    QObject::connect(this, &AutoSignalTriggerFlowsExeThread::SendLogInfoSynSig, ptr_framlog, &FrmLog::LogMassageShowSlot);
    QObject::connect(this, &AutoSignalTriggerFlowsExeThread::SendLogInfoAsynSig, ptr_framlog, &FrmLog::LogMassageShowSlot, Qt::BlockingQueuedConnection);
}

AutoSignalTriggerFlowsExeThread::~AutoSignalTriggerFlowsExeThread()
{
    this->DestroyCycleThread();
}

void  AutoSignalTriggerFlowsExeThread::Initial()
{
    std::string path = "D:\\ProgramData\\Anaconda3\\envs\\Python310";
    ptrCommunicateDevicePythonEngine = new CommunicateDevicePythonEngine();
    ptrCommunicateDevicePythonEngine->InitailPython(path);
    ptrThreadPools = std::make_shared<ThreadPool>(MaxThreadNum);
    ptrThreadPools->Init();
    uptrQueueCommunicateOrder = std::make_unique<tbb::concurrent_queue<std::unordered_map<std::string, int>>>();
}

void AutoSignalTriggerFlowsExeThread::SetFramTaskFlowViewWidget(QWidget* qwidget)
{
    FramTaskFlowViewWidget* task_flows = (FramTaskFlowViewWidget*)qwidget;
    connect(this, &AutoSignalTriggerFlowsExeThread::SendCommunicateOrderSig, this, [this, task_flows](QMap<QString, int>& map_order) {
        for (auto itor = map_order.begin(); itor != map_order.end(); ++itor)   {
            QString key = itor.key();
            int order = itor.value();
            emit SendLogInfoSynSig( key + " 流程接收到命令：" + QString::number(order), LOGTYPE::INFO);
      
            if (order < -1) {
                emit SendLogInfoSynSig(QString::number(order) + " < -1, 命令错误， 不符合标准！", LOGTYPE::ERRORS);
                continue;
            }

            auto iter = StaticGlobalVariable::ptrToolNodeDataInteract->ptrMapQueueDeviceCommunicationData->find(key);
            if (iter != StaticGlobalVariable::ptrToolNodeDataInteract->ptrMapQueueDeviceCommunicationData->end()) {          
                std::shared_ptr < tbb::concurrent_queue<int> > ptr_queue_device_communication_data = iter->second;
                if (ptr_queue_device_communication_data != nullptr) {
                    ptr_queue_device_communication_data->push(order);
                    continue;
                }
            }
            else{
                GraphicsViewTaskFlowWidget* task_flow = task_flows->DelegateGraphicsTaskFlow(key);
                if (task_flow != nullptr) {
                    TaskFlowProcess(task_flow, order);
                    continue;
                }
            }
               emit SendLogInfoSynSig(key + "流程或触发信号不存在 ", LOGTYPE::ERRORS);
        }        
        }, Qt::BlockingQueuedConnection);
}

void AutoSignalTriggerFlowsExeThread::ClearAllDeviceQueues()
{
    if (!StaticGlobalVariable::ptrToolNodeDataInteract) return;
    auto ptr_map_queue_devicedata = StaticGlobalVariable::ptrToolNodeDataInteract->ptrMapQueuePtrExDeviceData;
    if (!ptr_map_queue_devicedata) return;
    for (auto& kv : *ptr_map_queue_devicedata) {
        if (kv.second) kv.second->clear();
    }
}

void AutoSignalTriggerFlowsExeThread::TaskFlowProcess(GraphicsViewTaskFlowWidget* task_flow, const int order)
{
    auto is_cycle_run = task_flow->GetTaskFlowRunState();
    QString flow_name = task_flow->GetTaskFlowName();
    switch (order){
    case -1: {
        if (!is_cycle_run) {
            emit SendLogInfoSynSig(flow_name + "处于暂停状态，请检查后流程状态 ", LOGTYPE::ERRORS);
        }
        else{
            task_flow->TaskFlowStopExecutor();
            emit SendLogInfoSynSig(flow_name + "流程，接收暂停命令!  ", LOGTYPE::INFO);
        }
    }
           break;
    case 0: {
        if (is_cycle_run) {
            emit SendLogInfoSynSig(flow_name + "处于运行状态，请检查后流程状态 ", LOGTYPE::ERRORS);
        
            auto ptr_map_queue_devicedata = StaticGlobalVariable::ptrToolNodeDataInteract->ptrMapQueuePtrExDeviceData;
            for (auto map_ : *ptr_map_queue_devicedata) {
                map_.second->clear();
            }
            ptrThreadPools->Submit([this, task_flow]() {
                task_flow->TaskFlowCycleExecutor();
                emit SendLogInfoAsynSig(task_flow->GetTaskFlowName() + "流程，运行结束! ", LOGTYPE::INFO);
                });
        }
    }
          break;
    default: {
        if (is_cycle_run) {
            emit SendLogInfoSynSig(flow_name + "处于运行状态，请检查后流程状态 ", LOGTYPE::ERRORS);
        }
        else{
            auto ptr_map_queue_devicedata = StaticGlobalVariable::ptrToolNodeDataInteract->ptrMapQueuePtrExDeviceData;
            for (auto map_ : *ptr_map_queue_devicedata) {
                map_.second->clear();
            }
            ptrThreadPools->Submit([this, task_flow, order]() {
                task_flow->TaskFlowNCycleExecutor(order);
                emit SendLogInfoAsynSig(task_flow->GetTaskFlowName() + "流程，运行结束! ", LOGTYPE::INFO);
                });
        }
    }
        break;
    }
}

/// <summary>
/// 创建，现有线程
/// </summary>
bool AutoSignalTriggerFlowsExeThread::BuildCycleThread()
{
    if (cycleRunReviceDataThread != nullptr) {
        emit SendLogInfoSynSig(" 外部信号触发流执行线程已经存在，线程创建失败 ", LOGTYPE::ERRORS);
        return false; // 线程已存在
    }
    QString application_path = qApp->applicationDirPath();
    QString python_path = application_path + "\\Python\\CommunicateDevice.py";
    if (!QFile::exists(python_path)) { 
        emit SendLogInfoSynSig("信号解析文件不存在: " + python_path + " -- 请检查该路径文件", LOGTYPE::ERRORS);
        return false;
    }

    isThreadStopFlag = true;
    isThreadPauseFlag = true;
    ptrCommunicateDevicePythonEngine->PythonCodeLoad(python_path.toStdString());
    cycleRunReviceDataThread = std::make_unique<std::thread>(&AutoSignalTriggerFlowsExeThread::CycleGetReviceDataThread, this);
    emit SendLogInfoSynSig(" 外部信号触发流执行线程创建成功 ", LOGTYPE::INFO);

    return true;
}

/// <summary>
/// 销毁，现有线程
/// </summary>
bool AutoSignalTriggerFlowsExeThread::DestroyCycleThread()
{
    isThreadStopFlag = false;
    isThreadPauseFlag = false;
    conditionVariableThread.notify_all();
    ptrThreadPools->Shutdown();
    
    if (cycleRunReviceDataThread != nullptr) {
        if (cycleRunReviceDataThread->joinable())
            cycleRunReviceDataThread->join();
        cycleRunReviceDataThread.reset();
        cycleRunReviceDataThread = nullptr;
    }

    emit SendLogInfoSynSig(" 外部信号触发流执行线程销毁成功 ", LOGTYPE::INFO);
    return true;
}

/// <summary>
/// 开始解析外部事件线程
/// </summary>
void AutoSignalTriggerFlowsExeThread::StartCycleThread()
{
    isThreadStopFlag = true;
    isThreadPauseFlag = true;
    conditionVariableThread.notify_all();
    emit SendLogInfoSynSig(" 开始外部信号触发流执行线程 ", LOGTYPE::INFO);
}

/// <summary>
/// 暂停解析外部事件线程
/// </summary>
void AutoSignalTriggerFlowsExeThread::PauseCycleThread()
{
    isThreadPauseFlag = false;
    emit SendLogInfoSynSig(" 暂停外部信号触发流执行线程 ", LOGTYPE::INFO);
}

void AutoSignalTriggerFlowsExeThread::CycleGetReviceDataThread()
{
    if (!ptrCommunicateDevicePythonEngine) {
        emit SendLogInfoAsynSig(" 线程启动失败：Python 引擎为空 ", LOGTYPE::ERRORS);
        return;
    }
    ptrCommunicateDevicePythonEngine->GetSearchType(searchType);
    RateLimiter empty_limiter;
    int idle_sleep_ms = 2;

    while (isThreadStopFlag) {
        if (!isThreadPauseFlag) {
            std::unique_lock<std::mutex> thread_locker(mutexCycleRun);
            if (!isThreadPauseFlag) {
                conditionVariableThread.wait(thread_locker);
            }
            thread_locker.unlock();
            if (!isThreadStopFlag) break;
        }
        if (!ptrCommDeviceInterface) {
            if (empty_limiter.allow(std::chrono::milliseconds(500))) {
                emit SendLogInfoAsynSig(" 通信接口未就绪 ", LOGTYPE::WARN);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        QByteArray byte_array;
        int read_res = ptrCommDeviceInterface->Read(byte_array);
        if (read_res == 0 && !byte_array.isEmpty()) {
            idle_sleep_ms = 2; // reset backoff
            std::string std_str_data(byte_array.constData(), byte_array.length());
            ptrCommunicateDevicePythonEngine->GetReviceData(std_str_data);
            std::unordered_map<std::string, int> map_flow_order = ptrCommunicateDevicePythonEngine->GetFlowsExeOrder();

            if (map_flow_order.empty()) {
                if (empty_limiter.allow(std::chrono::milliseconds(100))) {
                    emit SendLogInfoAsynSig(" 接收触发命令为空，不符合规范! ", LOGTYPE::ERRORS);
                }
                continue;
            }

            QMap<QString, int> map_order;
            for (std::unordered_map<std::string, int>::const_iterator it = map_flow_order.begin(); it != map_flow_order.end(); it++) {
                QString key = QString::fromStdString(it->first);
                int order = it->second;
                map_order.insert(key, order);
            }
            emit SendCommunicateOrderSig(map_order);
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(idle_sleep_ms));
            idle_sleep_ms = (std::min)(idle_sleep_ms * 2, 100); // 指数退避至 100ms
        }
    }
}
