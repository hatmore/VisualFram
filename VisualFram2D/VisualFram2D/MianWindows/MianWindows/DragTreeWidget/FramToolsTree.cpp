#include "FramToolsTree.h"
#include <QHeaderView>
#include <QStyleFactory>
#include <QHBoxLayout>

FramToolsTree::FramToolsTree(QWidget *parent)
	: QWidget(parent)
{
	QHBoxLayout* v_layout = new QHBoxLayout();
	v_layout->setAlignment(Qt::AlignCenter);
	v_layout->setContentsMargins(2, 2, 0, 3); // 设置上、下、左、右边距
	toolTreeWidget = new DragTreeWidget();
	v_layout->addWidget(toolTreeWidget);
	this->setLayout(v_layout);

	toolTreeWidget->setIconSize(QSize(30, 30));
	////显示虚线
	//toolTreeWidget->setStyle(QStyleFactory::create("windows"));
	//更换系统默认图标
	toolTreeWidget->setStyleSheet("QTreeWidget::branch:has-children:!has-siblings:closed,\
	QWidget{border: 1px;border-style: solid;border-color: #d9d9d9}\
	QTreeWidget::item{color: #363636}\
	QTreeWidget::item{height: 40px}\
    QTreeView::item::selected{background-color:#96B5A8}\
	QTreeWidget::branch:closed:has-children:!has-siblings{border-style: none; border-image: none; image: url(:/MianWindows/image/unfold.png);}\
	QTreeWidget::branch:closed:has-children:has-siblings{border-image: none; image: url(:/MianWindows/image/unfold.png);}\
	QTreeWidget::branch:open:has-children:!has-siblings{border-image: none; image: url(:/MianWindows/image/open.png);}\
	QTreeWidget::branch:open:has-children:has-siblings{border-image: none; image: url(:/MianWindows/image/open.png);}"
		"QScrollBar:vertical{" //设置滚动条样式
		"background:#FFFFF2;"  //背景色
		"padding-top:20px;"    //上预留位置(放置向上箭头）
		"padding-bottom:20px;" //下预留位置(放置向下箭头）
		"padding-left:2px;"    //左预留位置
		"padding-right:2px;"   //右预留位置
		"border-left:1px solid #d7d7d7;}" //左分割线
		"QScrollBar::handle:vertical{" //滑块样式
		"background:#dbdbdb;"  //滑块颜色
		"border-radius:6px;"   //边角圆
		"min-height:20px;}"    //滑块最小高度
		"QScrollBar::handle:vertical:hover{" //鼠标触及滑块样式
		"background:#d0d0d0;}" //滑块颜色
		"QScrollBar::add-line:vertical{" //向下箭头样式
		"background:url(:/res/ico/down.png) center no-repeat;}"
		"QScrollBar::sub-line:vertical{" //向上箭头样式
		"background:url(:/res/ico/up.png) center no-repeat;}");
	//设置字体及字体大小
	QFont font("Microsoft YaHei");
	font.setPixelSize(18);
	toolTreeWidget->setFont(font);
	toolTreeWidget->header()->setVisible(false);
	toolTreeWidget->clear();

	InitialFramToolsTree();
}

FramToolsTree::~FramToolsTree()
{}

void FramToolsTree::InitialFramToolsTree()
{
	auto map_function_plugin = PluginsManager::PluginsManagerInstance()->mapPtrInterfacePlugins;
	for (auto iter = map_function_plugin.begin(); iter != map_function_plugin.end(); ++iter) {	
		QString name = iter->first;
		std::shared_ptr<InterfacePlugin> ptr = iter->second;
		QTreeWidgetItem* item = new QTreeWidgetItem(toolTreeWidget);
		//item->setBackgroundColor(0, QColor(222,222,222));
		QIcon icon = ptr->ThisIcon();
		item->setIcon(0, QIcon(icon));
		item->setText(0, name);
	}
}