#pragma once
#include <QWidget>
#include <unordered_map>
#include <iostream>
#include <functional>
#include <memory>
#include <QString>

class CommDeviceFactory {
public:
    using Ptr = std::shared_ptr<CommDeviceFactory>;
    using ConstPtr = std::shared_ptr<const CommDeviceFactory>;

    CommDeviceFactory() {
        if (commDeviceMap == nullptr) {
            commDeviceMap = std::make_shared<std::unordered_map<QString, std::function<QWidget* ()>>>();
        }
    }

    CommDeviceFactory(const CommDeviceFactory&) = delete;
    CommDeviceFactory(CommDeviceFactory&&) = delete;

    template<typename T>
    void CommDeviceRegister(const QString& key) {
        commDeviceMap->emplace(key, [] { return new T(); });
    }

    QWidget* CommDeviceProduce(const QString& key)
    {
        if (commDeviceMap->find(key) == commDeviceMap->end())
            throw std::invalid_argument("the message key is not exist!");
        return (*commDeviceMap)[key]();
    }

    std::vector<QString> GetAllCameraKind()
    {
        std::vector<QString> v_all_camera;
        for (auto iter = commDeviceMap->begin(); iter != commDeviceMap->end(); iter++) {
            v_all_camera.push_back(iter->first);
        }
        return v_all_camera;
    }

private:
    std::shared_ptr<std::unordered_map<QString, std::function<QWidget* ()>>> commDeviceMap;
};
