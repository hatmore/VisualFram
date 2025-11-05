#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>


struct Detection
{
    // x1, y1, x2, y2
    float bbox[4];
    float conf;
    int classId;
    float mask[32];  // mask coefficient
    std::vector<float> maskMatrix;  // 2D mask after mask coefficient multiply proto, and scale to original image
    std::vector<cv::Point> polygon; // 多边形坐标（类似于 result.masks.xy）
};


#endif  // TYPES_H
