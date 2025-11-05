#pragma once
#pragma execution_character_set("utf-8")
#include "../Toolnterface/InterfacePlugin.h"
#include"../GeneralStruct.h"
#include <QPluginLoader>
#include <QObject>
#include <QString>
#include <Qmap>
#include <map>
#include <memory>
#include <QVector>
#include <QtDebug>


class InterfacePlugin;
using PtrInterfacePlugin = std::shared_ptr<InterfacePlugin>;

class PluginsManager : public QObject
{
	Q_OBJECT
public:

	PluginsManager();
	~PluginsManager();

	static PluginsManager* PluginsManagerInstance() {
		if (pluginInstance == nullptr)
			pluginInstance = new PluginsManager();
		return pluginInstance;
	}
	//加载所有插件
	void LoadAllPlugins();
	//加载其中某个插件
	void LoadPlugin(const QString& filepath);


private:
	void ScanMetaData(const QPluginLoader& pluginLoader, PluginsMetaData & plugins_metadata);


public:
	std::map<QString, PtrInterfacePlugin> mapPtrInterfacePlugins;
	//std::map<QString, std::shared_ptr<std::map<QString, PluginsMetaData>>> mapPtrMapInterfacePluginsMetaData;
	////std::map<QString, ShowFromDialog> mapFunctionPlugin; //算法函数插件，初始所有算法

private:
	static PluginsManager* pluginInstance;
};

