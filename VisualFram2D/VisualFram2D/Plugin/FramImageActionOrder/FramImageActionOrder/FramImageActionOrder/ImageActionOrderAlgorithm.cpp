
#include"ImageActionOrderAlgorithm.h"
#include <unordered_set>
#include <QFile>         // 添加
#include <QTextStream>   // 添加
#include <QDateTime>     // 添加
#include <algorithm>    // 添加


ImageActionOrderAlgorithm::ImageActionOrderAlgorithm()
{
}

ImageActionOrderAlgorithm::~ImageActionOrderAlgorithm()
{
}

void ImageActionOrderAlgorithm::SetParam(const std::vector<ROI>&area)
{
    rois = area;
}


float CalculateIoU(const cv::Rect& rect1, const cv::Rect& rect2) {
    
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
    return std::max(unionValue1, unionValue2);
}

std::vector<ROI>ImageActionOrderAlgorithm::InitRoi(PtrVMUnorderedMap<int, PtrVMVector<VMRectangle>>& roi_map)
{
    std::vector<ROI> rois;
    if (!roi_map) {
        return rois;  
    }
    // 遍历输入的ROI map
    for (const auto& vmPair : roi_map->vmMap) {
        PtrVMVector<VMRectangle> vmVector = vmPair.second; // 获取值（vmVector）
        for (size_t j = 0; j < vmVector->vmVector.size(); ++j) {
            VMRectangle vm_rect = vmVector->vmVector[j];
            ROI roi;
            roi.id = vmPair.first; //map的键值：类别

            //roi.area.x = vm_rect.centerPoint.x;
            //roi.area.y = vm_rect.centerPoint.y; 
            //roi.area.width = vm_rect.width;
            //roi.area.height = vm_rect.height;
            //roi.orderId = vm_rect.orderId;
            //rois.push_back(roi);

            // 处理负宽高的情况
            float actualWidth = std::abs(vm_rect.width);
            float actualHeight = std::abs(vm_rect.height);

            roi.area.x = std::min(vm_rect.centerPoint.x, vm_rect.centerPoint.x + vm_rect.width);
            roi.area.y = std::min(vm_rect.centerPoint.y, vm_rect.centerPoint.y + vm_rect.height);
            roi.area.width = actualWidth;
            roi.area.height = actualHeight;
            roi.orderId = vm_rect.orderId;
            rois.push_back(roi);

        }
    }


    return rois;
}


