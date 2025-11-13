#pragma once
#include <cstdint>
#include <unordered_map>
#include <chrono>
#include"ImageDurationAlgorithm.h"
#include <unordered_set>

#include <fstream>
#include <mutex>
#include <sstream>
#include <iomanip>

struct DurationRecord
{
    uint64_t start_time;  // 动作开始时间戳（ms）
    uint64_t end_time;    // 动作结束时间戳（ms）
    uint64_t duration() const {
        if (end_time < start_time) {
            return 0; // 或者 throw 异常
        }
        return end_time - start_time;
    }
};

//// 输出结构：ROI_ID -> 该ROI所有动作的持续时间记录
//std::unordered_map<int, std::vector<DurationRecord>> roi_durations;
//
//// 存储每个ROI最近三帧的检测状态（true=检测到，false=未检测到）
//std::unordered_map<int, std::deque<bool>> roiDetectionHistory;
//std::unordered_map<int, bool> anyDetectedInLastThreeFrames;

// 获取当前时间戳（毫秒）
inline uint64_t GetCurrentTimestamp()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

inline bool CalculateIoU(const cv::Rect& rect1, const cv::Rect& rect2,float ReferIou)
{
    int x1 = std::max(rect1.x, rect2.x);
    int y1 = std::max(rect1.y, rect2.y);
    int x2 = std::min(rect1.x + rect1.width, rect2.x + rect2.width);
    int y2 = std::min(rect1.y + rect1.height, rect2.y + rect2.height);
    int intersection_area = std::max(0, x2 - x1) * std::max(0, y2 - y1);
    if (intersection_area == 0) {
        return 0.0f;
    }
    //考虑到一个大的检测区域可能多个位置都会发生某一种动作
    //通过计算交并比来判断物体是否出现在Roi区域，有点问题，换一种计算方法
    //原来：int union_area = rect1.area() + rect2.area() - intersection_area;
    //计算交集区域与各区域的占比大小，输出最大的那个
    float unionValue1 = static_cast<float>(intersection_area) / rect1.area();
    float unionValue2 = static_cast<float>(intersection_area) / rect2.area();
    float unionValue = std::max(unionValue1, unionValue2);
    return ((unionValue > ReferIou/10.0) ? true : false);
}

class ROIActionTracker
{
public:
    ROIActionTracker(const std::vector<ROI>& regions) : rois(regions) {}

private:
    struct ActionState
    {
        bool is_active = false;
        uint64_t start_time = 0;
        int miss_count = 0;
    };
public:
    static std::unordered_map<int, ActionState> active_actions;  // ROI_ID -> 当前状态

    // 跟踪已记录过的最高orderId（用于强制顺序）
    int highestRecordedOrderId = 0;


    std::vector<ROI> rois;  // 预定义的ROI列表
    
    std::unordered_map<int, std::vector<DurationRecord>> roi_durations;  // 结果存储
    // 添加这两个成员变量：
    std::unordered_map<int, std::deque<bool>> roiDetectionHistory;
    std::unordered_map<int, bool> anyDetectedInLastThreeFrames;

public:
    //PtrVMUnorderedMap<int, PtrVMNodeState> result { nullptr };

    // 日志文件路径
    std::string logFilePath = "D:/app/TestData/ROIDebugLog.txt";
    static std::mutex logMutex;  // 保证线程安全

    // 日志输出函数
    void WriteLog(const std::string& message) {
        std::lock_guard<std::mutex> lock(logMutex);
        std::ofstream logFile(logFilePath, std::ios::app);
        if (logFile.is_open()) {
            // 添加时间戳
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) % 1000;

            std::tm tm;
            localtime_s(&tm, &time_t);

            logFile << std::put_time(&tm, "%H:%M:%S")
                << "." << std::setfill('0') << std::setw(3) << ms.count()
                << " - " << message << std::endl;
            logFile.close();
        }
    }

