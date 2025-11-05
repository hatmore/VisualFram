#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>


const int kGpuId = 0;
//const int kNumClass = 5;
//const int kInputH = 640;
//const int kInputW = 640;
const float kNmsThresh = 0.45f;
//const float kConfThresh = 0.5f;
const int kMaxNumOutputBbox = 500;  // assume the box outputs no more than kMaxNumOutputBbox boxes that conf >= kNmsThresh;
const int kNumBoxElement = 7;  // left, top, right, bottom, confidence, class, keepflag(whether drop when NMS)


// for FP16 mode
const bool bFP16Mode = true;
// for INT8 mode
const bool bINT8Mode = false;
const std::string cacheFile = "./int8.cache";
const std::string calibrationDataPath = "../calibrator";  // 存放用于 int8 量化校准的图像

//const std::vector<std::string> vClassNames {
//    "equipmentShell", "hand", "oilPlug", "ribbon", "cutRibbon"
//};

#endif  // CONFIG_H
