#pragma once
#include <opencv2/opencv.hpp>
#include "SerializeStructParam.h"
#include "PublicFunction.h"
#include "ImageView/QGraphicsViews2d.h"
#include"ImageWithRect.h"

class ImageROISelectAlgorithm
{
public:

    ImageROISelectAlgorithm();
    ~ImageROISelectAlgorithm();

    using Ptr = std::shared_ptr<ImageROISelectAlgorithm>;
    //void SetParam(const FunctionParam& fun_param);
    int RunImageRoi(const QList<BaseItem*>rects, std::vector<int>& classIds, std::vector<int>& orderIds,
        PtrVMUnorderedMap<int, PtrVMVector<VMRectangle>>& ptrMRectangle, const FunctionParam& fun_param, bool useInputModelLabels);
    std::vector<PtrVMRectangle> GetThresReuslt();
    
private:
    std::vector<MRectangle>MRectangles;
};