public:

    int ProcessFrame(const DetectionResults& detections, const uint64_t& timestamp, VideoInfo frame)
    {
        // Step 1: 检测哪些ROI（按orderId）当前帧是否有目标动作
        std::unordered_set<int> detected_roi_order_ids; // 存储当前帧中有目标动作的ROI orderId集合

        if (detections.empty()) {
            return -1;
        }
        int detectionType = detections[0].IsPtsBox;

        switch (detectionType) {
        case 0: // 处理关键点
        {
            // 遍历所有ROI，检查每个ROI是否包含足够的点
            for (const ROI& roi : rois) {
                int point_count = 0;

                // 统计当前ROI包含的点数
                for (const auto& detection : detections) {
                    if (roi.area.contains(detection.keyPoint) && detection.state) {
                        point_count++;
                    }
                }

                // 检查当前ROI是否满足条件
                if (point_count >= frame.IouValue) {
                    detected_roi_order_ids.insert(roi.orderId);
                }
            }
            break;
        }
        case 1: // 处理检测框
        {
            // 遍历所有ROI，检查每个ROI是否有匹配的检测框
            for (const ROI& roi : rois) {
                for (const auto& detection : detections) {
                    if (detection.class_id == roi.id &&
                        detection.state &&
                        CalculateIoU(detection.box, roi.area, frame.IouValue)) {
                        detected_roi_order_ids.insert(roi.orderId);
                        break; // 只要有一个检测框匹配就跳出内层循环
                    }
                }
            }
            break;
        }
        case 2: // 处理多边形点
        {
            // 遍历所有ROI，检查每个ROI是否包含足够的点
            for (const ROI& roi : rois) {
                int polygon_point_count = 0;

                for (const auto& detection : detections) {
                    if (!detection.polygonPoints.empty() && detection.state) {
                        for (const auto& point : detection.polygonPoints) {
                            if (roi.area.contains(point)) {
                                polygon_point_count++;
                            }
                        }
                    }
                }

                // 检查当前ROI是否满足条件
                if (polygon_point_count >= frame.IouValue) {
                    detected_roi_order_ids.insert(roi.orderId);
                }
            }
            break;
        }
        default:
            break;
        }

        // ====== 新增：顺序强制逻辑 ======
        // 1. 获取所有ROI中最大的orderId
        int maxOrderId = 0;
        for (const ROI& roi : rois) {
            if (roi.orderId > maxOrderId) {
                maxOrderId = roi.orderId;
            }
        }

        //// 2. 计算下一个期望的orderId
        //int nextExpectedOrderId = highestRecordedOrderId + 1;

        //// 3. 只保留符合顺序的检测
        //std::unordered_set<int> filtered_detected_roi_order_ids;
        //for (int orderId : detected_roi_order_ids) {
        //    if (orderId == nextExpectedOrderId) {
        //        filtered_detected_roi_order_ids.insert(orderId);
        //        break; // 只处理下一个顺序的动作
        //    }
        //}

        //// 4. 用过滤后的结果替换原始检测结果
        //detected_roi_order_ids = filtered_detected_roi_order_ids;
        // ====== 顺序强制逻辑结束 ======

        // 更新每个orderId的最近三帧检测状态
        for (int orderId : detected_roi_order_ids) {
            // 初始化检测历史（如果不存在）
            if (roiDetectionHistory.find(orderId) == roiDetectionHistory.end()) {
                roiDetectionHistory[orderId] = std::deque<bool>(3, false);
            }

            // 更新检测状态
            roiDetectionHistory[orderId].pop_front();
            roiDetectionHistory[orderId].push_back(true);

            // 检查最近三帧中是否有任意2帧检测到
            anyDetectedInLastThreeFrames[orderId] = false;
            int judgeCount = 0;
            for (bool frameDetected : roiDetectionHistory[orderId]) {
                if (frameDetected) {
                    judgeCount++;
                    if (judgeCount >= 2) {
                        anyDetectedInLastThreeFrames[orderId] = true;
                        break;
                    }
                }
            }
        }
        // Step 2: 更新状态并记录持续时间（按 ROI orderId）
        for (const ROI& roi : rois) {
            // 使用 orderId 作为键来跟踪状态
            ActionState& state = active_actions[roi.orderId];

            if (detected_roi_order_ids.count(roi.orderId)) {
                // 情况1: 该 ROI 当前帧有目标
                if (!state.is_active) {
                    if (anyDetectedInLastThreeFrames[roi.orderId]) {
                        // 新增：检查是否符合顺序
                        int nextExpectedOrderId = highestRecordedOrderId + 1;

                        // 添加调试日志
                        std::ostringstream oss;
                        oss << "检测到动作 " << roi.orderId
                            << ", 期望的是 " << nextExpectedOrderId
                            << ", 当前highest=" << highestRecordedOrderId;
                        WriteLog(oss.str());

                        if (roi.orderId == nextExpectedOrderId) {
                            // 符合顺序，开始记录
                            state = { true, timestamp, 0 };

                            // 立即更新已记录的最高orderId
                            highestRecordedOrderId = roi.orderId;

                            // 添加调试日志
                            std::ostringstream oss2;
                            oss2 << "开始记录动作 " << roi.orderId
                                << ", 更新highest=" << highestRecordedOrderId;
                            WriteLog(oss2.str());
                        }
                        else {
                            // 添加调试日志
                            std::ostringstream oss3;
                            oss3 << "跳过动作 " << roi.orderId << " (不符合顺序)";
                            WriteLog(oss3.str());
                        }
                    }
                }
                else {
                    // 持续检测，重置丢失计数
                    state.miss_count = 0;
                }
            }
            else if (state.is_active) {
                // 情况2: 该 ROI 当前帧无目标但之前有动作
                if (state.miss_count >= frame.MaxMissFrames) {

                    // 添加调试日志
                    std::ostringstream oss;
                    oss << "动作 " << roi.orderId
                        << " 结束，记录持续时间 " << (timestamp - state.start_time) << " ms";
                    WriteLog(oss.str());

                    // 动作结束: 保存持续时间记录
                    roi_durations[roi.orderId].push_back(DurationRecord{
                        state.start_time,
                        static_cast<uint64_t>(timestamp /*- state.miss_count * frame.VideoFps*/)
                        });

                    state = ActionState{};
                }
                else {
                    state.miss_count++;
                }
            }
        }

        return 0;
    }


    void CalculateDurations(const PtrVMUnorderedMap<int, PtrVMNodeState>& ptrOutVec)
    {
        //ptrOutVec->vmMap.clear();


        for (const auto& [order_id, records] : roi_durations) // 使用order_id作为键
        {
            uint64_t total_duration = 0;

            // 计算总持续时间
            if (!records.empty()) {
                for (const DurationRecord& dr : records) {
                    uint64_t dur = dr.duration();
                    if (dur > UINT64_MAX - total_duration) {
                        // 处理溢出情况
                        total_duration = UINT64_MAX;
                        break;
                    }
                    total_duration += dur;
                }
            }

            // 确保映射中存在该键
            if (ptrOutVec->vmMap.find(order_id) == ptrOutVec->vmMap.end()) {
                ptrOutVec->vmMap[order_id] = std::make_shared<VMNodeState>();
                ptrOutVec->vmMap[order_id]->nodeState = false;
            }

            if (!ptrOutVec->vmMap[order_id]->nodeState) {
                ptrOutVec->vmMap[order_id]->calssId = order_id;
                ptrOutVec->vmMap[order_id]->timeStamp = total_duration;
                ptrOutVec->vmMap[order_id]->nodeState = (total_duration > 0);
            }
        }
    }
};