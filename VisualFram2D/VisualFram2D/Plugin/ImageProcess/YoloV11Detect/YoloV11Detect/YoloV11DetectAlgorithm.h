#pragma once
#include <opencv2\opencv.hpp>
#include "SerializeStructParam.h"
#include "GeneralStruct.h"
#include "YoloV11Src/YoloV11_Detect.h"

class YoloV11DetectAlgorithm
{
public:

    YoloV11DetectAlgorithm();
	~YoloV11DetectAlgorithm();

    using Ptr = std::shared_ptr<YoloV11DetectAlgorithm>;

    void SetParam(const FunctionParam& fun_param);

    int RunYoloV11Detect(const cv::Mat& src, const YoloDetector& detector, const PtrVMUnorderedMap<int, PtrVMVector<PtrVMRectangle>>& yoloV11ResultVMUnorderedMap, const FunctionParam& fun_param, bool inferResultShow);

    const cv::Mat& GetInferImage() {
        return this->yoloV11DetectResultMat;
    }


private:

    FunctionParam functionParam;
    cv::Mat sourceMat;
    cv::Mat yoloV11DetectResultMat;
    PtrVMRectangle vmRectangle;
    PtrVMRectangle vmRectangleInitial;
    YoloDetector yoloDetector;
};

