#include"ImageDurationAlgorithm.h"

ImageDurationAlgorithm::ImageDurationAlgorithm()
{
}

ImageDurationAlgorithm::~ImageDurationAlgorithm()
{
}


std::vector<ROI>ImageDurationAlgorithm::InitRoi(PtrVMUnorderedMap<int, PtrVMVector<VMRectangle>>& roi_map)
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
            //roi.area.y = vm_rect.centerPoint.y; // 确认是否需要注释中的代码
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


std::vector<DetectedObject>ImageDurationAlgorithm::InitModel(const BaseNodeData::Ptr inputNode, uint64_t &timePoint)
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
        timePoint=point_map->timeStamp;
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


