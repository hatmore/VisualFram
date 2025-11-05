#include "AutoRunOperationThreadPool.h"
#include "../StaticGlobalVariable.h"
#include "../RemoteOperation/FramRemoteOperationEvents.h"

AutoRunOperationThreadPool::AutoRunOperationThreadPool()
{
    Initial();
    FrmLog* ptr_framlog = FrmLog::FrmLogInstance();
    QObject::connect(this, &AutoRunOperationThreadPool::SendLogInfoSynSig, ptr_framlog, &FrmLog::LogMassageShowSlot);
    QObject::connect(this, &AutoRunOperationThreadPool::SendLogInfoAsynSig, ptr_framlog, &FrmLog::LogMassageShowSlot, Qt::QueuedConnection);
}

AutoRunOperationThreadPool::~AutoRunOperationThreadPool()
{
    DestroyCycleThread();
}

void AutoRunOperationThreadPool::Initial()
{
    ptrThreadPools = std::make_shared<ThreadPool>(MaxThreadNum);
    ptrThreadPools->Init();
}

void AutoRunOperationThreadPool::BuildCycleThread()
{
    isThreadStopFlag = true;
    isThreadPauseFlag = true;
    CycleGetMultiCameraData();
    cycleRunCameraDataThread = std::make_unique<std::thread>(&AutoRunOperationThreadPool::CycleRunCameraThread, this);
}

void AutoRunOperationThreadPool::GetFramExterHardwareSettingDialog(QDialog* dialog)
{
    FramExterHardwareSetting* fram_exterhardware_dialog = dynamic_cast<FramExterHardwareSetting*>(dialog);
    framCameraSetParam =  fram_exterhardware_dialog->GetFramCameraSetParam();
    framCommunicateDevice = fram_exterhardware_dialog->GetFramCommunicateDevice();
}

void AutoRunOperationThreadPool::DestroyCycleThread()
{
    isThreadStopFlag = false;
    isThreadPauseFlag = false;
    conditionVariableThread.notify_all();

    ptrThreadPools->Shutdown();

    if (cycleRunCameraDataThread->joinable())
        cycleRunCameraDataThread->join();
    cycleRunCameraDataThread.reset();
    cycleRunCameraDataThread = nullptr;
}

void AutoRunOperationThreadPool::StartCycleThread()
{

}

void AutoRunOperationThreadPool::PauseCycleThread()
{

}

void AutoRunOperationThreadPool::CycleGetMultiCameraData()
{
    QMap<QString, BaseCamera::Ptr>* map_camera = dynamic_cast<FramCameraSetParam*>(framCameraSetParam)->GetMapCameraDevices();
    for (auto iter = map_camera->constBegin(); iter != map_camera->constEnd(); ++iter) {  
        QString key = iter.key();
        BaseCamera::Ptr ptr_camera = iter.value();
        std::future<void>  get_future = ptrThreadPools->Submit([this, key, ptr_camera]() {
            this->CycleGetCameraData(key, ptr_camera);
            }
        );
    }
}

void AutoRunOperationThreadPool::CycleGetCameraData(const QString& key, const BaseCamera::Ptr& ptr_camera)
{
    QString info = "相机获取图像: -- " + key + ": ";
    while (isThreadStopFlag) {
        if (!isThreadPauseFlag) {
            std::unique_lock<std::mutex> thread_locker(mutexCycleRun);
            if (!isThreadPauseFlag) {
                conditionVariableThread.wait(thread_locker);
            }
            thread_locker.unlock();
        }
        if (ptr_camera == nullptr) {
            emit SendLogInfoAsynSig(info + "相机指针为空! " , LOGTYPE::ERRORS);
            return;
        }
        if (!ptr_camera->GetCameraConnectState()) {
            emit SendLogInfoAsynSig(info + "相机断开链接，采图失败!  ", LOGTYPE::ERRORS);
        }
        cv::Mat image;
        ptr_camera->MvGetImage(image);
        DeviceCameraData2D::Ptr ptr_data = std::make_shared<DeviceCameraData2D>();
        ptr_data->cameraData = image.clone();
        StaticGlobalVariable::ptrToolNodeDataInteract->PushExDeviceData(key, ptr_data);
    }
}

void AutoRunOperationThreadPool::CycleMutilCommunicateDeviceReadData()
{
 


}


void AutoRunOperationThreadPool::CycleRunCameraThread()
{
    while (isThreadStopFlag) {
        if (!isThreadPauseFlag) {
            std::unique_lock<std::mutex> thread_locker(mutexCycleRun);
            if (!isThreadPauseFlag) {
                conditionVariableThread.wait(thread_locker);
            }
            thread_locker.unlock();
        }

    }
}
