#include "YoloV11PoseAlgorithm.h"
#include <QtDebug>

YoloV11PoseAlgorithm::YoloV11PoseAlgorithm()
{
}


YoloV11PoseAlgorithm::~YoloV11PoseAlgorithm()
{
}

void YoloV11PoseAlgorithm::SetParam(const FunctionParam& fun_param)
{
	functionParam = fun_param;
}

int YoloV11PoseAlgorithm::RunYoloV11Pose(const cv::Mat& src, const YoloDetector& detector, const PtrVMUnorderedMap<int, PtrVMPolygon>& yoloV11ResultVMUnorderedMap, const FunctionParam& fun_param)
{
	if (src.empty()) {
		return -1;
	}
    //yoloV11PoseResultMat = src.clone();
    yoloV11PoseResultMat = src;
    functionParam = fun_param;
    yoloDetector = detector;
    if (yoloV11ResultVMUnorderedMap != nullptr) {
        yoloV11ResultVMUnorderedMap->vmMap.clear();
    }

    std::vector<Detection> resultDetection = yoloDetector.inference(yoloV11PoseResultMat);
    YoloDetector::draw_image(yoloV11PoseResultMat, resultDetection);
    for (int i = 0; i < resultDetection.size(); ++i) {
        if (yoloV11ResultVMUnorderedMap->vmMap.find(i) == yoloV11ResultVMUnorderedMap->vmMap.end()) {
            yoloV11ResultVMUnorderedMap->vmMap[i] = std::make_shared<VMPolygon>();
        }
        yoloV11ResultVMUnorderedMap->vmMap[i]->calssId = 0;
        Detection currentDetection = resultDetection[i];
        yoloV11ResultVMUnorderedMap->vmMap[i]->confidence = currentDetection.conf;
        for (int j = 0; j < currentDetection.vKpts.size(); ++j) {
            VMPoint2d vmPoint2d;
            vmPoint2d.x = currentDetection.vKpts[j][0];
            vmPoint2d.y = currentDetection.vKpts[j][1];
            vmPoint2d.confidence = currentDetection.vKpts[j][2];
            vmPoint2d.calssId = j;
            if (vmPoint2d.confidence > 0.5) {
                vmPoint2d.state = true;
            }
            else {
                vmPoint2d.state = false;
            }
            yoloV11ResultVMUnorderedMap->vmMap[i]->vectorPoints.push_back(vmPoint2d);
        }
    }


    return 0;
}