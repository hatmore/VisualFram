#include "AutoSOPReportShowThread.h"
#include "../StaticGlobalVariable.h"
#include "../Log/frmLog.h"
#include "../ImageView/FramImageView.h"
#include <QDateTime>
#include <QDate>

AutoSOPReportShowThread::AutoSOPReportShowThread()
{
    isThreadStopFlag = false; // 线程停止标志，默认不停止
    isThreadPauseFlag = false; // 线程暂停标志，默认暂停
    cycleRunSOPReportShowThread = nullptr; // 初始化线程指针为空

    FrmLog* ptr_framlog = FrmLog::FrmLogInstance();
    QObject::connect(this, &AutoSOPReportShowThread::SendLogInfoSynSig, ptr_framlog, &FrmLog::LogMassageShowSlot);
    QObject::connect(this, &AutoSOPReportShowThread::SendLogInfoAsynSig, ptr_framlog, &FrmLog::LogMassageShowSlot,Qt::BlockingQueuedConnection);
}

AutoSOPReportShowThread::~AutoSOPReportShowThread()
{
    this->DestroyCycleThread(); // 确保在析构时销毁线程
}


void AutoSOPReportShowThread::GetFramImageView(QWidget* dialog)
{
    FramImageView* fram_dialog = dynamic_cast<FramImageView*>(dialog);
    if (fram_dialog) {
        QObject::connect(this, &AutoSOPReportShowThread::SendRenderImageShowSig,
            fram_dialog, &FramImageView::DispImage,
            Qt::BlockingQueuedConnection);
    }
}

bool AutoSOPReportShowThread::BuildCycleThread()
{
    if (cycleRunSOPReportShowThread != nullptr) {
        emit SendLogInfoSynSig(" SOPReport显示线程创建失败 ", LOGTYPE::ERRORS);
        return false; // 线程已存在
    }

    isThreadStopFlag = true;
    isThreadPauseFlag = true;
    cycleRunSOPReportShowThread = std::make_unique<std::thread>(&AutoSOPReportShowThread::CycleRunSOPReportShowThread, this);
    emit SendLogInfoSynSig(" SOPReport显示线程创建成功 ", LOGTYPE::INFO);
    return true;
}


bool AutoSOPReportShowThread::DestroyCycleThread()
{
    isThreadStopFlag = false;
    isThreadPauseFlag = false;
    conditionVariableThread.notify_all();

    if (cycleRunSOPReportShowThread != nullptr) {
        if (cycleRunSOPReportShowThread->joinable()) {
            cycleRunSOPReportShowThread->join();
        }
        cycleRunSOPReportShowThread.reset();
        cycleRunSOPReportShowThread = nullptr;
    }
    emit SendLogInfoSynSig("PReport显示线程销毁成功 ", LOGTYPE::INFO);
    return true; // 销毁成功
}

void AutoSOPReportShowThread::StartCycleThread()
{
    if (!cycleRunSOPReportShowThread) {
        BuildCycleThread();
        return;
    }
    isThreadStopFlag = true;
    isThreadPauseFlag = true;
    conditionVariableThread.notify_all();
    emit SendLogInfoSynSig(" SOPReport显示线程重新开始 ", LOGTYPE::INFO);
}

void AutoSOPReportShowThread::PauseCycleThread()
{
    isThreadPauseFlag = false;
    emit SendLogInfoSynSig(" SOPReport显示线程暂停 ", LOGTYPE::INFO);
}

void AutoSOPReportShowThread::CycleRunSOPReportShowThread()
{
    // 指数退避用，避免数据暂时没有时固定长睡眠造成响应慢
    int empty_sleepms = 2;
    auto ptr_queue_sop_report = StaticGlobalVariable::ptrToolNodeDataInteract? StaticGlobalVariable::
                                          ptrToolNodeDataInteract->ptrQueuePairSOPReport: nullptr;

    if (!ptr_queue_sop_report) {
        emit SendLogInfoAsynSig("SOPReport显示线程启动失败：数据队列为空", LOGTYPE::ERRORS);
        return;
    }

    while (isThreadStopFlag) {
        if (!isThreadPauseFlag) {
            std::unique_lock<std::mutex> thread_locker(mutexCycleRun);
            if (!isThreadPauseFlag) {
                conditionVariableThread.wait(thread_locker);
            }
            thread_locker.unlock();
            if (!isThreadStopFlag) break; // 被要求退出
        }

        std::pair<int, PtrVMObject> ptr_vmobject;
        if (ptr_queue_sop_report->try_pop(ptr_vmobject)) {
            empty_sleepms = 2; // 成功取到数据，重置退避
            int index = ptr_vmobject.first;
            PtrVMObject ptr_mobject = ptr_vmobject.second;
            if (ptr_mobject == nullptr) {
                emit SendLogInfoAsynSig(" SOPReport显示线程获取数据失败 ", LOGTYPE::ERRORS);
                continue;
            }
            if (ptr_mobject->cgType != VMDATATYPE::VM_MAT) {
                emit SendLogInfoAsynSig(" SOPReport显示线程获取数据类型错误 ", LOGTYPE::ERRORS);
                continue;
            }
            auto vm_mat = std::static_pointer_cast<VMMat>(ptr_mobject);
            cv::Mat image = vm_mat->vmMat;
            if (image.empty()) {
                emit SendLogInfoAsynSig("SOPReport显示线程：空图像帧", LOGTYPE::WARN);
                continue;
            }
            auto qimage = CvMat2QImage(image);
            emit SendRenderImageShowSig(index, qimage);

        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(empty_sleepms));
            empty_sleepms = std::min(empty_sleepms * 2, 200);
        }
    }
}


QImage AutoSOPReportShowThread::CvMat2QImage(const cv::Mat& mat)
{
    QImage image;
    switch (mat.type())
    {
    case CV_8UC1:
        // QImage构造：数据，宽度，高度，每行多少字节，存储结构
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        break;
    case CV_8UC3:
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        image = image.rgbSwapped(); // BRG转为RGB
        // Qt5.14增加了Format_BGR888
        // image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.cols * 3, QImage::Format_BGR888);
        break;
    case CV_8UC4:
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        break;
    case CV_16UC4:
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGBA64);
        image = image.rgbSwapped(); // BRG转为RGB
        break;
    }
    return image;
}