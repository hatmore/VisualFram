#pragma once
#pragma execution_character_set("utf-8")

#include <vector>
#include <memory>
#include <QString>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <tbb\concurrent_queue.h>
#include "ImageView/QGraphicsViews.h"
#include "GeneralStruct.h"
#include "SystemParamStruct.h"
#include "Toolnterface/ToolNodeDataInteract.h"


class StaticGlobalVariable
{
public:
	static std::chrono::milliseconds threadTimeOut;
    static  ToolNodeDataInteract::Ptr ptrToolNodeDataInteract;
    static PtrSystemSetParam ptrSystemSetParam; // 系统参数，主要是加载路径相关
};

