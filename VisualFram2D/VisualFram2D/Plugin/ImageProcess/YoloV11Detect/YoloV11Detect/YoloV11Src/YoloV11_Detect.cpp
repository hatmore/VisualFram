#include <iostream>
#include <fstream>

#include <NvOnnxParser.h>

#include "YoloV11_Detect.h"
#include "preprocess.h"
#include "postprocess.h"
#include "calibrator.h"
#include "utils.h"

using namespace nvinfer1;

YoloDetector::YoloDetector()
{ }

int YoloDetector::LoadEngineFromFile(const std::string& trt_file, const FunctionParam& fun_param)
{
    trtFile_ = trt_file;
    gLogger = Logger(ILogger::Severity::kERROR);
    cudaSetDevice(kGpuId);

    CHECK(cudaStreamCreate(&stream));

    // load engine
    int res = get_engine();
    if (res != 0) {
        return -1;
    }

    context = engine->createExecutionContext();
    context->setBindingDimensions(0, Dims32{ 4, {1, 3, fun_param.kInputH, fun_param.kInputW} });

    // get engine output info
    Dims32 outDims = context->getBindingDimensions(1);  // [1, 84, 8400]
    OUTPUT_CANDIDATES = outDims.d[2];  // 8400
    int outputSize = 1;  // 84 * 8400
    for (int i = 0; i < outDims.nbDims; i++) {
        outputSize *= outDims.d[i];
    }

    // prepare output data space on host
    outputData = new float[1 + kMaxNumOutputBbox * kNumBoxElement];
    // prepare input and output space on device
    vBufferD.resize(2, nullptr);
    CHECK(cudaMalloc(&vBufferD[0], 3 * fun_param.kInputH * fun_param.kInputW * sizeof(float)));
    CHECK(cudaMalloc(&vBufferD[1], outputSize * sizeof(float)));

    CHECK(cudaMalloc(&transposeDevice, outputSize * sizeof(float)));
    CHECK(cudaMalloc(&decodeDevice, (1 + kMaxNumOutputBbox * kNumBoxElement) * sizeof(float)));

    return 0;
}

int YoloDetector::get_engine() {
    std::ifstream engineFile(trtFile_, std::ios::binary);
    long int fsize = 0;

    engineFile.seekg(0, engineFile.end);
    fsize = engineFile.tellg();
    engineFile.seekg(0, engineFile.beg);
    if (fsize <= 0) {
        return -1;
    }
    std::vector<char> engineString(fsize);
    engineFile.read(engineString.data(), fsize);
    if (engineString.size() == 0) { std::cout << "Failed getting serialized engine!" << std::endl; return -1; }
    std::cout << "Succeeded getting serialized engine!" << std::endl;

    runtime = createInferRuntime(gLogger);
    engine = runtime->deserializeCudaEngine(engineString.data(), fsize);
    if (engine == nullptr) { std::cout << "Failed loading engine!" << std::endl; return -1; }
    std::cout << "Succeeded loading engine!" << std::endl;
    return 0;
}

void YoloDetector::BuildOnnx2TrT(const std::string& onnx_path, const std::string& trt_path)
{
    /*IBuilder* builder = createInferBuilder(gLogger);
    INetworkDefinition* network = builder->createNetworkV2(1U << int(NetworkDefinitionCreationFlag::kEXPLICIT_BATCH));
    IOptimizationProfile* profile = builder->createOptimizationProfile();
    IBuilderConfig* config = builder->createBuilderConfig();
    config->setMaxWorkspaceSize(1 << 30);
    IInt8Calibrator* pCalibrator = nullptr;
    if (bFP16Mode) {
        config->setFlag(BuilderFlag::kFP16);
    }
    if (bINT8Mode) {
        config->setFlag(BuilderFlag::kINT8);
        int batchSize = 8;
        pCalibrator = new Int8EntropyCalibrator2(batchSize, kInputW, kInputH, calibrationDataPath.c_str(), cacheFile.c_str());
        config->setInt8Calibrator(pCalibrator);
    }

    nvonnxparser::IParser* parser = nvonnxparser::createParser(*network, gLogger);
    if (!parser->parseFromFile(onnx_path.c_str(), int(gLogger.reportableSeverity))) {
        std::cout << std::string("Failed parsing .onnx file!") << std::endl;
        for (int i = 0; i < parser->getNbErrors(); ++i) {
            auto* error = parser->getError(i);
            std::cout << std::to_string(int(error->code())) << std::string(":") << std::string(error->desc()) << std::endl;
        }
        return;
    }
    std::cout << std::string("Succeeded parsing .onnx file!") << std::endl;

    ITensor* inputTensor = network->getInput(0);
    profile->setDimensions(inputTensor->getName(), OptProfileSelector::kMIN, Dims32{ 4, {1, 3, kInputH, kInputW} });
    profile->setDimensions(inputTensor->getName(), OptProfileSelector::kOPT, Dims32{ 4, {1, 3, kInputH, kInputW} });
    profile->setDimensions(inputTensor->getName(), OptProfileSelector::kMAX, Dims32{ 4, {1, 3, kInputH, kInputW} });
    config->addOptimizationProfile(profile);

    IHostMemory* engineString = builder->buildSerializedNetwork(*network, *config);
    std::cout << "Succeeded building serialized engine!" << std::endl;

    runtime = createInferRuntime(gLogger);
    engine = runtime->deserializeCudaEngine(engineString->data(), engineString->size());
    if (engine == nullptr) { std::cout << "Failed building engine!" << std::endl; return; }
    std::cout << "Succeeded building engine!" << std::endl;

    if (bINT8Mode && pCalibrator != nullptr) {
        delete pCalibrator;
    }

    std::ofstream engineFile(trt_path, std::ios::binary);
    engineFile.write(static_cast<char*>(engineString->data()), engineString->size());
    std::cout << "Succeeded saving .plan file!" << std::endl;

    delete engineString;
    delete parser;
    delete config;
    delete network;
    delete builder;*/
}

