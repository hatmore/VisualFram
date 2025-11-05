#pragma once
#include <QMap>
#include <QObject>
#include <tbb\concurrent_queue.h>
#include "../GeneralStruct.h"

class AutoSOPReportShowThread : public QObject
{
    Q_OBJECT
public:
    using Ptr = std::shared_ptr<AutoSOPReportShowThread>;
    using ConstPtr = std::shared_ptr<const AutoSOPReportShowThread>;

    AutoSOPReportShowThread();
    ~AutoSOPReportShowThread();

    /// <summary>
    /// 显示画面
    /// </summary>
    /// <param name="dialog"></param>
    void GetFramImageView(QWidget* dialog);

    /// <summary>
    /// 创建，现有线程
    /// </summary>
    bool BuildCycleThread();

    /// <summary>
    /// 销毁，现有线程
    /// </summary>
    bool DestroyCycleThread();

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
    /// <summary>
    /// 解析接收到的外部信息，循环线程
    /// </summary>
    void CycleRunSOPReportShowThread();

    /// <summary>
    /// 
    /// </summary>
    /// <param name="mat"></param>
    /// <returns></returns>
    QImage CvMat2QImage(const cv::Mat& mat);

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

    std::unique_ptr<std::thread> cycleRunSOPReportShowThread{ nullptr };

};

