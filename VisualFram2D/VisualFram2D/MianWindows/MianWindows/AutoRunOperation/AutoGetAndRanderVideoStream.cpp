#pragma execution_character_set("utf-8")
#include "AutoGetAndRanderVideoStream.h"
#include "../StaticGlobalVariable.h"
#include "../CameraDevice/FramCameraSetParam.h"
#include "../Log/frmLog.h"
#include "../ImageView/FramImageView.h"
#include <QDateTime>
#include <QDate>
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

AutoGetAndRanderVideoStream::AutoGetAndRanderVideoStream()
{
    Initial();
    FrmLog* ptr_framlog = FrmLog::FrmLogInstance();
    QObject::connect(this, &AutoGetAndRanderVideoStream::SendLogInfoSynSig, ptr_framlog, &FrmLog::LogMassageShowSlot);
    QObject::connect(this, &AutoGetAndRanderVideoStream::SendLogInfoAsynSig, ptr_framlog, &FrmLog::LogMassageShowSlot, Qt::BlockingQueuedConnection);
}

AutoGetAndRanderVideoStream::~AutoGetAndRanderVideoStream()
{
    this->DestroyCycleThread();
}

void AutoGetAndRanderVideoStream::Initial()
{
    ptrThreadPools = std::make_shared<ThreadPool>(MaxThreadNum);
    ptrThreadPools->Init();
}

void AutoGetAndRanderVideoStream::GetFramCameraDialog(QDialog* dialog)
{
    FramCameraSetParam* fram_dialog = dynamic_cast<FramCameraSetParam*>(dialog);
    ptrMapIndexCameraDevices = fram_dialog->GetMapIndexCameraDevices();
}

void AutoGetAndRanderVideoStream::GetFramImageView(QWidget* dialog)
{
    FramImageView* fram_dialog = dynamic_cast<FramImageView*>(dialog);
    QObject::connect(this, &AutoGetAndRanderVideoStream::SendRenderImageShowSig, fram_dialog, &FramImageView::DispImage,Qt::BlockingQueuedConnection);
}

void AutoGetAndRanderVideoStream::BuildCycleThread() 
{
    if (ptrMapIndexCameraDevices == nullptr)
        return;

    if (isThreadStopFlag) {
        emit SendLogInfoSynSig( " 采集渲染流程已经开启，请检查! ", LOGTYPE::ERRORS);
        return;
    }

    isThreadStopFlag = true;
    isThreadPauseFlag = true;
    CycleGetMultiCameraData();
}

void AutoGetAndRanderVideoStream::DestroyCycleThread() 
{
    isThreadStopFlag = false;
    isThreadPauseFlag = false;
    conditionVariableThread.notify_all();
    ptrThreadPools->Shutdown();
}

void AutoGetAndRanderVideoStream::StartCycleThread()
{
    auto ptr_map_queue_devicedata = StaticGlobalVariable::ptrToolNodeDataInteract->ptrMapQueuePtrExDeviceData;
    for (auto iter = ptr_map_queue_devicedata->begin(); iter != ptr_map_queue_devicedata->end(); ++iter) {
        std::shared_ptr<tbb::concurrent_bounded_queue<PtrExDeviceData>> ptr_queue = iter->second;
        ptr_queue->clear();
    }

    isThreadStopFlag = true;
    isThreadPauseFlag = true;
    CycleGetMultiCameraData();
    conditionVariableThread.notify_all();
}

void AutoGetAndRanderVideoStream::PauseCycleThread()
{
    isThreadStopFlag = false;
    isThreadPauseFlag = false;
    conditionVariableThread.notify_all();
}

void AutoGetAndRanderVideoStream::CycleGetMultiCameraData()
{
    for (auto iter = ptrMapIndexCameraDevices->constBegin(); iter != ptrMapIndexCameraDevices->constEnd(); ++iter) {
        int key = iter.key();
        BaseCamera::Ptr ptr_camera = iter.value();
        std::future<void>  get_future = ptrThreadPools->Submit([this, key, ptr_camera]() {
            this->CycleGetCameraData(key, ptr_camera);
            }
        );
    }
}

