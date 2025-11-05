#pragma once
#include <mutex>
#include <thread>
#include <QObject>
#include < QString>
#include <tbb\concurrent_queue.h>
#include "../GeneralStruct.h"

class ToolNodeDataInteract : public QObject
{
	Q_OBJECT
public:
    using Ptr = std::shared_ptr<ToolNodeDataInteract>;
    using ConstPtr = std::shared_ptr<const ToolNodeDataInteract>;
	ToolNodeDataInteract();
	~ToolNodeDataInteract();
    /// <summary>
    /// Push到队列调试信息
    /// </summary>
    /// <param name="log"></param>
    void PushLogData(const std::pair<LOGTYPE, QString>& log);
    /// <summary>
    /// 输出日志信息
    /// </summary>
    /// <param name="lo">返回值，成功true，false失败</param>
    /// <returns></returns>
    bool TryPopLogData(std::pair<LOGTYPE, QString>& log);
    /// <summary>
    /// 插入元素，新建外部设备map
    /// </summary>
    /// <param name="key"></param>
    void MapInserExDevice(const QString& key);
    /// <summary>
    /// 删除key
    /// </summary>
    /// <param name="key"></param>
    void MapEraseExDevice(const QString& key);
    /// <summary>
    /// 获得所有键值
    /// </summary>
    /// <param name="v_keys"></param>
    void GetMapExDeviceKeys(std::vector<QString>& v_keys);
    /// <summary>
    /// push外部设计数据
    /// </summary>
    /// <param name="key">关键字</param>
    /// <param name="ptr">输入数据</param>
    bool PushExDeviceData(const QString& key, const  ExDeviceData::Ptr& ptr);
    /// <summary>
    /// 弹出外部数据
    /// </summary>
    /// <param name="key">关键字</param>
    /// <param name="ptr">输入数据</param>
    bool TryPopExDeviceData(const QString& key, ExDeviceData::Ptr& ptr);

public:
    std::shared_ptr<tbb::concurrent_queue<std::pair<LOGTYPE, QString>>> ptrQueueNodeLogData;  //子节点发出来的调试数据
    //std::shared_ptr<tbb::concurrent_queue<std::pair<int, std::vector<PtrVMObject>>>> ptrQueueNodesRanderObject;  //子节点渲染数据，废弃不用了

    /// <summary>
    /// map id:代表渲染画面屏幕，后面是渲染数据
    /// </summary>
    std::shared_ptr<std::unordered_map<int, std::shared_ptr<tbb::concurrent_queue<std::vector<PtrVMObject>>>>> ptrMapPtrQueueNodeRanderObject;//子节点渲染数据

    /// <summary>
    /// pair: id:节点id,后面是SOP报表数据
    /// </summary>
    std::shared_ptr<tbb::concurrent_queue<std::pair<int, PtrVMObject>>> ptrQueuePairSOPReport;//SOP报表数据

	std::shared_ptr<std::unordered_map<QString, std::shared_ptr<tbb::concurrent_bounded_queue<PtrExDeviceData>>>> ptrMapQueuePtrExDeviceData;  //外部设备数据
	std::shared_ptr<tbb::concurrent_queue<QByteArray>> ptrQueueExternDeviceWriteData;  //向外部设备写数据
    std::shared_ptr<std::unordered_map<int, std::shared_ptr<std::unordered_map<int, std::vector<PtrVMObject>>>>> ptrMapMapAllNodesOutData;   //每个节点数据，用于获得所有节点的输出数据情况
    std::shared_ptr<std::unordered_map<QString, std::shared_ptr<tbb::concurrent_queue<int>>>> ptrMapQueueDeviceCommunicationData;  //设备通信数据,用于设备通信的输入输出数据，暂时仅仅输入int

private:
    int detectionImageCacheSize{10};   //检测是缓存图像数量
};

