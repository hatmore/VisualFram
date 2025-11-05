#include "ImageROISelectAlgorithm.h"

ImageROISelectAlgorithm::ImageROISelectAlgorithm()
{

}


ImageROISelectAlgorithm::~ImageROISelectAlgorithm()
{

}

//void ImageROISelectAlgorithm::SetParam(const FunctionParam& fun_param)
//{
//    //functionParam = fun_param;
//}


std::vector<PtrVMRectangle>ImageROISelectAlgorithm::GetThresReuslt()
{
    std::vector<PtrVMRectangle>rectangles;
    for (size_t i = 0; i < MRectangles.size(); i++)
    {
        PtrVMRectangle rect;
        rect->width = MRectangles[i].width;
        rect->height = MRectangles[i].height;
        rectangles.push_back(rect);
    }
    return rectangles;
}


void adjustClassIds(std::vector<int>& classIds, size_t targetSize) {
    if (classIds.size() > targetSize) {
        // ½Ø¶Ï
        classIds.resize(targetSize);
    }
    else if (classIds.size() < targetSize) {
        //È±ÉÙ
        size_t oldSize = classIds.size();
        classIds.resize(targetSize);
        std::fill_n(classIds.begin() + oldSize, targetSize - oldSize, 0);
    }
}

//int ImageROISelectAlgorithm::RunImageRoi(const QList<BaseItem*>rects,std::vector<int>& classIds, std::vector<int>&orderIds,
//    PtrVMUnorderedMap<int, PtrVMVector<VMRectangle>>&ptrMRectangle)
//{
//
//    if (!ptrMRectangle|| rects.empty()){
//        return -1;
//    }
//
//    if (classIds.size() != static_cast<size_t>(rects.size())) {
//        adjustClassIds(classIds, rects.size());  
//    }
//    if (orderIds.size() != static_cast<size_t>(rects.size())) {
//        adjustClassIds(orderIds, rects.size());
//    }
//
//    ptrMRectangle->vmMap.clear();
//
//    int count = 0;
//    for (BaseItem* item : rects) {
//        RectItem* rectItem = dynamic_cast<RectItem*>(item);
//        if (rectItem) {
//            MRectangle mRect;
//            rectItem->GetRect(mRect);
//            VMPoint2d center(mRect.col/* + mRect.width / 2.0*/, mRect.row /*+ mRect.height / 2.0*/);
//            VMRectangle vmRect(mRect.width, mRect.height, center, 0.0f, orderIds[count]);
//            ptrMRectangle->vmMap[classIds[count]] = vmRect;
//            count++;
//        }
//    }
//   return 0;
//}


int ImageROISelectAlgorithm::RunImageRoi(const QList<BaseItem*>rects, std::vector<int>& classIds, std::vector<int>& orderIds,
    PtrVMUnorderedMap<int, PtrVMVector<VMRectangle>>& ptrMRectangle, const FunctionParam& fun_param, bool useInputModelLabels)
{
    if (!ptrMRectangle || rects.empty()) {
        return -1;
    }

    if (classIds.size() != static_cast<size_t>(rects.size())) {
        adjustClassIds(classIds, rects.size());
    }
    if (orderIds.size() != static_cast<size_t>(rects.size())) {
        adjustClassIds(orderIds, rects.size());
    }

    ptrMRectangle->vmMap.clear();

    int count = 0;
    for (BaseItem* item : rects) {
        RectItem* rectItem = dynamic_cast<RectItem*>(item);
        if (rectItem) {
            MRectangle mRect;
            rectItem->GetRect(mRect);
            VMPoint2d center(mRect.col, mRect.row);
            VMRectangle vmRect(mRect.width, mRect.height, center, 0.0f, orderIds[count]);
            if (useInputModelLabels) {
                if (count < fun_param.inputModelLabels.length()) {
                    vmRect.toolName = fun_param.inputModelLabels[orderIds[count]];
                }
            }
            if (ptrMRectangle->vmMap.find(classIds[count]) == ptrMRectangle->vmMap.end()) {
                ptrMRectangle->vmMap[classIds[count]] = std::make_shared<VMVector<VMRectangle>>();
            }
            ptrMRectangle->vmMap[classIds[count]]->vmVector.push_back(vmRect);
            count++;
        }
    }
    return 0;
}