#pragma once
#include <opencv2\opencv.hpp>
#include "SerializeStructParam.h"
#include "GeneralStruct.h"
#include "YoloV11Src/YoloV11_Pose.h"

class YoloV11PoseAlgorithm
{
public:

    YoloV11PoseAlgorithm();
	~YoloV11PoseAlgorithm();

    using Ptr = std::shared_ptr<YoloV11PoseAlgorithm>;

    void SetParam(const FunctionParam& fun_param);

    int RunYoloV11Pose(const cv::Mat& src, const YoloDetector& detector, const PtrVMUnorderedMap<int, PtrVMPolygon>& yoloV11ResultVMUnorderedMap, const FunctionParam& fun_param = FunctionParam());

    const cv::Mat& GetInferImage() {
        return this->yoloV11PoseResultMat;
    }


private:

    FunctionParam functionParam;
    cv::Mat sourceMat;
    cv::Mat yoloV11PoseResultMat;
    YoloDetector yoloDetector;
};

