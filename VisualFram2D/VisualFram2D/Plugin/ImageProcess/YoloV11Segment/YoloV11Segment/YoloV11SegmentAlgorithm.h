#pragma once
#include <opencv2\opencv.hpp>
#include "SerializeStructParam.h"
#include "GeneralStruct.h"
#include "YoloV11Src/YoloV11_Segment.h"

class YoloV11SegmentAlgorithm
{
public:

    YoloV11SegmentAlgorithm();
	~YoloV11SegmentAlgorithm();

    using Ptr = std::shared_ptr<YoloV11SegmentAlgorithm>;

    void SetParam(const FunctionParam& fun_param);

    int RunYoloV11Segment(const cv::Mat& src, const YoloDetector& detector, const PtrVMUnorderedMap<int, PtrVMPolygon>& yoloV11ResultVMUnorderedMap, const FunctionParam& fun_param = FunctionParam());

    const cv::Mat& GetInferImage() {
        return this->yoloV11SegmentResultMat;
    }


private:

    FunctionParam functionParam;
    cv::Mat sourceMat;
    cv::Mat yoloV11SegmentResultMat;
    YoloDetector yoloDetector;
};

