#include "VirtualCamera.h"
#include <sstream>
#include <QDateTime>
#include <QDir>

VirtualCamera::VirtualCamera()
{
    unPtrVideoCapture = std::make_unique<cv::VideoCapture>();  
    unPtrVideoWriter = std::make_unique<cv::VideoWriter>();
    //开启保存线程
    isThreadStopFlag = true;
    cycleCamerSaveDataThread = std::make_unique<std::thread>(&VirtualCamera::SaveVideoImage, this);

    unPtrVideoCapture->set(cv::CAP_PROP_BUFFERSIZE, 2); // 内部缓冲区帧数
    unPtrVideoCapture->set(cv::CAP_PROP_FPS, 30);       // 设置期望帧率
    std::ostringstream ffmpeg_options;
    ffmpeg_options << "rtsp_transport=tcp"; // 强制使用TCP传输
    ffmpeg_options << ":buffer_size=" << 1024 * 1024; // 设置缓冲区大小（字节）
    ffmpeg_options << ":max_delay=500000";  // 最大延迟（微秒）
    ffmpeg_options << ":fflags=nobuffer";   // 减少缓冲
}

VirtualCamera::~VirtualCamera()
{
    isThreadStopFlag = false;
    if (cycleCamerSaveDataThread != nullptr) {
        if (cycleCamerSaveDataThread->joinable()) {
            cycleCamerSaveDataThread->join();
        }
        cycleCamerSaveDataThread.reset();
        cycleCamerSaveDataThread = nullptr;
    }
    unPtrVideoCapture->release();
    unPtrVideoWriter->release();
}


bool VirtualCamera::MvOpenDevice(const  CameraParam& camera_param)
{
    this->cameraParam = camera_param;;
    bool res =  unPtrVideoCapture->open(camera_param.strURL.toStdString(), cv::CAP_FFMPEG);
    width = static_cast<int>(unPtrVideoCapture->get(cv::CAP_PROP_FRAME_WIDTH));
    height = static_cast<int>(unPtrVideoCapture->get(cv::CAP_PROP_FRAME_HEIGHT));
    fps = unPtrVideoCapture->get(cv::CAP_PROP_FPS);
    coder = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
    segmentFrames = saveTimesLenght * fps; // 计算保存的帧数
    return res;
}

void VirtualCamera::MvCloeDevice()
{
    unPtrVideoCapture->release();
    unPtrVideoWriter->release();
}

bool VirtualCamera::MvGetImage(cv::Mat& image)
{
    cv::Mat get_image;
    bool res =   unPtrVideoCapture->read(get_image);
    if (res) {
        image = get_image;
        if (cameraParam.isSave) {
            ptrQueueCameraSaveData.push(get_image);
        }
        droppedFrames = 0;
    }
    else{
        droppedFrames++;
        if (droppedFrames > 20) {
            unPtrVideoCapture->release();
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (unPtrVideoCapture->open(this->cameraParam.strURL.toStdString(), cv::CAP_FFMPEG)) {
                droppedFrames = 0;
            }
            else{
                return false;
            }
        }
    }
    return res;
}

bool VirtualCamera::SaveVideoImage()
{
    int frame_count{ 0 };  // 记录帧数
    while (isThreadStopFlag) {
        cv::Mat image;
        if (ptrQueueCameraSaveData.try_pop(image)) {
            if (frame_count % segmentFrames == 0) {
                // 关闭上一个writer
                if (unPtrVideoWriter->isOpened()) {
                    unPtrVideoWriter->release();
                }
                std::string outputFile = GenerateFilename(cameraParam.savePath.toStdString());
                if (!unPtrVideoWriter->open(outputFile, coder, fps, cv::Size(width, height))) {
                    return false;
                }
                frame_count = 0;
            }
            frame_count++;
            unPtrVideoWriter->write(image);
        }
        else{
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 避免忙等待
        }
    }
}