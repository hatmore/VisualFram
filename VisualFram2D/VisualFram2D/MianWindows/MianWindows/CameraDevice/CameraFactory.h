#pragma once
#include "BaseCamera.h"
#include <unordered_map>
#include <iostream>
#include <functional>
#include <memory>

class CameraFactory {
public:
    template<typename T>
    struct CameraRegister {
        CameraRegister(const std::string& key) {
            CameraFactory::get().camerasMap->emplace(key, [] { return std::make_shared<T>(); });
        }

        template<typename... Args>   CameraRegister(const std::string& key, Args... args) {
            CameraFactory::get().camerasMap->emplace(key, [&] { return std::make_shared<T>(args...); });
        }
    };

    static std::shared_ptr<BaseCamera> CameraProduce(const std::string& key)
    {
        if (camerasMap->find(key) == camerasMap->end())
            throw std::invalid_argument("the message key is not exist!");
        return (*camerasMap)[key]();
    }

    static std::vector<std::string> GetAllCameraKind()
    {
        std::vector<std::string> v_all_camera;
        for (auto iter = camerasMap->begin(); iter != camerasMap->end(); iter++) {
            v_all_camera.push_back(iter->first);
        }
        return v_all_camera;
    }

private:
    CameraFactory() {
        if (camerasMap == nullptr) {
            camerasMap = std::make_shared<std::unordered_map<std::string, std::function<std::shared_ptr<BaseCamera>()>>>();
        }
    }
    CameraFactory(const CameraFactory&) = delete;
    CameraFactory(CameraFactory&&) = delete;

    static CameraFactory& get() {
        static CameraFactory instance;
        return instance;
    }

    static std::shared_ptr<std::unordered_map<std::string, std::function<std::shared_ptr<BaseCamera>()>>> camerasMap;
};


#define REGISTER_MESSAGE_VNAME(T) reg_msg_##T##_
#define REGISTER_MESSAGE(T, key, ...) static CameraFactory::CameraRegister<T> REGISTER_MESSAGE_VNAME(T)(key, ##__VA_ARGS__)