std::vector<DetectedObject>ImageActionOrderAlgorithm::InitModel(const BaseNodeData::Ptr inputNode, uint64_t &timePoint)
{
    std::vector<DetectedObject> rois;  // 准备返回的检测对象列表

// 第一步：识别输入数据类型
    int Idex = 0;
    if (inputNode->GetCGObjectNodeFlow<VMUnorderedMap<int, PtrVMPoint2d>>()) {
        Idex = 1;  // 点类型数据
    }
    else if (inputNode->GetCGObjectNodeFlow<VMUnorderedMap<int, PtrVMVector<PtrVMRectangle>>>()) {
        Idex = 2;  // 矩形类型数据
    }
    else if (inputNode->GetCGObjectNodeFlow<VMUnorderedMap<int, PtrVMPolygon>>()) {
        Idex = 3;  // 多边形类型数据
    }

    // 第二步：根据不同类型进行数据处理
    switch (Idex)
    {
    case 1:  // 处理点数据
    {
        auto point_map = inputNode->GetCGObjectNodeFlow<VMUnorderedMap<int, PtrVMPoint2d>>();
        timePoint = point_map->timeStamp;
        if (!point_map) break;

        for (const auto& [id, point] : point_map->vmMap)
        {
            DetectedObject roi;
            roi.class_id = id;
            roi.IsPtsBox = false;
            roi.state = point->state;
            //roi.timePoint=point.timeStamp;
            roi.keyPoint = cv::Point2d(point->x, point->y);
            rois.push_back(roi);
        }
        break;
    }
    case 2:  // 处理矩形数据
    {
        auto rect_map = inputNode->GetCGObjectNodeFlow<VMUnorderedMap<int, PtrVMVector<PtrVMRectangle>>>();
        timePoint = rect_map->timeStamp;
        if (!rect_map) break;
        for (size_t i = 0; i < rect_map->vmMap.size(); i++) {
            PtrVMVector<PtrVMRectangle> vmVector = rect_map->vmMap[i];
            for (size_t i = 0; i < vmVector->vmVector.size(); i++) {
                PtrVMRectangle rect = vmVector->vmVector[i];
                DetectedObject roi;
                roi.class_id = rect->calssId;
                roi.confidence = rect->confidence;
                roi.IsPtsBox = true;
                roi.state = rect->state;
                roi.box.x = rect->centerPoint.x - rect->width / 2.0f;
                roi.box.y = rect->centerPoint.y - rect->height / 2.0f;
                roi.box.width = rect->width;
                roi.box.height = rect->height;
                rois.push_back(roi);
            }
        }
        break;
    }
    case 3:  // 处理多边形数据
    {
        auto polygon_map = inputNode->GetCGObjectNodeFlow<VMUnorderedMap<int, PtrVMPolygon>>();
        timePoint = polygon_map->timeStamp;
        if (!polygon_map) break;

        for (const auto& [id, polygon] : polygon_map->vmMap)
        {
            if (!polygon->state) continue;

            DetectedObject roi;
            roi.class_id = id;
            roi.IsPtsBox = 2;  // 2表示多边形
            roi.state = polygon->state;
            roi.polygonPoints.clear();
            for (const auto& pt : polygon->vectorPoints)
            {
                roi.polygonPoints.emplace_back(pt.x, pt.y);
            }
            rois.push_back(roi);
        }
        break;
    }
    default:  // 未知类型不做处理
        break;
    }

    return rois;
}


