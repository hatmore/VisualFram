#define EXPORT_DLL
#include "PoseC3D.h"
#include <windows.h>
#include <fstream>
#include <filesystem>

PoseC3DDetector::PoseC3DDetector()
{ }

PoseC3DDetector::~PoseC3DDetector()
{ }


void PoseC3DDetector::BuildOnnx2TrT(std::string onnxFilePath, std::string trtFilePath)
{
}

void PoseC3DDetector::InitializePoseC3D(std::string trtPath) 
{
    
}

int PoseC3DDetector::InferPoseC3D( float* inputData, float* outputData) 
{

    return 0;
}