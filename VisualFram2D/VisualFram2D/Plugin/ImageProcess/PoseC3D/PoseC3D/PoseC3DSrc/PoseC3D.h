#ifndef POSEC3D_H
#define POSEC3D_H

#include <iostream>
#include <fstream>
#include <vector>
#include <NvInfer.h>
#include <NvOnnxParser.h>
#include <cuda_runtime_api.h>


class PoseC3DDetector
{
public:
    PoseC3DDetector();
    ~PoseC3DDetector();

    void InitializePoseC3D(std::string trt_path);
    void BuildOnnx2TrT(std::string onnxFilePath, std::string trtFilePath);

    int InferPoseC3D(float* inputData, float* outputData);

private:
    void get_engine();

};


#endif 