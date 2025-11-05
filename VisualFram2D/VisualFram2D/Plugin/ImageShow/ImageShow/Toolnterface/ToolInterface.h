#pragma once
#include <QDialog>
#include "ToolNodeDataInteract.h"

class ToolInterface : public QDialog
{
	Q_OBJECT
public:
	ToolInterface(QWidget* parent = nullptr) :QDialog(parent) {};
	ToolInterface(const QString& function_name, QWidget* parent = nullptr) :selfToolName(function_name), QDialog(parent) {};
    virtual ~ToolInterface() {};

	/// <summary>
	/// 循环运行
	/// </summary>
	/// <returns></returns>
	virtual int Execute() = 0;
	/// <summary>
	/// 运行一次
	/// </summary>
	/// <returns></returns>
	virtual int OnceExecute() = 0;
	/// <summary>
	/// 序列化
	/// </summary>
	/// <returns></returns>
	virtual QJsonObject SerializeFunciton() = 0;
	/// <summary>
	/// 反序列化文件
	/// </summary>
	/// <returns></returns>
	virtual void ExserializeFunciton(const QJsonObject& json) = 0;
	/// <summary>
	/// 设置子节点与主界面数据交互指针
	/// </summary>
	/// <param name="ptr">数据交互指针</param>
	void SetToolNodeDataInteract(ToolNodeDataInteract::Ptr ptr) {
		toolNodeDataInteract = ptr;
	}
	/// <summary>
	/// 返回子节点数据交互的指针
	/// </summary>
	const ToolNodeDataInteract::Ptr GetToolNodeDataInteract() const{
		return this ->toolNodeDataInteract;
	}

    /// <summary>
    /// 设置子节点与主界面数据交互指针
    /// </summary>
    /// <param name="ptr">数据交互指针</param>
    void SetToolNodeMapData(std::shared_ptr<std::unordered_map<int, std::vector<PtrVMObject>>> ptr) {
        ptrMapNodesOutData = ptr;
    }

    /// <summary>
    /// 返回子节点数据交互的指针
    /// </summary>
    const  std::shared_ptr<std::unordered_map<int, std::vector<PtrVMObject>>> SetToolNodeMapData() const {
        return this->ptrMapNodesOutData;
    }

	/// <summary>
	/// 设定工具名称
	/// </summary>
	/// <param name="name"></param>
	void SetToolName(const QString& name) {
		this->selfToolName = name;
	}
	/// <summary>
	/// 获得工具名称
	/// </summary>
	/// <returns></returns>
	const QString GetToolName() const {
		return this->selfToolName;
	}

    /// <summary>
    /// 设置nodeid，唯一编码
    /// 为寻找全局变量做准备的
    /// </summary>
    /// <param name="node_id"></param>
    virtual void SetNodeId(const int node_id) = 0;

    /// <summary>
    /// 返回nodoid，唯一编码
    /// </summary>
    /// <returns></returns>
    const int GetNodeId() const {
        return this->nodeId;
    }

protected:
	QString selfToolName{ "ToolName" };
    int nodeId{-1};
	ToolNodeDataInteract::Ptr toolNodeDataInteract{nullptr};
    /// <summary>
    /// 自身流程的节点数据
    /// </summary>
    std::shared_ptr<std::unordered_map<int, std::vector<PtrVMObject>>> ptrMapNodesOutData{nullptr};
};