/// <summary>
/// 由于相机设置不能修改，因此这里采样频率只能在新建相机的时候设置好
/// 要求实时性修改参数，可能会涉及到多线程处理，后续再考虑
/// 降低代码难度;
/// </summary>
/// <param name="index">相机id</param>
/// <param name="ptr_camera">相机指针</param>
void AutoGetAndRanderVideoStream::CycleGetCameraData(const int index, const BaseCamera::Ptr& ptr_camera)
{
    CameraParam camera_param = ptr_camera->GetDeviceParam();
    ///采样频率
    int sampleFrequencyTime = camera_param.sampleFrequencyTime;
    QString camera_name = camera_param.deviceName;
    QString info = "相机获取图像: -- " + camera_name  + ": ";
    int sample_frequency = 0;
    std::vector<PtrVMObject> vPtrVmobject;

    if (!ptr_camera->GetCameraConnectState()) {
        emit SendLogInfoAsynSig(info + "相机断开链接，采图失败!  ", LOGTYPE::ERRORS);
    }
    ///打开相机
    ptr_camera->StartPlay();
    //判断是否录像
    if (camera_param.isSave) { 
        ptr_camera->StartRecord();
    }
    //图像数据
    auto ptr_map_queue_devicedata = StaticGlobalVariable::ptrToolNodeDataInteract->ptrMapQueuePtrExDeviceData;
    //渲染数据
    std::shared_ptr<tbb::concurrent_queue<std::vector<PtrVMObject>>> ptr_queue_rander = (*StaticGlobalVariable::ptrToolNodeDataInteract->ptrMapPtrQueueNodeRanderObject)[index];

    auto iter = ptr_map_queue_devicedata->find(camera_name);
    if (iter == ptr_map_queue_devicedata->end()) {
        emit SendLogInfoAsynSig(camera_name + "相机不存在，请检查后，重新运行！ ", LOGTYPE::ERRORS);
        return;
    }
    std::shared_ptr<tbb::concurrent_bounded_queue<ExDeviceData::Ptr>> ptr_queue_devicedata = iter->second;
    
    RateLimiter empty_limiter;

    while (isThreadStopFlag) {
        if (!isThreadPauseFlag) {
            std::unique_lock<std::mutex> thread_locker(mutexCycleRun);
            if (!isThreadPauseFlag) {
                conditionVariableThread.wait(thread_locker);
            }
            thread_locker.unlock();
        }
        if (ptr_camera == nullptr) {
            if (empty_limiter.allow(std::chrono::milliseconds(1000)))
                 emit SendLogInfoAsynSig(info + "相机指针为空! ", LOGTYPE::ERRORS);
            break;
        }
        if (!ptr_camera->GetCameraConnectState()) {
            if (empty_limiter.allow(std::chrono::milliseconds(1000)))
                 emit SendLogInfoAsynSig(info + "相机断开链接，采图失败!  ", LOGTYPE::ERRORS);
            continue;
        }
       cv::Mat image;
       bool res =  ptr_camera->MvGetImage(image);
       if (!res)
           continue;

      ///设置检测采集频率，5hz检测一次
       sample_frequency++;
       if (sample_frequency == sampleFrequencyTime) {
           DeviceCameraData2D::Ptr ptr = std::make_shared<DeviceCameraData2D>();
           ptr->cameraData = image.clone();
           ptr->timeStamp = QDateTime::currentMSecsSinceEpoch();
           if (!ptr_queue_devicedata->try_push(std::move(ptr))) {
               ExDeviceData::Ptr ptr_;
               ptr_queue_devicedata->pop(ptr_);
               ptr_queue_devicedata->push(std::move(ptr));
           }
           sample_frequency = 0;
       }
       //to_do,刷新够快，不需要这么写，但不知道刷新如何，后续更改
       std::vector<PtrVMObject> vptr_vmobject;
       if (ptr_queue_rander->try_pop(vptr_vmobject)) {
           if (!vptr_vmobject.empty()) {
               std::vector<PtrVMObject>().swap(vPtrVmobject);
               vPtrVmobject = vptr_vmobject;
           }
       }
       RanderImageResult(image, vPtrVmobject);
       auto qimage = CvMat2QImage(image);
       emit SendRenderImageShowSig(index, qimage);
    }
    ptr_camera->StopPlay();
}


QImage AutoGetAndRanderVideoStream::CvMat2QImage(const cv::Mat& mat)
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

