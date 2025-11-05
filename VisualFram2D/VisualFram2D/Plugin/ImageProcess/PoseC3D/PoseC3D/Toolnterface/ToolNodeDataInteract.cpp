#include "ToolNodeDataInteract.h"

#define MAX_IMAGESHOW_VIEWS 9

ToolNodeDataInteract::ToolNodeDataInteract()
{
	ptrMapQueuePtrExDeviceData = std::make_shared<std::unordered_map<QString, std::shared_ptr<tbb::concurrent_bounded_queue<ExDeviceData::Ptr>>>>();
    ptrQueueNodeLogData = std::make_shared<tbb::concurrent_queue<std::pair<LOGTYPE, QString>>>();  //子节点发出来的调试数据
    //ptrQueueNodesRanderObject = std::make_shared<tbb::concurrent_queue<std::pair<int, std::vector<PtrVMObject>>>>();  //子节点渲染数据
    ptrQueueExternDeviceWriteData = std::make_shared<tbb::concurrent_queue<QByteArray>>();
    ptrMapMapAllNodesOutData = std::make_shared<std::unordered_map<int, std::shared_ptr<std::unordered_map<int, std::vector<PtrVMObject>>>>>();
    ptrMapQueueDeviceCommunicationData = std::make_shared<std::unordered_map<QString, std::shared_ptr<tbb::concurrent_queue<int>>>>();
    ///
    ptrMapPtrQueueNodeRanderObject = std::make_shared<std::unordered_map<int, std::shared_ptr<tbb::concurrent_queue<std::vector<PtrVMObject>>>>>();
    for (int index = 0; index < MAX_IMAGESHOW_VIEWS; index++){
        std::shared_ptr<tbb::concurrent_queue<std::vector<PtrVMObject>>> ptr_queue = std::make_shared<tbb::concurrent_queue<std::vector<PtrVMObject>>>();
        ptrMapPtrQueueNodeRanderObject->insert(std::make_pair(index, ptr_queue));
    }
    ///最多有九个SOP报表
   ptrQueuePairSOPReport = std::make_shared<tbb::concurrent_queue<std::pair<int, PtrVMObject>>>();//SOP报表数据
}

ToolNodeDataInteract::~ToolNodeDataInteract()
{

}

void ToolNodeDataInteract::PushLogData(const std::pair<LOGTYPE, QString>& log)
{
    ptrQueueNodeLogData->push(log);
}

bool ToolNodeDataInteract::TryPopLogData(std::pair<LOGTYPE, QString>& log)
{
    return  ptrQueueNodeLogData->try_pop(log);
}

void ToolNodeDataInteract::MapInserExDevice(const QString& key)
{
    ///新建设备
    std::shared_ptr<tbb::concurrent_bounded_queue<ExDeviceData::Ptr>> ptr_queue = std::make_shared<tbb::concurrent_bounded_queue<ExDeviceData::Ptr>>();
    ptr_queue->set_capacity(detectionImageCacheSize);
    ptrMapQueuePtrExDeviceData->insert(std::make_pair(key, ptr_queue));
}

void ToolNodeDataInteract::MapEraseExDevice(const QString& key)
{
    auto iter = ptrMapQueuePtrExDeviceData->find(key);
    if (iter != ptrMapQueuePtrExDeviceData->end()) {
        ptrMapQueuePtrExDeviceData->erase(iter);
    }
}

void ToolNodeDataInteract::GetMapExDeviceKeys(std::vector<QString>& v_keys)
{
    std::transform(ptrMapQueuePtrExDeviceData->begin(), ptrMapQueuePtrExDeviceData->end(), std::back_inserter(v_keys),
        [](const auto& pair) { return pair.first; });
}

bool ToolNodeDataInteract::PushExDeviceData(const QString& key, const ExDeviceData::Ptr& ptr)
{
    auto iter = ptrMapQueuePtrExDeviceData->find(key);
    if (iter == ptrMapQueuePtrExDeviceData->end())
        return false;

    std::shared_ptr<tbb::concurrent_bounded_queue<ExDeviceData::Ptr>> ptr_queue = iter->second;
    if (ptr_queue->try_push(ptr)) { // 不阻塞，失败返回 false
        return true;
    }
    else {
        ExDeviceData::Ptr ptr_;
        ptr_queue->pop(ptr_);
        ptr_queue->push(ptr);
        return false;
    }
}

bool ToolNodeDataInteract::TryPopExDeviceData(const QString& key, ExDeviceData::Ptr& ptr)
{
    auto iter = ptrMapQueuePtrExDeviceData->find(key);
    if (iter != ptrMapQueuePtrExDeviceData->end())
        return false;
    std::shared_ptr<tbb::concurrent_bounded_queue<ExDeviceData::Ptr>> ptr_queue = iter->second;
    return ptr_queue->try_pop(ptr);
}