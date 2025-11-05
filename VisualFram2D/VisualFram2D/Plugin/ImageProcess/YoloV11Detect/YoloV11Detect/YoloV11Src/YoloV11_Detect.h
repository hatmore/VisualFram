#ifndef YOLOV11_DETECT_H
#define YOLOV11_DETECT_H

#include <opencv2/opencv.hpp>
#include "../SerializeStructParam.h"
#include "public.h"
#include "config.h"
#include "types.h"

using namespace nvinfer1;



class YoloDetector
{
public:
    YoloDetector();
    ~YoloDetector();

    int LoadEngineFromFile(const std::string& trt_path, const FunctionParam& fun_param);
    void BuildOnnx2TrT(const std::string& onnx_path, const std::string& trt_path);

    std::vector<Detection> inference(cv::Mat& img, const FunctionParam& fun_param);
    static void draw_image(cv::Mat& img, std::vector<Detection>& inferResult, const FunctionParam& fun_param);

private:
    int get_engine();

private:
    Logger              gLogger;
    std::string         trtFile_;

    int                 numClass_;
    float               nmsThresh_;
    float               confThresh_;

    ICudaEngine* engine{ nullptr };
    IRuntime* runtime{ nullptr };
    IExecutionContext* context{ nullptr };

    cudaStream_t        stream;

    float* outputData{ nullptr };
    std::vector<void*> vBufferD;
    float* transposeDevice{ nullptr };
    float* decodeDevice{ nullptr };

    int                 OUTPUT_CANDIDATES;  // 8400: 80 * 80 + 40 * 40 + 20 * 20
};

#endif  // YOLOV11_DETECT_H