int AutoGetAndRanderVideoStream::RanderImageResult(cv::Mat& image, const std::vector<PtrVMObject>& v_ptr_mobject, const int screen_index)
{
    size_t index = 0;
    for (auto it = v_ptr_mobject.cbegin(); it != v_ptr_mobject.cend(); ++it) {
        auto ptr_mobject = *it;
        VMDATATYPE cg_type = ptr_mobject->cgType;
        bool state = ptr_mobject->state;
        ///可以修改，针对不同vector
        if (state == false) {
            return -1;
        }

        int class_id = ptr_mobject->calssId;
        QString tool_name = ptr_mobject->toolName;
        float confidence = ptr_mobject->confidence;

        switch (cg_type) {
        case VM_NULL:
            break;
        case VM_BOOL: {
            auto vm_node_state = std::static_pointer_cast<VMNodeState>(ptr_mobject);
            uint64 time_stamp = vm_node_state->timeStamp;

            std::string label = cv::format("ID: %d; Time: %d; %.2f", class_id, time_stamp, confidence);
            int base_line;
            cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &base_line);
            cv::putText(image, label, cv::Point(0, (index++) * 20 - base_line), 2.5, cv::FONT_HERSHEY_DUPLEX, cv::Scalar(255, 255, 255), 3, cv::LINE_AA);
        }
                    break;
        case VM_POINT: {
            auto vm_p = std::static_pointer_cast<VMPoint2d>(ptr_mobject);
            std::string label = cv::format("ID: %d; %.2f", class_id, confidence);
            int base_line;
            cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &base_line);
            cv::Point p = cv::Point(vm_p->x, vm_p->y);
            cv::circle(image, p, 10, cv::Scalar(0, 255, 120), -1);//画点，其实就是实心圆
            cv::putText(image, label, cv::Point(vm_p->x, vm_p->y - base_line), 2.5, cv::FONT_HERSHEY_DUPLEX, cv::Scalar(255, 255, 255), 3, cv::LINE_AA);
        }
                     break;
        case VM_RECTANGLE: {
            auto vm_rectangle = std::static_pointer_cast<VMRectangle>(ptr_mobject);
            cv::RotatedRect rorect = cv::RotatedRect(cv::Point(vm_rectangle->centerPoint.x, vm_rectangle->centerPoint.y),
                cv::Size(vm_rectangle->width, vm_rectangle->height), -vm_rectangle->angle);
            cv::Point2f vertices[4];
            rorect.points(vertices);
            cv::Point p1 = cv::Point((vertices[0].x + vertices[3].x) / 2, (vertices[0].y + vertices[3].y) / 2);
            cv::Point p2 = cv::Point((vertices[1].x + vertices[2].x) / 2, (vertices[1].y + vertices[2].y) / 2);
            for (int i = 0; i < 4; i++) {
                line(image, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 0, 255), 3);
            }

            std::string label = cv::format("ID: %d; %.2f", class_id, confidence);
            int base_line;
            cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &base_line);
            cv::putText(image, label, cv::Point(rorect.boundingRect().tl().x, rorect.boundingRect().tl().y - base_line), 2.5, cv::FONT_HERSHEY_DUPLEX, cv::Scalar(255, 255, 255), 3, cv::LINE_AA);
        }
                         break;
        case VM_POLYGON: {
            //画点
            auto vm_polygon = std::static_pointer_cast<VMPolygon>(ptr_mobject);
            std::vector<cv::Point> v_point;
            float pointConfidence;
            for (size_t i = 0; i < vm_polygon->vectorPoints.size(); i++) {
                VMPoint2d vm_point = vm_polygon->vectorPoints[i];
                pointConfidence = vm_polygon->vectorPoints[i].confidence;
                cv::Point cv_point = cv::Point(vm_point.x, vm_point.y);
                v_point.push_back(cv_point);

                if (vm_point.x < 0 || vm_point.y < 0) continue;
                if (pointConfidence < 0.5) continue;
                cv::circle(image, cv_point, 4, cv::Scalar(0, 255, 255), -1);//画点，其实就是实心圆
            }
            //画矩形框
            int penWidth = 2;
            cv::Rect aabb = cv::boundingRect(v_point);
            cv::rectangle(image, aabb, cv::Scalar(255, 125, 255), penWidth, cv::LINE_AA);
            //画关节
            int kpt1_idx, kpt2_idx, kpt1_x, kpt1_y, kpt2_x, kpt2_y;
            float kpt1_conf, kpt2_conf;

            cv::Scalar skeletonColor(0, 255, 255);
            for (size_t m = 0; m < skeleton.size(); m++) {
                kpt1_idx = skeleton[m][0] - 1;
                kpt2_idx = skeleton[m][1] - 1;
                kpt1_x = (int)v_point[kpt1_idx].x;
                kpt1_y = (int)v_point[kpt1_idx].y;
                kpt1_conf = vm_polygon->vectorPoints[kpt1_idx].confidence;
                kpt2_x = (int)v_point[kpt2_idx].x;
                kpt2_y = (int)v_point[kpt2_idx].y;
                kpt2_conf = vm_polygon->vectorPoints[kpt2_idx].confidence;
                if (kpt1_conf < 0.5 || kpt2_conf < 0.5) continue;
                if (kpt1_x < 0 || kpt1_y < 0) continue;
                if (kpt2_x < 0 || kpt2_y < 0) continue;
                cv::line(image, cv::Point(kpt1_x, kpt1_y), cv::Point(kpt2_x, kpt2_y), skeletonColor, penWidth, cv::LINE_AA);
            }

            std::string label = cv::format("ID: %d; %.2f", class_id, confidence);
            int base_line;
            cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &base_line);
            cv::putText(image, label, cv::Point(aabb.x, aabb.y - base_line), 2.5, cv::FONT_HERSHEY_DUPLEX, cv::Scalar(255, 255, 255), 3, cv::LINE_AA);

        }break;
        case VM_MAT:
            break;
        case VM_VECTOR:
            break;
        default:
            break;
        }
    }
    return 0;
}
