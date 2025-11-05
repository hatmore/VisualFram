#include "YoloV11SegmentAlgorithm.h"
#include <QtDebug>

YoloV11SegmentAlgorithm::YoloV11SegmentAlgorithm()
{
}


YoloV11SegmentAlgorithm::~YoloV11SegmentAlgorithm()
{
}

void YoloV11SegmentAlgorithm::SetParam(const FunctionParam& fun_param)
{
	functionParam = fun_param;
}

int YoloV11SegmentAlgorithm::RunYoloV11Segment(const cv::Mat& src, const YoloDetector& detector, const PtrVMUnorderedMap<int, PtrVMPolygon>& yoloV11ResultVMUnorderedMap, const FunctionParam& fun_param)
{
	if (src.empty()) {
		return -1;
	}
    yoloV11SegmentResultMat = src;
    functionParam = fun_param;
    yoloDetector = detector;
    if (yoloV11ResultVMUnorderedMap != nullptr) {
        yoloV11ResultVMUnorderedMap->vmMap.clear();
    }
    
    std::vector<Detection> resultDetection = yoloDetector.inference(yoloV11SegmentResultMat);
    YoloDetector::draw_image(yoloV11SegmentResultMat, resultDetection);
    for (int i = 0; i < resultDetection.size(); ++i) {
        Detection currentDetection = resultDetection[i];
        VMPoint2d vmPoint2d;
        if (yoloV11ResultVMUnorderedMap->vmMap.find(currentDetection.classId) == yoloV11ResultVMUnorderedMap->vmMap.end()) {
            yoloV11ResultVMUnorderedMap->vmMap[currentDetection.classId] = std::make_shared<VMPolygon>();
        }
        yoloV11ResultVMUnorderedMap->vmMap[currentDetection.classId]->calssId = currentDetection.classId;
        yoloV11ResultVMUnorderedMap->vmMap[currentDetection.classId]->state = 1;
        yoloV11ResultVMUnorderedMap->vmMap[currentDetection.classId]->confidence = currentDetection.conf;
        for (int j = 0; j < currentDetection.polygon.size(); ++j) {
            vmPoint2d.x = currentDetection.polygon[j].x;
            vmPoint2d.y = currentDetection.polygon[j].y;
            yoloV11ResultVMUnorderedMap->vmMap[currentDetection.classId]->vectorPoints.push_back(vmPoint2d);
        }
    }

    for (int i = 0; i < functionParam.classNum; ++i) {
        if (yoloV11ResultVMUnorderedMap->vmMap.count(i) > 0) {
            continue;
        }
        else {
            if (yoloV11ResultVMUnorderedMap->vmMap.find(i) == yoloV11ResultVMUnorderedMap->vmMap.end()) {
                yoloV11ResultVMUnorderedMap->vmMap[i] = std::make_shared<VMPolygon>();
            }
            yoloV11ResultVMUnorderedMap->vmMap[i]->calssId = i;
            yoloV11ResultVMUnorderedMap->vmMap[i]->state = 0;
        }
    }

    return 0;
}