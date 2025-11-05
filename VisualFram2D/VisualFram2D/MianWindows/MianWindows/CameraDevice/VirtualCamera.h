#include "BaseCamera.h"
#include <memory>
#include<opencv2/opencv.hpp>


class VirtualCamera:public BaseCamera
{
public:
    VirtualCamera();
    virtual  ~VirtualCamera();

    /// <summary>
    /// 打开对应相机
    /// </summary>
    /// <param name="index"> 由idex 控制</param>
    /// <returns></returns>
    virtual bool MvOpenDevice(const  CameraParam& camera_param);

    /// <summary>
    /// 关闭相机
    /// </summary>
    virtual void MvCloeDevice();

    /// <summary>
    /// 开始采集数据
    /// </summary>
    virtual void StartPlay() {
    
    }

    /// <summary>
    ///暂停采集数据
    /// </summary>
    virtual void StopPlay() {
    
    
    }

    /// <summary>
    /// 获得图像数据
    /// </summary>
    /// <param name=""></param>
    virtual bool  MvGetImage(cv::Mat& image);

    /// <summary>
    /// 开始录像
    /// </summary>
    /// <returns></returns>
    virtual bool StartRecord() {
        return true;
    }

    /// <summary>
    /// 暂停录像
    /// </summary>
    /// <returns></returns>
    virtual bool StopRecord() {
        return true;
    
    }

    /// <summary>
    /// 保存图像
    /// </summary>
    /// <returns></returns>
    virtual bool SaveVideoImage();

    /// <summary>
    /// 获得相机状态
    /// </summary>
    /// <returns></returns>
    virtual const bool GetCameraConnectState() const {
        bool state = unPtrVideoCapture->isOpened();
        return state;
    }

private:
    std::unique_ptr<cv::VideoCapture> unPtrVideoCapture{nullptr}; // 相机对象指针   
    std::unique_ptr<cv::VideoWriter > unPtrVideoWriter{nullptr};
    int droppedFrames{0};
    int width;
    int height;
    double fps;
    int coder;
    int segmentFrames{0};

private:
    int saveTimesLenght{ 60 };  //保存时间，10分钟一段，测试一分钟一段

private:
    std::unique_ptr<std::thread> cycleCamerSaveDataThread{ nullptr };
    bool isThreadStopFlag{ false };   //true表示存在，false表示不存在
};
