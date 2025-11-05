#include "PoseC3DAlgorithm.h"
#include "PoseC3DSrc/PoseC3D.h"
#include <QtDebug>

PoseC3DAlgorithm::PoseC3DAlgorithm()
{}


PoseC3DAlgorithm::~PoseC3DAlgorithm()
{
}

void PoseC3DAlgorithm::SetParam(const FunctionParam& fun_param)
{
	functionParam = fun_param;
}

int PoseC3DAlgorithm::RunPoseC3D(const PoseC3DDetector& detector, const FunctionParam& fun_param)
{
    
    /*functionParam = fun_param;
    poseC3DDetector = detector;
    poseC3DResultVMUnorderedMap = std::make_shared<VMUnorderedMap<int, PtrVMPolygon>>();
    if (poseC3DResultVMUnorderedMap != nullptr) {
        poseC3DResultVMUnorderedMap->vmMap.clear();
    }*/


    //try {
    //    // 图像尺寸参数
    //    const int imgHeight = 480;  // 原始图像高度
    //    const int imgWidth = 853;   // 原始图像宽度
    //    const int modelInputHeight = 128;  // 模型输入高度
    //    const int modelInputWidth = 128;   // 模型输入宽度
    //    const int numFrames = 48;   // 模型要求的帧数
    //    const int numJoints = 17;   // 关节点数量

    //    // 创建测试输入数据
    //    float* inputData = new float[poseC3DDetector.context->inputSize];
    //    memset(inputData, 0, poseC3DDetector.context->inputSize * sizeof(float));

    //    // 示例poseResults数据 - 48帧骨骼点数据
    //    std::vector<std::vector<std::vector<float>>> poseResults(numFrames);

    //    std::vector<std::vector<float>> frameData1 = {
    //        {200.5, 120.3}, {220.1, 110.7}, {180.8, 115.2}, {250.3, 125.6}, {150.7, 130.1},
    //        {300.2, 210.5}, {120.4, 220.8}, {400.6, 320.4}, {80.3, 310.7}, {550.9, 330.2},
    //        {90.5, 380.6}, {420.1, 410.8}, {180.3, 405.2}, {210.7, 280.4}, {480.2, 290.1},
    //        {350.6, 270.5}, {390.3, 300.7}
    //    };

    //    std::vector<std::vector<float>> frameData2 = {
    //        {230.4, 135.7}, {250.8, 125.3}, {210.2, 130.5}, {280.5, 140.1}, {190.6, 145.2},
    //        {320.1, 220.7}, {150.3, 230.4}, {430.8, 330.6}, {100.5, 320.2}, {580.3, 345.1},
    //        {120.7, 390.5}, {450.2, 420.8}, {200.4, 410.3}, {240.6, 290.7}, {500.1, 300.5},
    //        {380.3, 280.2}, {410.7, 310.6}
    //    };

    //    // 生成48帧数据
    //    for (int f = 0; f < numFrames; f++) {
    //        float t = static_cast<float>(f) / (numFrames - 1); // 0到1之间的插值因子
    //        std::vector<std::vector<float>> interpolatedFrame(numJoints);

    //        for (int j = 0; j < numJoints; j++) {
    //            // 对每个关节点进行线性插值
    //            float x = frameData1[j][0] + t * (frameData2[j][0] - frameData1[j][0]);
    //            float y = frameData1[j][1] + t * (frameData2[j][1] - frameData1[j][1]);
    //            interpolatedFrame[j] = { x, y };
    //        }
    //        poseResults[f] = interpolatedFrame;
    //    }

    //    // 处理每一帧数据
    //    for (int f = 0; f < numFrames; f++) {
    //        // 缩放当前帧的关键点坐标
    //        std::vector<float> scaledKeypoints;
    //        scaleKeypoints(poseResults[f], scaledKeypoints, imgWidth, imgHeight, modelInputWidth, modelInputHeight);

    //        // 将当前帧数据填充到输入数组
    //        for (int j = 0; j < numJoints; j++) {
    //            int jointOffset = j * modelInputHeight * modelInputWidth * 2;
    //            int frameOffset = f * numJoints * modelInputHeight * modelInputWidth * 2;

    //            // 计算坐标在输入数组中的位置
    //            int xIndex = frameOffset + jointOffset + static_cast<int>(scaledKeypoints[j * 2] * modelInputWidth);
    //            int yIndex = frameOffset + jointOffset + static_cast<int>(scaledKeypoints[j * 2 + 1] * modelInputHeight);

    //            // 设置关节点位置（边界检查）
    //            if (xIndex < poseC3DDetector.context->inputSize) inputData[xIndex] = 1.0f;
    //            if (yIndex < poseC3DDetector.context->inputSize) inputData[yIndex] = 1.0f;
    //        }
    //    }

    //    // 创建输出缓冲区
    //    float* outputData = new float[poseC3DDetector.context->outputSize];

    //    // 执行推理
    //    int result = poseC3DDetector.InferPoseC3D(poseC3DDetector.context, inputData, outputData);
    //    if (result == 0) {
    //        std::cout << "推理成功! 输出大小: " << poseC3DDetector.context->outputSize << std::endl;
    //        // 处理输出数据
    //        for (size_t i = 0; i < poseC3DDetector.context->outputSize && i < 10; i++) { // 只打印前10个结果
    //            std::cout << "输出[" << i << "]: " << outputData[i] << std::endl;
    //        }
    //    }
    //    else {
    //        std::cerr << "推理失败! 错误代码: " << result << std::endl;
    //    }

    //    // 释放内存
    //    delete[] inputData;
    //    delete[] outputData;
    //}
    //catch (const std::exception& e) {
    //    std::cerr << "发生异常: " << e.what() << std::endl;
    //}

    //for (int i = 0; i < functionParam.classNum; ++i) {
    //    if (poseC3DResultVMUnorderedMap->vmMap.count(i) > 0) {
    //        continue;
    //    }
    //    else {
    //        //poseC3DResultVMUnorderedMap->vmMap[i].calssId = i;
    //        //poseC3DResultVMUnorderedMap->vmMap[i].state = 0;
    //    }
    //}

    return 0;
}