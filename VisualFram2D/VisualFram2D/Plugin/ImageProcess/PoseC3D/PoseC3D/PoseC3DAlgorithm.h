#pragma once
#include <opencv2\opencv.hpp>
#include "SerializeStructParam.h"
#include "GeneralStruct.h"
#include "PoseC3DSrc/PoseC3D.h"

class PoseC3DAlgorithm
{
public:

    PoseC3DAlgorithm();
	~PoseC3DAlgorithm();

    using Ptr = std::shared_ptr<PoseC3DAlgorithm>;

    void SetParam(const FunctionParam& fun_param);

    int RunPoseC3D( const PoseC3DDetector& detector, const FunctionParam& fun_param = FunctionParam());


    const PtrVMUnorderedMap<int, PtrVMPolygon>& GetInferResult() {
        return this->poseC3DResultVMUnorderedMap;
    }

private:

    FunctionParam functionParam;
    cv::Mat sourceMat;
    PtrVMUnorderedMap<int, PtrVMPolygon> poseC3DResultVMUnorderedMap{ nullptr };
    PoseC3DDetector poseC3DDetector;
};

