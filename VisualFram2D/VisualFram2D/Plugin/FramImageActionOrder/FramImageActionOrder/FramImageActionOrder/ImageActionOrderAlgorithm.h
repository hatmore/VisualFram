#pragma once
#include <opencv2/opencv.hpp>
#include "SerializeStructParam.h"
#include "PublicFunction.h"
#include "GeneralStruct.h"
#include <tbb/concurrent_queue.h> 


struct ROI {
    int id;                //ROI的唯一标识符（如1, 2, 3）
    cv::Rect area;         //区域的位置
    bool state;            //目标是否被检测到标志   
    std::string name;
    float min_confidence;
    int orderId;
};

struct DetectedObject {
    int class_id;
    float confidence;   // 置信度
    cv::Rect box;       // 位置
    bool state;
    int IsPtsBox;  //0=关键点，1=检测框，2=多边形
    cv::Point2d keyPoint;//关键点
    std::vector<cv::Point2d> polygonPoints;//存储多边形点

};
using DetectionResults = std::vector<DetectedObject>;  // 一帧中所有检测目标


// 记录每个动作首次发生的信息
struct FirstActionEvent {
    int roi_id;          // ROI区域ID
    int action_class;    // 动作类别
    uint64_t timestamp;  // 首次发生时间戳
    bool detected;       // 是否已检测到
};

class ImageActionOrderAlgorithm
{
public:

    ImageActionOrderAlgorithm() ;
    ~ImageActionOrderAlgorithm();

    using Ptr = std::shared_ptr<ImageActionOrderAlgorithm>;

    void SetParam(const std::vector<ROI>&area);

    // 日志
    void SetLogQueue(std::shared_ptr<tbb::concurrent_queue<std::pair<LOGTYPE, QString>>> logQueue) {
        ptrQueueNodeLogData = logQueue;
    }

    int RunProcessFrame(PtrVMUnorderedMap<int, PtrVMNodeState>& ptrOutVec, const DetectionResults& detections,
        uint64_t timestamp, float referIouValue);

    std::vector<ROI>InitRoi(PtrVMUnorderedMap<int, PtrVMVector<VMRectangle>>& roi);
    std::vector<DetectedObject>InitModel(const BaseNodeData::Ptr inputNode, uint64_t &timePoint);

    //std::unordered_map<int, FirstActionEvent> first_occurrences;  // 各ROI的首次动作记录
    // 存储每个ROI最近三帧的检测状态（true=检测到，false=未检测到）
    std::unordered_map<int, std::deque<bool>> roiDetectionHistory;

    

private:
    std::vector<ROI>rois;

    // 日志
    std::shared_ptr<tbb::concurrent_queue<std::pair<LOGTYPE, QString>>> ptrQueueNodeLogData = nullptr;
    void WriteLog(const QString& message, LOGTYPE logType = LOGTYPE::INFO);
 
};

