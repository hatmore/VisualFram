#pragma once
#include <Qvector>
#include <QString>
#include <memory>
#include <mutex>

/// <summary>
/// 软件自启动
/// 外部触发使能
/// 启动后流程自动运行
/// 是否强制重启软件
/// </summary>
struct  SystemSetParam {
	using Ptr = std::shared_ptr<SystemSetParam>;
	bool programSelfStart{ false };
    bool programTriggerEnable{ false };
	bool processSelfStart{ false };
	bool forceProgramSelfStart{ false };
	int signOutTime{ 10 };
	int saveLogTime{ 30 };
    int imageShowMode{0};
	QString projectPath;
};
using PtrSystemSetParam = std::shared_ptr<SystemSetParam>;