YoloDetector::~YoloDetector() {
    if (engine == nullptr)
        return;
    cudaStreamDestroy(stream);

    for (int i = 0; i < 2; ++i)
    {
        CHECK(cudaFree(vBufferD[i]));
    }

    CHECK(cudaFree(transposeDevice));
    CHECK(cudaFree(decodeDevice));

    delete[] outputData;

    delete context;
    delete engine;
    delete runtime;
}

std::vector<Detection> YoloDetector::inference(cv::Mat& img, const FunctionParam& fun_param) {
    if (img.empty()) return {};

    // put input on device, then letterbox、bgr to rgb、hwc to chw、normalize.
    preprocess(img, (float*)vBufferD[0], fun_param.kInputH, fun_param.kInputW, stream);

    // tensorrt inference
    context->enqueueV2(vBufferD.data(), stream, nullptr);

    // transpose [1 84 8400] convert to [1 8400 84]
    transpose((float*)vBufferD[1], transposeDevice, OUTPUT_CANDIDATES, fun_param.classNum + 4, stream);
    // convert [1 8400 84] to [1 7001]
    decode(transposeDevice, decodeDevice, OUTPUT_CANDIDATES, fun_param.classNum, fun_param.kConfThresh, kMaxNumOutputBbox, kNumBoxElement, stream);
    // cuda nms
    nms(decodeDevice, kNmsThresh, kMaxNumOutputBbox, kNumBoxElement, stream);

    CHECK(cudaMemcpyAsync(outputData, decodeDevice, (1 + kMaxNumOutputBbox * kNumBoxElement) * sizeof(float), cudaMemcpyDeviceToHost, stream));
    cudaStreamSynchronize(stream);

    std::vector<Detection> vDetections;
    int count = (std::min)((int)outputData[0], kMaxNumOutputBbox);
    for (int i = 0; i < count; i++) {
        int pos = 1 + i * kNumBoxElement;
        int keepFlag = (int)outputData[pos + 6];
        if (keepFlag == 1) {
            Detection det;
            memcpy(det.bbox, &outputData[pos], 4 * sizeof(float));
            det.conf = outputData[pos + 4];
            det.classId = (int)outputData[pos + 5];
            vDetections.push_back(det);
        }
    }

    for (size_t j = 0; j < vDetections.size(); j++) {
        scale_bbox(img, vDetections[j].bbox, fun_param.kInputH, fun_param.kInputW);
    }

    return vDetections;
}

void YoloDetector::draw_image(cv::Mat& img, std::vector<Detection>& inferResult, const FunctionParam& fun_param) {
    // draw inference result on image
    for (size_t i = 0; i < inferResult.size(); i++) {
        cv::Scalar bboxColor(get_random_int(), get_random_int(), get_random_int());
        cv::Rect r(
            round(inferResult[i].bbox[0]),
            round(inferResult[i].bbox[1]),
            round(inferResult[i].bbox[2] - inferResult[i].bbox[0]),
            round(inferResult[i].bbox[3] - inferResult[i].bbox[1])
        );
        cv::rectangle(img, r, bboxColor, 2);

        std::vector<std::string> vClassNames = {};
        if (fun_param.inputModelLabels.size() > 0) {
            for (const auto& label : fun_param.inputModelLabels) {
                if (!label.isEmpty()) {
                    vClassNames.push_back(label.toStdString());
                }
            }
        }
        else {
            for (int i = 0; i < fun_param.classNum; ++i) {
                vClassNames.push_back("class" + QString::number(i).toStdString());
            }
        }

        std::string className = vClassNames[(int)inferResult[i].classId];
        std::string labelStr = className + " " + std::to_string(inferResult[i].conf).substr(0, 4);

        cv::Size textSize = cv::getTextSize(labelStr, cv::FONT_HERSHEY_PLAIN, 1.2, 2, NULL);
        cv::Point topLeft(r.x, r.y - textSize.height - 3);
        cv::Point bottomRight(r.x + textSize.width, r.y);
        cv::rectangle(img, topLeft, bottomRight, bboxColor, -1);
        cv::putText(img, labelStr, cv::Point(r.x, r.y - 2), cv::FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(255, 255, 255), 2, cv::LINE_AA);
    }
}
