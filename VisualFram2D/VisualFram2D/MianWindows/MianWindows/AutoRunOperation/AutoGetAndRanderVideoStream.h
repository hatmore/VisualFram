#pragma once
#include <QObject>
#include <QDialog>
#include <QDialog>
#include <QMap>
#include "../CameraDevice/BaseCamera.h"
#include "../GeneralStruct.h"
#include "../ThreadPools/ThreadPool.h"


class AutoGetAndRanderVideoStream : public QObject
{
    Q_OBJECT
public:
    using Ptr = std::shared_ptr<AutoGetAndRanderVideoStream>;
    using ConstPtr = std::shared_ptr<const AutoGetAndRanderVideoStream>;

    AutoGetAndRanderVideoStream();
    ~AutoGetAndRanderVideoStream();

     /// <summary>
    /// 获得dialog，相机设置指针
    /// </summary>
    /// <param name="dialog"></param>
    void GetFramCameraDialog(QDialog* dialog);

    /// <summary>
    /// 显示画面
    /// </summary>
    /// <param name="dialog"></param>
    void GetFramImageView(QWidget* dialog);

    /// <summary>
    /// 创建，现有线程
    /// </summary>
    void BuildCycleThread();

    /// <summary>
    /// 销毁，现有线程
    /// </summary>
    void DestroyCycleThread();

    /// <summary>
    /// 开始解析外部事件线程
    /// </summary>
    void StartCycleThread();

    /// <summary>
    /// 暂停解析外部事件线程
    /// </summary>
    void PauseCycleThread();

    /// <summary>
    /// 线程是否运行
    /// </summary>
    /// <returns></returns>
    const bool IsRun() { return this->isThreadPauseFlag; }

    /// <summary>
    /// 线程是否创建
    /// </summary>
    /// <returns></returns>
    const bool IsCreateThread() { return this->isThreadStopFlag; }

private:
    void Initial();

    /// <summary>
    /// 打开获得相机数据
    /// </summary>
    void CycleGetMultiCameraData();

    /// <summary>
    /// 单个相机线程
    /// </summary>
    /// <param name="index"></param>
    /// <param name="ptr_camera"></param>
    void CycleGetCameraData(const int index, const BaseCamera::Ptr& ptr_camera);

    /// <summary>
    /// 
    /// </summary>
    /// <param name="mat"></param>
    /// <returns></returns>
    QImage CvMat2QImage(const cv::Mat& mat);

    /// <summary>
    /// 渲染图像
    /// </summary>
    /// <param name="image">输入图像</param>
    /// <param name="v_ptr_mobject">渲染参数</param>
    int RanderImageResult(cv::Mat& image, const  std::vector<PtrVMObject>& v_ptr_mobject, const int screen_index = 0);

signals:
    /// <summary>
    /// 发送显示
    /// </summary>
    void SendRenderImageShowSig(const int index, const QImage& qimage);
    /// <summary>
    /// 同步信号
    /// </summary>
    void SendLogInfoSynSig(const QString& info, const LOGTYPE type);

    /// <summary>
    /// 异步信号
    /// </summary>
    void SendLogInfoAsynSig(const QString& info, const LOGTYPE type);

private:
    std::atomic<bool> isThreadPauseFlag{ false };  // 线程暂停标志, fasle暂停
    std::atomic<bool> isThreadStopFlag{ false };   //线程停止标志， false停止
    std::mutex mutexCycleRun;
    std::condition_variable conditionVariableThread;
   // std::unique_ptr<std::thread> cycleRunCameraDataThread{ nullptr };
    ThreadPool::Ptr ptrThreadPools;

private:
    std::shared_ptr<QMap<int, BaseCamera::Ptr>> ptrMapIndexCameraDevices{ nullptr };  //相机设备相关参数
    int sampleFrequency{5};  //采样频率
    std::map<int, int> cameraIndexToScreenNum;  //维护一个映射，从相机索引（index）到 functionParam.screenNum 的对应关系

    const std::vector<std::vector<int>> skeleton{
        {16, 14},
        {14, 12},
        {17, 15},
        {15, 13},
        {12, 13},
        {6, 12},
        {7, 13},
        {6, 7},
        {6, 8},
        {7, 9},
        {8, 10},
        {9, 11},
        {2, 3},
        {1, 2},
        {1, 3},
        {2, 4},
        {3, 5},
        {4, 6},
        {5, 7}
    };

};


