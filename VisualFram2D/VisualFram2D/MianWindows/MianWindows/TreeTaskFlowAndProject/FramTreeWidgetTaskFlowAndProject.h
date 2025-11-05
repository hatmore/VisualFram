#pragma once
#include <QWidget>
#include "../GraphicsViewWidget/FramTaskFlowViewWidget.h"
#include "../ThreadPools/ThreadPool.h"
#include "../Log/frmLog.h"
#include "ui_FramTreeWidgetTaskFlowAndProject.h"

class FramTreeWidgetTaskFlowAndProject : public QWidget
{
	Q_OBJECT

public:
	FramTreeWidgetTaskFlowAndProject(QWidget *parent = nullptr);
	FramTreeWidgetTaskFlowAndProject(FramTaskFlowViewWidget* fram_widget, QWidget* parent = nullptr);
	~FramTreeWidgetTaskFlowAndProject();
	/// <summary>
	/// 添加流程
	/// </summary>
	void AddTaskFlow();
    
    /// <summary>
    ///获得所有流程图的名称 
    /// </summary>
    inline void GetAllTaskFlowNname(QVector<QString> &v_name) {
       return  framTaskFlowViewWidget->GetAllTaskFlowName(v_name);
    }

	/// <summary>
	/// 反序列化文件
	/// </summary>
	/// <param name="path"></param>
	bool DeserializeProject(const QJsonObject& json_project);
	/// <summary>
	/// 序列化文件
	/// </summary>
	/// <param name="path"></param>
	bool SerializeProject(const QString& path);

    /// <summary>
    /// 序列化流程
    /// </summary>
    /// <returns></returns>
    QJsonObject SerializeProject();


private:
	void Initial();
	void InitialPushButton();
	void InitialMenu();
	void InitialTreeWidget();

    // 提取原始名称（如果不存在分隔符，返回原值）
    QString ExtractBaseName(const QString& display) const;
    /// <summary>
    /// 改变Item的颜色
    /// </summary>
    /// <param name="item"></param>
    /// <param name="enabled"></param>
    inline void SetItemEnableColor(QTreeWidgetItem* item, bool enabled) {
        if (!item) return;
        item->setBackground(0, QBrush(enabled ? QColor(255, 255, 255) : QColor(205, 205, 205)));
    }

private slots:
	/// <summary>
	/// 添加节点
	/// </summary>
	void AddTaskFlowSlot();
	/// <summary>
	/// 删除节点
	/// </summary>
	void DeleteTaskFlowSlot();
	/// <summary>
	/// 选择节点
	/// </summary>
	/// <param name="item"></param>
	/// <param name="column"></param>
	void ItemPressedSlot(QTreeWidgetItem* item, int column);
	/// <summary>
	/// 右键菜单栏
	/// </summary>
	/// <param name="point"></param>
	void OnRightClickedSlot(const QPoint& point);
	/// <summary>
	/// 图流程状态。是否禁用
	/// </summary>
	void DisableTaskFlowSlot();
	/// <summary>
	/// 图流程名称修改
	/// </summary>
	void ModefyTaskFlowNameSlot();
	/// <summary>
	/// 图模型运行一次
	/// </summary>
	void TaskFlowRunOnce();
	/// <summary>
	/// 图模型运行N次
	/// </summary>
	void TaskFlowRunNCycle();
	/// <summary>
	/// 图模型运行
	/// </summary>
	void TaskFlowRunCycle();
	/// <summary>
	/// 暂停运行
	/// </summary>
	void TaskFlowStopRun();
	/// <summary>
	/// 添加item
	/// </summary>
	/// <param name="name"></param>
	void QTreeWidgetAddItem(const QString & item_name, const bool state = true);

signals:
	void SendLogInfoSynSig(const QString& info, const LOGTYPE type);

private:
	Ui::FramTreeWidgetTaskFlowAndProjectClass ui;
	QTreeWidgetItem* itemParent;
	QTreeWidgetItem* leftCurrentPressedItem{ nullptr };
	QTreeWidgetItem* rightCurrentPressedItem{ nullptr };
	/// <summary>
	/// 流程图容器
	/// </summary>
	FramTaskFlowViewWidget* framTaskFlowViewWidget{nullptr};
	QMenu* mMenu{ nullptr };
	/// <summary>
	/// Flows之间的间隔符号
	/// </summary>
    const QString flowNameSpace{ ": " };

	ThreadPool::Ptr ptrThreadPools;   //新建线程池
};
