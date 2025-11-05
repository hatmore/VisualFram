#pragma once
#pragma execution_character_set("utf-8")
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QPoint>
#include <QLabel>
#include <QMenu>
#include <QListWidgetItem>
#include <QTabWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QPainter>
#include "../GeneralStruct.h"

class NodeEmbedWidget : public QWidget
{
	Q_OBJECT
public:
	explicit NodeEmbedWidget(QWidget* parent = 0);
	/// <summary>
	/// 修改Node名称
	/// </summary>
	/// <param name="tile"> tile名称</param>
	void ModifyNodeTitle(const QString &tile);

	/// <summary>
	/// 修改Node名称
	/// </summary>
	/// <param name="tile"> tile名称</param>
	void ModifyNodeTitle(const unsigned int &node_id,const QString& tile);

	const QString GetNodeTitle() const {
		return this->nodeTitle;
	}
	/// <summary>
	/// 修改执行时间
	/// </summary>
	/// <param name="time">执行时间显示</param>
	void ModifyNodeTime(const float &time);
	/// <summary>
	/// 改变显示图像
	/// </summary>
	/// <param name="icon"> 图像数据 </param>
	void SetImageNode(const QIcon& icon);

public slots:
	void AcceptNodeStateSlot(const float& time, const RUNNINGSTATE& run_state);

private:
	void Initial();

protected:
	void paintEvent(QPaintEvent* event) override;

private:
	QVBoxLayout* qVBoxLayout{nullptr};
	QLabel *titleQLabel{nullptr};
	QLabel *imageQLabel{nullptr};
	QLabel *nodeIdQLabel{nullptr};
	QLabel *timeQLbale{ nullptr };
	const QSize imageSize{ QSize(64, 64) };
	QString nodeTitle {""};

	QPen* qPen{nullptr};
	
};

