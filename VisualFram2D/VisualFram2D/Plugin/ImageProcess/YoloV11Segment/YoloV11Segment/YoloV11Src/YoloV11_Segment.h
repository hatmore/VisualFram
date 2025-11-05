#ifndef YOLOV11_SEGMENT_H
#define YOLOV11_SEGMENT_H

#include <opencv2/opencv.hpp>
#include "public.h"
#include "types.h"
#include "config.h"

using namespace nvinfer1;


class YoloDetector
{
public:
    YoloDetector();
    ~YoloDetector();

    void LoadEngineFromFile(const std::string& trt_path);
    void BuildOnnx2TrT(const std::string& onnx_path, const std::string& trt_path);


    std::vector<Detection> inference(cv::Mat& img);
    static void draw_image(cv::Mat& img, std::vector<Detection>& inferResult, bool drawBbox = true);

private:
    void get_engine();
    static void process_mask(
        float* protoDevice, Dims32 protoOutDims, std::vector<Detection>& vDetections,
        int kInputH, int kInputW, cv::Mat& img, cudaStream_t stream
    );

private:
    Logger              gLogger;
    std::string         trtFile_;

    ICudaEngine* engine { nullptr };
    IRuntime* runtime{ nullptr };
    IExecutionContext* context{ nullptr };

    cudaStream_t        stream;

    float* outputData{ nullptr };
    std::vector<void*> vBufferD;
    float* transposeDevide{ nullptr };
    float* decodeDevice{ nullptr };

    int                 OUTPUT_CANDIDATES;  // 8400: 80 * 80 + 40 * 40 + 20 * 20
    Dims32              protoOutDims;  // proto shape [1 32 160 160]
};

#endif  // YOLOV11_SEGMENT_H