#pragma once
#include <QWidget>
#include <QTabWidget>
#include "GraphicsViewTaskFlowWidget.h"
#include "ui_FramTaskFlowViewWidget.h"


class FramTaskFlowViewWidget : public QWidget
{
	Q_OBJECT

public:
	FramTaskFlowViewWidget(QWidget *parent = nullptr);
	~FramTaskFlowViewWidget();
	/// <summary>
	/// 显示当前画面
	/// </summary>
	/// <param name="name"></param>
	void SetCurrentTableWidget(const QString& name);
	//添加taskflow
	bool AddTaskTableWidget(const QString &name);
    /// <summary>
    /// 序列化时，添加taskflowId
    /// </summary>
    /// <param name="name"></param>
    /// <param name="task_flow_id"></param>
    /// <returns></returns>
    bool AddTaskTableWidget(const QString& name, const int task_flow_id);
	//删除流程
	void DelteTableWidget(const QString& name);
	/// <summary>
	/// 修改流程名称
	/// </summary>
	/// <param name="oldname">原名称</param>
	/// <param name="newname">新名称</param>
	bool ModifyTableWidgetName(const QString& oldname, const QString &newname);
	/// <summary>
	/// 返回图模型指针
	/// </summary>
	/// <param name="flow_name">图模型名称</param>
	/// <returns></returns>
	GraphicsViewTaskFlowWidget* DelegateGraphicsTaskFlow(const QString &flow_name){
		for (auto iter = ptrMapGraphicsViewTaskFlowWidget->begin(); iter != ptrMapGraphicsViewTaskFlowWidget->end(); iter++) {
			QString str = iter->second->GetTaskFlowName();
			if (str == flow_name) {
				return iter->second;
			}	
		}
		return nullptr;
	}

    /// <summary>
    /// 获得所有流程Name
    /// </summary>
    /// <param name="flows_name"></param>
    const QList<QString> GetAllTaskFlowName() {
        QList<QString> flows_name;
        for (auto iter = ptrMapGraphicsViewTaskFlowWidget->begin(); iter != ptrMapGraphicsViewTaskFlowWidget->end(); iter++) {
            QString str = iter->second->GetTaskFlowName();
            flows_name.append(str);
        }
        return flows_name;
    }

    /// <summary>
    /// 获得所有流程Name
    /// </summary>
    /// <param name="flows_name"></param>
 void GetAllTaskFlowName(QVector<QString>& flows_name) {
        for (auto iter = ptrMapGraphicsViewTaskFlowWidget->begin(); iter != ptrMapGraphicsViewTaskFlowWidget->end(); iter++) {
            QString str = iter->second->GetTaskFlowName();
            flows_name.append(str);
        }
    }

     /// <summary>
     /// 获得所有流程内部的函数
     /// </summary>
     /// <returns></returns>
 const std::shared_ptr<std::map<int, GraphicsViewTaskFlowWidget*>> GetMapGraphicsViewTaskFlows() { 
     return this->ptrMapGraphicsViewTaskFlowWidget;
 }

 /// <summary>
 /// 获得节点的Map的Id
 /// </summary>
 /// <param name="flow_name">流程名</param>
 /// <returns></returns>
 const int GetTaskFlowIndex(const QString &flow_name) {
     for (auto iter = ptrMapGraphicsViewTaskFlowWidget->begin(); iter != ptrMapGraphicsViewTaskFlowWidget->end(); iter++) {
         QString str = iter->second->GetTaskFlowName();
         if (str == flow_name) {
             return iter->first;
         }
     }
     return -1;
 }

	/// <summary>
	/// 反序列化文件
	/// </summary>
	/// <param name="path"></param>
	bool DeserializeProject(QJsonObject const& jsonDocument);
	/// <summary>
	/// 序列化文件
	/// </summary>
	/// <param name="path"></param>
	QJsonObject SerializeProject()const;


private:
	void Initial();
	int GetStringNameTabIndex(const QString &name);
	QJsonObject SerializeTaskFlow(GraphicsViewTaskFlowWidget* taskflow);

private:
	Ui::FramTaskFlowViewWidgetClass ui;
	QTabWidget* tabWidget;
	int nextTaskFlowId = 0;
    //流数据的Map，保存了所有流
    std::shared_ptr<std::map<int, GraphicsViewTaskFlowWidget*>> ptrMapGraphicsViewTaskFlowWidget{ nullptr };
};
