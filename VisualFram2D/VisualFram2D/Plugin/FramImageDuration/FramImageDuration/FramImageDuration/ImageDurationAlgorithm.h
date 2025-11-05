#pragma once
#include <opencv2/opencv.hpp>
#include "SerializeStructParam.h"
#include"PublicFunction.h"

struct ROI {
    int id;               
    cv::Rect area;        
    bool state;            
    std::string name;      
    float min_confidence;  
    int orderId;
};

struct DetectedObject {
    int class_id;       
    float confidence;   
    cv::Rect box;       
    bool state;
    int IsPtsBox;     
    cv::Point2d keyPoint;
    std::vector<cv::Point2d> polygonPoints;
    //uint64_t timePoint;
};
using DetectionResults = std::vector<DetectedObject>;  

class ImageDurationAlgorithm
{
public:

    ImageDurationAlgorithm();
    ~ImageDurationAlgorithm();

    using Ptr = std::shared_ptr<ImageDurationAlgorithm>;
    std::vector<ROI>InitRoi(PtrVMUnorderedMap<int, PtrVMVector<VMRectangle>>&roi);
    std::vector<DetectedObject>InitModel(const BaseNodeData::Ptr inputNode, uint64_t &timePoint);

};

