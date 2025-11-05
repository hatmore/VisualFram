#pragma once
#include "BaseCamera.h"
#include <unordered_map>
#include <iostream>
#include <functional>
#include <memory>

class CameraFactory {
public:
    using Ptr = std::shared_ptr<CameraFactory>;
    using ConstPtr = std::shared_ptr<const CameraFactory>;

    CameraFactory() {
        if (camerasMap == nullptr) {
            camerasMap = std::make_shared<std::unordered_map<std::string, std::function<std::shared_ptr<BaseCamera>()>>>();
        }
    }

    CameraFactory(const CameraFactory&) = delete;
    CameraFactory(CameraFactory&&) = delete;

    template<typename T>
    void CameraRegister(const std::string& key) {
        camerasMap->emplace(key, [] { return std::make_shared<T>(); });
    }

    std::shared_ptr<BaseCamera> CameraProduce(const std::string& key)
    {
        if (camerasMap->find(key) == camerasMap->end())
            throw std::invalid_argument("the message key is not exist!");
        return (*camerasMap)[key]();
    }

    std::vector<std::string> GetAllCameraKind()
    {
        std::vector<std::string> v_all_camera;
        for (auto iter = camerasMap->begin(); iter != camerasMap->end(); iter++) {
            v_all_camera.push_back(iter->first);
        }
        return v_all_camera;
    }

private:
    std::shared_ptr<std::unordered_map<std::string, std::function<std::shared_ptr<BaseCamera>()>>> camerasMap;
};
