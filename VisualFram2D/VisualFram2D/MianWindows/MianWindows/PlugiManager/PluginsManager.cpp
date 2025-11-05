#include "PluginsManager.h"
#include <pybind11/embed.h>
#include <QDir>
#include <QCoreApplication>
#include <QJsonArray>
#include <QDebug>

PluginsManager* PluginsManager::pluginInstance = nullptr;

PluginsManager::PluginsManager()
{
}

PluginsManager::~PluginsManager()
{
}

void PluginsManager::LoadAllPlugins()
{
	QDir pluginsdir = QDir(qApp->applicationDirPath());
	pluginsdir.cd("plugins");
	QFileInfoList pluginsInfo = pluginsdir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

	//加载插件
	for (QFileInfo fileinfo : pluginsInfo) {
		LoadPlugin(fileinfo.absoluteFilePath());
	}
}

void PluginsManager::ScanMetaData(const QPluginLoader& pluginLoader, PluginsMetaData& plugins_metadata)
{
	QJsonObject json = pluginLoader.metaData().value("MetaData").toObject();
	plugins_metadata.author =  json.value("author").toString();
	plugins_metadata.date = json.value("date").toString();
	plugins_metadata.name = json.value("name").toString();
	plugins_metadata.classes = json.value("classes").toString();
	plugins_metadata.version = json.value("version").toString();
}


void PluginsManager::LoadPlugin(const QString& filepath)
{
	if (!QLibrary::isLibrary(filepath))
		return;
	//加载插件
	QPluginLoader loader(filepath);
	auto is_load = loader.load();
	if (loader.load()) {
		// 如果继承自 Plugin，则认为是自己的插件（防止外部插件注入）。
		InterfacePlugin* plugin = qobject_cast<InterfacePlugin*>(loader.instance());
		if (plugin){
			PtrInterfacePlugin ptr_plugin  = std::shared_ptr<InterfacePlugin>(plugin);
			PluginsMetaData plugins_metadata;
			ScanMetaData(loader, plugins_metadata);
			QString name = ptr_plugin->ThisName();
			QString information = ptr_plugin->ThisClass();
			mapPtrInterfacePlugins.insert(std::make_pair(name, ptr_plugin));
		}
	}
}