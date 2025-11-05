#include "YoloV11DetectAlgorithm.h"
#include <QtDebug>
#include <unordered_map>

YoloV11DetectAlgorithm::YoloV11DetectAlgorithm()
{
}


YoloV11DetectAlgorithm::~YoloV11DetectAlgorithm()
{
}

void YoloV11DetectAlgorithm::SetParam(const FunctionParam& fun_param)
{
	functionParam = fun_param;
}

int YoloV11DetectAlgorithm::RunYoloV11Detect(const cv::Mat& src, const YoloDetector& detector, 
                   const PtrVMUnorderedMap<int, PtrVMVector<PtrVMRectangle>>& yoloV11ResultVMUnorderedMap, const FunctionParam& fun_param, bool inferResultShow)
{
	if (src.empty()) {
		return -1;
	}

    //yoloV11DetectResultMat = src.clone();
    yoloV11DetectResultMat = src;
    functionParam = fun_param;
    yoloDetector = detector;

    std::vector<Detection> inferenceResult = yoloDetector.inference(yoloV11DetectResultMat, fun_param);
    if (inferResultShow) {
        YoloDetector::draw_image(yoloV11DetectResultMat, inferenceResult, fun_param);
    }

    for (int i = 0; i < yoloV11ResultVMUnorderedMap->vmMap.size(); ++i) {
        yoloV11ResultVMUnorderedMap->vmMap[i]->vmVector.clear();
    }

    //不同类别下目标个数统计
    std::vector<int> classIdCount(functionParam.classNum,0);

    for (int j = 0; j < inferenceResult.size(); ++j) {
        Detection currentDetection = inferenceResult[j];

        int currentClassId = currentDetection.classId;
        if (yoloV11ResultVMUnorderedMap->vmMap[currentClassId]->vmVector.size() <= classIdCount[currentClassId]) {
            auto vmRectangle = std::make_shared <VMRectangle >();
            yoloV11ResultVMUnorderedMap->vmMap[currentClassId]->vmVector.push_back(vmRectangle);
            //yoloV11ResultVMUnorderedMap->vmMap[classIdCount[currentClassId]] = std::make_shared<VMVector<PtrVMRectangle>>();
        }
        yoloV11ResultVMUnorderedMap->vmMap[currentClassId]->vmVector[classIdCount[currentClassId]]->width = (int)(currentDetection.bbox[2] - currentDetection.bbox[0]);
        yoloV11ResultVMUnorderedMap->vmMap[currentClassId]->vmVector[classIdCount[currentClassId]]->height = (int)(currentDetection.bbox[3] - currentDetection.bbox[1]);
        yoloV11ResultVMUnorderedMap->vmMap[currentClassId]->vmVector[classIdCount[currentClassId]]->centerPoint.x = (float)(currentDetection.bbox[0] + (currentDetection.bbox[2] - currentDetection.bbox[0]) / 2);
        yoloV11ResultVMUnorderedMap->vmMap[currentClassId]->vmVector[classIdCount[currentClassId]]->centerPoint.y = (float)(currentDetection.bbox[1] + (currentDetection.bbox[3] - currentDetection.bbox[1]) / 2);
        yoloV11ResultVMUnorderedMap->vmMap[currentClassId]->vmVector[classIdCount[currentClassId]]->calssId = currentClassId;
        yoloV11ResultVMUnorderedMap->vmMap[currentClassId]->vmVector[classIdCount[currentClassId]]->state = 1;
        yoloV11ResultVMUnorderedMap->vmMap[currentClassId]->vmVector[classIdCount[currentClassId]]->confidence = currentDetection.conf;
        classIdCount[currentClassId] ++;
    }

    for (const auto& pair : yoloV11ResultVMUnorderedMap->vmMap) {
        int key = pair.first;
        const auto& vec = pair.second;
        int j = 0;
        for (const auto& rect : vec->vmVector) {
            if (j < classIdCount[key])
            {
                j++;
                continue;
            }
            rect->width = 0;
            rect->height = 0;
            rect->centerPoint.x = 0.0;
            rect->centerPoint.y = 0.0;
            rect->state = false;
            rect->confidence = 0.0;
        }
    }

    return 0;
}