int ImageActionOrderAlgorithm::RunProcessFrame(PtrVMUnorderedMap<int, PtrVMNodeState>& ptrOutVec,
    const DetectionResults& detections, uint64_t timestamp, float referIouValue)
{
    if (detections.empty()) {
        return -1;
    }

    int detectionType = detections[0].IsPtsBox;

    // 存储当前帧每个orderId是否被检测到
    std::unordered_set<int> detectedOrderIds;

    switch (detectionType) {
    case 0: // 处理关键点
    {
        std::unordered_map<int, int> orderIdPointCounts; // orderId -> 包含的点数

        for (const auto& detection : detections) {
            for (const ROI& roi : rois) {
                if (roi.area.contains(detection.keyPoint) && detection.state) {
                    orderIdPointCounts[roi.orderId]++;
                    break; // 同一个点只要在一个ROI中计数一次
                }
            }
        }

        for (const auto& [orderId, count] : orderIdPointCounts) {
            if (count >= referIouValue) {
                detectedOrderIds.insert(orderId);
            }
        }
        break;
    }
    case 1: // 处理检测框
    {
        //// 只为class 0打印详细匹配日志
        //bool hasClass0 = false;
        //for (const auto& detection : detections) {
        //    if (detection.class_id == 0) {
        //        hasClass0 = true;
        //        break;
        //    }
        //}
        //if (hasClass0) {
        //    // 调试：打印所有ROI信息
        //    WriteLog(QString("[调试-ROI信息] 共有 %1 个ROI区域").arg(rois.size()));
        //    for (const ROI& roi : rois) {
        //        if (roi.id == 0) {
        //            WriteLog(QString("[调试-ROI] orderId=%1 classId=%2 位置=(x:%3, y:%4, w:%5, h:%6)")
        //                .arg(roi.orderId)
        //                .arg(roi.id)
        //                .arg(roi.area.x)
        //                .arg(roi.area.y)
        //                .arg(roi.area.width)
        //                .arg(roi.area.height));
        //        }
        //    }
        //    // 调试：打印类0的检测信息
        //    WriteLog(QString("[调试-检测] 类0的检测:"));
        //    for (size_t i = 0; i < detections.size(); i++) {
        //        const auto& detection = detections[i];
        //        if (detection.class_id == 0) {
        //            WriteLog(QString("  [检测%1] state=%2 位置=(x:%3, y:%4, w:%5, h:%6)")
        //                .arg(i)
        //                .arg(detection.state ? "有效" : "无效")
        //                .arg(detection.box.x)
        //                .arg(detection.box.y)
        //                .arg(detection.box.width)
        //                .arg(detection.box.height));
        //        }
        //    }
        //    WriteLog(QString("[调试-阈值] IoU阈值 = %1 / 10 = %2")
        //        .arg(referIouValue)
        //        .arg(referIouValue / 10.0f));

        //    WriteLog(QString("[调试-当前进度] 当前已记录到动作 %1，下一个应该检测动作 %2")
        //        .arg(highestRecordedOrderId)
        //        .arg(highestRecordedOrderId + 1));
        //}

        // 为每个检测框找到所有匹配的ROI，但只选择符合顺序的
        for (const auto& detection : detections) {
            // 存储当前检测框所有匹配的ROI及其IoU
            struct RoiMatch {
                int orderId;
                float iou;
            };
            std::vector<RoiMatch> matches;

            for (const ROI& roi : rois) {
                if (detection.class_id == roi.id && detection.state) {
                    float iouValue = CalculateIoU(detection.box, roi.area);

                    //// 只为类0打印详细匹配日志
                    //if (detection.class_id == 0 && roi.id == 0) {
                    //    WriteLog(QString("[调试-匹配] 检测 vs ROI(orderId=%1)")
                    //        .arg(roi.orderId));
                    //    WriteLog(QString("  - IoU: %1 > %2 ? %3")
                    //        .arg(iouValue)
                    //        .arg(referIouValue / 10.0f)
                    //        .arg(iouValue > referIouValue / 10.0f ? "是" : "否"));
                    //}

                    if (iouValue > referIouValue / 10.0f) {
                        matches.push_back({ roi.orderId, iouValue });
                    }
                }
            }

            // 如果有多个匹配，只考虑下一个顺序的动作
            if (!matches.empty()) {
                // 1. 先查找所有ROI中最大的orderId（获取总动作数）
                int maxOrderId = 0;
                for (const ROI& roi : rois) {
                    if (roi.orderId > maxOrderId) {
                        maxOrderId = roi.orderId;
                    }
                }


                // 2. 计算期待的orderId（此时highestRecordedOrderId可能已经被重置为0）
                int nextExpectedOrderId = highestRecordedOrderId + 1;

                // 3. 查找是否有符合顺序的匹配
                auto it = std::find_if(matches.begin(), matches.end(),
                    [nextExpectedOrderId](const RoiMatch& match) {
                        return match.orderId == nextExpectedOrderId;
                    });

                if (it != matches.end()) {
                    // 找到了下一个顺序的动作
                    detectedOrderIds.insert(it->orderId);

                    if (detection.class_id == 0) {
                        WriteLog(QString("  检测到下一个顺序动作: orderId=%1 IoU=%2")
                            .arg(it->orderId)
                            .arg(it->iou));
                    }
                }
                else {
                    // 没有找到下一个顺序的动作，记录被忽略的匹配
                    if (detection.class_id == 0 && !matches.empty()) {
                        QString ignoredList;
                        for (const auto& match : matches) {
                            ignoredList += QString("orderId=%1(IoU=%2) ")
                                .arg(match.orderId)
                                .arg(match.iou);
                        }
                        WriteLog(QString("  跳过非顺序匹配: %1 (期待orderId=%2, 最大orderId=%3)")
                            .arg(ignoredList)
                            .arg(nextExpectedOrderId)
                            .arg(maxOrderId));
                    }
                }
            }
        }
        //if (hasClass0) {
        //    WriteLog("----------------------------------------");
        //}
        break;
    }
    case 2: // 处理多边形点
    {
        std::unordered_map<int, int> orderIdPolygonCounts; // orderId -> 包含的多边形点数

        for (const auto& detection : detections) {
            if (detection.state && !detection.polygonPoints.empty()) {
                for (const ROI& roi : rois) {
                    int pointsInside = 0;
                    for (const auto& point : detection.polygonPoints) {
                        if (roi.area.contains(point)) {
                            pointsInside++;
                        }
                    }
                    if (pointsInside > 0) {
                        orderIdPolygonCounts[roi.orderId] += pointsInside;
                        break; // 同一个多边形只要在一个ROI中计数
                    }
                }
            }
        }

        for (const auto& [orderId, count] : orderIdPolygonCounts) {
            if (count >= referIouValue) {
                detectedOrderIds.insert(orderId);
            }
        }
        break;
    }
    default:
        break;
    }

    // 更新每个orderId的最近三帧检测状态并记录首次出现的时间戳
    for (int orderId : detectedOrderIds) {
        // 初始化检测历史（如果不存在）
        if (roiDetectionHistory.find(orderId) == roiDetectionHistory.end()) {
            roiDetectionHistory[orderId] = std::deque<bool>(3, false);
        }

        // 更新检测状态
        roiDetectionHistory[orderId].pop_front();
        roiDetectionHistory[orderId].push_back(true);
 

        // 检查最近三帧中是否有任意2帧检测到
        bool anyDetectedInLastThreeFrames = false;
        int judgeCount = 0;
        for (bool frameDetected : roiDetectionHistory[orderId]) {
            if (frameDetected) {
                judgeCount++;
                if (judgeCount >= 2) {
                    anyDetectedInLastThreeFrames = true;
                    break;
                }
            }
        }


        // 如果满足条件，并且是首次触发，则记录时间戳
        if (anyDetectedInLastThreeFrames) {
            // 确保VMVector存在
            if (ptrOutVec->vmMap.find(orderId) == ptrOutVec->vmMap.end()) {
                ptrOutVec->vmMap[orderId] = std::make_shared<VMNodeState>();
                ptrOutVec->vmMap[orderId]->nodeState = false;
            }

            if (!ptrOutVec->vmMap[orderId]->nodeState) {
                ptrOutVec->vmMap[orderId]->calssId = orderId;
                ptrOutVec->vmMap[orderId]->timeStamp = timestamp;
                ptrOutVec->vmMap[orderId]->nodeState = true;
            }

            // 更新已记录的最高orderId
            if (orderId > highestRecordedOrderId) {
                highestRecordedOrderId = orderId;
            }

            // 记录首次出现
            //first_occurrences[orderId] = { orderId, orderId, timestamp, true };


        }
    }

    // 对于未检测到的orderId，更新检测历史（保持false状态）
    for (auto& [orderId, detectionHistory] : roiDetectionHistory) {
        if (detectedOrderIds.find(orderId) == detectedOrderIds.end()) {
            detectionHistory.pop_front();
            detectionHistory.push_back(false);
        }
    }

    return 0;
}

void ImageActionOrderAlgorithm::WriteLog(const QString& message, LOGTYPE logType)
{
    if (ptrQueueNodeLogData != nullptr) {
        std::pair<LOGTYPE, QString> p_log_info;
        p_log_info.first = logType;
        p_log_info.second = message;
        ptrQueueNodeLogData->push(p_log_info);

        // 2. 同时写入文件（持久化保存）
        static QFile logFile("D:/app/TestData/action_order_debug.txt");
        static bool fileOpened = false;

        if (!fileOpened) {
            if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
                fileOpened = true;
            }
        }

        if (fileOpened) {
            QTextStream out(&logFile);
            out.setCodec("UTF-8");  // 确保中文正常显示

            // 添加时间戳
            QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
            out << "[" << timestamp << "] " << message << "\n";
            out.flush();  // 立即刷新到磁盘
        }
    }
}