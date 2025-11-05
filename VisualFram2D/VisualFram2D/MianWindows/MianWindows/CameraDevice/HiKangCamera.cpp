#include "HiKangCamera.h"
#include <sstream>
#include <QDateTime>
#include <QDir>

HiKangCamera::HiKangCamera()
{
    unPtrVideoCapture = std::make_unique<cv::VideoCapture>();  
    unPtrVideoWriter = std::make_unique<cv::VideoWriter>();
    //开启保存线程
    isThreadStopFlag = true;
    cycleCamerSaveDataThread = std::make_unique<std::thread>(&HiKangCamera::SaveVideoImage, this);

    unPtrVideoCapture->set(cv::CAP_PROP_BUFFERSIZE, 2); // 内部缓冲区帧数
    unPtrVideoCapture->set(cv::CAP_PROP_FPS, 30);       // 设置期望帧率
    std::ostringstream ffmpeg_options;
    ffmpeg_options << "rtsp_transport=tcp"; // 强制使用TCP传输
    ffmpeg_options << ":buffer_size=" << 1024 * 1024; // 设置缓冲区大小（字节）
    ffmpeg_options << ":max_delay=500000";  // 最大延迟（微秒）
    ffmpeg_options << ":fflags=nobuffer";   // 减少缓冲
}

HiKangCamera::~HiKangCamera()
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


bool HiKangCamera::MvOpenDevice(const  CameraParam& camera_param)
{
    this->cameraParam = camera_param;;
    bool res =  unPtrVideoCapture->open(camera_param.strURL.toStdString(), cv::CAP_FFMPEG);
    width = static_cast<int>(unPtrVideoCapture->get(cv::CAP_PROP_FRAME_WIDTH));
    height = static_cast<int>(unPtrVideoCapture->get(cv::CAP_PROP_FRAME_HEIGHT));
    fps = unPtrVideoCapture->get(cv::CAP_PROP_FPS);
    coder = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
    segmentFrames = saveTimesLenght * fps; // 计算保存的帧数

    std::string outputFile = GenerateFilename(cameraParam.savePath.toStdString());
    if (!unPtrVideoWriter->open(outputFile, coder, fps, cv::Size(width, height))) {
        return false;
    }

    return res;
}

void HiKangCamera::MvCloeDevice()
{
    unPtrVideoCapture->release();
    unPtrVideoWriter->release();
}

bool HiKangCamera::MvGetImage(cv::Mat& image)
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

//bool HiKangCamera::SaveVideoImage()
//{
//    int frame_count{ 0 };  // 记录帧数
//    while (isThreadStopFlag) {
//        cv::Mat image;
//        if (ptrQueueCameraSaveData.try_pop(image)) {
//            if (frame_count == segmentFrames) {
//                // 关闭上一个writer
//                if (unPtrVideoWriter->isOpened()) {
//                    unPtrVideoWriter->release();
//                }
//                std::string outputFile = GenerateFilename(cameraParam.savePath.toStdString());
//                if (!unPtrVideoWriter->open(outputFile, coder, fps, cv::Size(width, height))) {
//                    continue;
//                }
//                frame_count = 0;
//            }
//            frame_count++;
//            unPtrVideoWriter->write(image);
//        }
//        else{
//            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 避免忙等待
//        }
//    }
//    unPtrVideoWriter->release();
//
//    return true;
//}

bool HiKangCamera::SaveVideoImage() {
    int frame_count{ 0 };  // 记录帧数
    const int target_width = 1280;  // 目标宽度
    const int target_height = 720;  // 目标高度

    while (isThreadStopFlag) {
        cv::Mat image;
        if (ptrQueueCameraSaveData.try_pop(image)) {
            // 检查原始帧有效性
            if (!image.empty() && image.cols == 2560 && image.rows == 1440 && image.isContinuous()) {
                // 缩放帧到目标分辨率
                cv::Mat resized_image;
                cv::resize(image, resized_image, cv::Size(target_width, target_height), 0, 0, cv::INTER_AREA);

                // 验证缩放后的帧
                if (!resized_image.empty() && resized_image.cols == target_width && resized_image.rows == target_height) {
                    if (frame_count == segmentFrames) {
                        // 关闭上一个writer
                        if (unPtrVideoWriter->isOpened()) {
                            unPtrVideoWriter->release();
                            std::cout << "Closed VideoWriter for segment switch." << std::endl;
                        }
                        std::string outputFile = GenerateFilename(cameraParam.savePath.toStdString());
                        std::cout << "Opening new video file: " << outputFile << std::endl;
                        // 使用缩放后的分辨率初始化 VideoWriter
                        if (!unPtrVideoWriter->open(outputFile, coder, fps, cv::Size(target_width, target_height))) {
                            std::cerr << "Failed to open VideoWriter for " << outputFile << std::endl;
                            continue;
                        }
                        frame_count = 0;
                    }
                    frame_count++;
                    unPtrVideoWriter->write(resized_image);
                }
                else {
                    std::cerr << "Invalid resized frame detected (empty: " << resized_image.empty()
                        << ", size: " << resized_image.cols << "x" << resized_image.rows
                        << "), skipping write." << std::endl;
                }
            }
            else {
                std::cerr << "Invalid original frame detected (empty: " << image.empty()
                    << ", size: " << image.cols << "x" << image.rows
                    << "), skipping write." << std::endl;
            }
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 避免忙等待
        }
    }
    if (unPtrVideoWriter->isOpened()) {
        unPtrVideoWriter->release();
        std::cout << "Closed VideoWriter on thread exit." << std::endl;
    }
    return true;
}