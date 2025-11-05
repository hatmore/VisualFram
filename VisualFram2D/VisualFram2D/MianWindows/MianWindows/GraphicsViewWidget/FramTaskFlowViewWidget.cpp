#include "FramTaskFlowViewWidget.h"
#include <QJsonArray>

FramTaskFlowViewWidget::FramTaskFlowViewWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	Initial();
}

FramTaskFlowViewWidget::~FramTaskFlowViewWidget()
{
    for (auto iter = ptrMapGraphicsViewTaskFlowWidget->begin(); iter != ptrMapGraphicsViewTaskFlowWidget->end(); iter++) {
        GraphicsViewTaskFlowWidget* taskflow = iter->second;
        taskflow->deleteLater();
        taskflow = nullptr;
    }
    this->deleteLater();
}

void FramTaskFlowViewWidget::Initial()
{
	tabWidget = ui.tabWidget;
	tabWidget->setTabBarAutoHide(true);
	tabWidget->setTabPosition(QTabWidget::North);
	tabWidget->setTabShape(QTabWidget::Triangular);
    ptrMapGraphicsViewTaskFlowWidget = std::make_shared<std::map<int, GraphicsViewTaskFlowWidget*>>();
}

bool FramTaskFlowViewWidget::DeserializeProject(QJsonObject const& jsonDocument)
{
	return false;
}

QJsonObject FramTaskFlowViewWidget::SerializeTaskFlow(GraphicsViewTaskFlowWidget* taskflow)
{
	QJsonObject taskflow_json;
	QJsonObject flowgraph_model_json = taskflow->GetDataFlowGraphModel()->save();
	taskflow_json["TaskFlowName"] = taskflow->GetTaskFlowName();
	taskflow_json["TaskFlowEnabledState"] = taskflow->GetTaskFlowEnabledState();
	taskflow_json["FlowGraphModel"] = flowgraph_model_json;
	return taskflow_json;
	// TODO: 在此处插入 return 语句
}

QJsonObject FramTaskFlowViewWidget::SerializeProject() const
{
	QJsonObject project_json;
	QJsonArray taskflow_json_array;
	for (auto iter = ptrMapGraphicsViewTaskFlowWidget->begin();iter != ptrMapGraphicsViewTaskFlowWidget->end(); iter++) {
		GraphicsViewTaskFlowWidget* taskflow = iter->second;
		QJsonObject taskflow_json;
		QJsonObject flowgraph_model_json = taskflow->GetDataFlowGraphModel()->save();
		taskflow_json["TaskFlowName"] = taskflow->GetTaskFlowName();
        taskflow_json["TaskFlowId"] = taskflow->GetTaskFlowId();
		taskflow_json["TaskFlowEnabledState"] = taskflow->GetTaskFlowEnabledState();
		taskflow_json["FlowGraphModel"] = flowgraph_model_json;
		taskflow_json_array.append(taskflow_json);
	}

	project_json["Project"] = taskflow_json_array;
	return project_json;
	// TODO: 在此处插入 return 语句
}

bool FramTaskFlowViewWidget::AddTaskTableWidget(const QString& name)
{
	for (auto iter = ptrMapGraphicsViewTaskFlowWidget->begin(); iter != ptrMapGraphicsViewTaskFlowWidget->end(); iter++) {
		QString str = iter->second->GetTaskFlowName();
		if (str == name) {
			QMessageBox::information(NULL, QString("提示"),QString("流程重命名"), QMessageBox::Yes, NULL);
			return false;
		}
	}

    //新加入的流程，编号+1
    nextTaskFlowId = nextTaskFlowId + 1;
	GraphicsViewTaskFlowWidget* graphicsview = new GraphicsViewTaskFlowWidget();
    graphicsview->SetTaskFlowId(nextTaskFlowId);
	graphicsview->SetTaskFlowName(name);
	graphicsview->SetTaskFlowEnabledState(true);
	tabWidget->addTab(graphicsview, name);
	ptrMapGraphicsViewTaskFlowWidget->insert(std::make_pair(nextTaskFlowId, graphicsview));
	return true;
}

bool FramTaskFlowViewWidget::AddTaskTableWidget(const QString& name,const int task_flow_id)
{
    for (auto iter = ptrMapGraphicsViewTaskFlowWidget->begin(); iter != ptrMapGraphicsViewTaskFlowWidget->end(); iter++) {
        QString str = iter->second->GetTaskFlowName();
        if (str == name) {
            QMessageBox::information(NULL, QString("提示"),QString("流程重命名"), QMessageBox::Yes, NULL);
            return false;
        }
    }

    GraphicsViewTaskFlowWidget* graphicsview = new GraphicsViewTaskFlowWidget();
    graphicsview->SetTaskFlowName(name);
    graphicsview->SetTaskFlowEnabledState(true);
    tabWidget->addTab(graphicsview, name);
    nextTaskFlowId = std::max(nextTaskFlowId, task_flow_id + 1);
    graphicsview->SetTaskFlowId(task_flow_id);
    ptrMapGraphicsViewTaskFlowWidget->insert(std::make_pair(task_flow_id, graphicsview));
    return true;
}

void FramTaskFlowViewWidget::SetCurrentTableWidget(const QString& name)
{
	int index = GetStringNameTabIndex(name);
	if (index < 0)
		return;
	tabWidget->setCurrentIndex(index);
}

void FramTaskFlowViewWidget::DelteTableWidget(const QString& name)
{
	int index = GetStringNameTabIndex(name);
	if (index < 0)
		return;

	for (auto iter = ptrMapGraphicsViewTaskFlowWidget->begin(); iter != ptrMapGraphicsViewTaskFlowWidget->end(); iter++) {
		QString str = iter->second->GetTaskFlowName();
		if (str == name) {
			delete iter->second;
			iter->second = nullptr;
			ptrMapGraphicsViewTaskFlowWidget->erase(iter);
			break;
		}
	}
}

bool FramTaskFlowViewWidget::ModifyTableWidgetName(const QString& oldname, const QString& newname)
{
	for (auto iter = ptrMapGraphicsViewTaskFlowWidget->begin(); iter != ptrMapGraphicsViewTaskFlowWidget->end(); iter++) {
		QString str = iter->second->GetTaskFlowName();
		if (str == newname) {
			QMessageBox::information(NULL, QString("提示"),QString("流程重命名"), QMessageBox::Yes, NULL);
			return false;
		}
	}

	int index = GetStringNameTabIndex(oldname);
	if (index < 0)
		return false;
	for (auto iter = ptrMapGraphicsViewTaskFlowWidget->begin(); iter != ptrMapGraphicsViewTaskFlowWidget->end(); iter++) {
		QString str = iter->second->GetTaskFlowName();
		if (str == oldname) {
			iter->second->SetTaskFlowName(newname);
		}
	}
	tabWidget->setTabText(index, newname);\
	return true;
}


int FramTaskFlowViewWidget::GetStringNameTabIndex(const QString& name)
{
	int tab_count = tabWidget->count();
	for (int i = 0; i < tab_count; i++){
		QString str = tabWidget->tabText(i);
		if (name == str){
			return i;
		}
	}
	return -1;
}