#include "NodeEmbedWidget.h"
NodeEmbedWidget::NodeEmbedWidget(QWidget* parent)
	: QWidget(parent)
{
	FramStandardOperatingProcedureReport();
}

void NodeEmbedWidget::FramStandardOperatingProcedureReport()
{
	QString qlabel_style1(
		"QLabel{"
		"border: none;"
		"color: rgb(0, 0, 125);"
		"font:bold normal 8pt;"
		"font-family:'Times New Roman,华文行楷';"
		"}"
	);

	QString qlabel_style2(
		"QLabel{"
		"border: none;"
		"color: rgb(0, 122, 0);"
		"font:bold normal 6pt;"
		"font-family:'Times New Roman,华文行楷';"
		"}"
	);

	qPen = new QPen();
	qPen->setWidth(1);
	qPen->setColor(QColor(122, 122, 122));
	qPen->setStyle(Qt::DotLine);

	qVBoxLayout = new QVBoxLayout(this);
	qVBoxLayout->setContentsMargins(2, 2, 2, 2);
	qVBoxLayout->setSpacing(0);

	titleQLabel = new QLabel();
	titleQLabel->setStyleSheet(qlabel_style1);
	titleQLabel->setAlignment(Qt::AlignCenter);
	qVBoxLayout->addWidget(titleQLabel);
	//qVBoxLayout->addStretch(1);

	imageQLabel = new QLabel();
	imageQLabel->setAlignment(Qt::AlignCenter);
	qVBoxLayout->addWidget(imageQLabel);
	QIcon icon = QIcon(":/FramExternalDeviceWrite/Image/write.png");
	QPixmap m_pic = icon.pixmap(icon.actualSize(imageSize));//size自行调整
	imageQLabel->setMargin(1);
	imageQLabel->setPixmap(m_pic);
	titleQLabel->setText("工具名");
	//qVBoxLayout->addStretch(1);

	timeQLbale = new QLabel();
	timeQLbale->setStyleSheet(qlabel_style2);
	timeQLbale->setAlignment(Qt::AlignCenter);
	qVBoxLayout->addWidget(timeQLbale);
	timeQLbale->setText("耗时:00.00ms");

	timeQLbale->adjustSize();
	timeQLbale->setWordWrap(true);
	titleQLabel->adjustSize();
	titleQLabel->setWordWrap(true);
	imageQLabel->adjustSize();
}

void NodeEmbedWidget::SetImageNode(const QIcon& icon)
{
	QPixmap m_pic = icon.pixmap(icon.actualSize(imageSize));//size自行调整
	imageQLabel->setMargin(1);
	imageQLabel->setPixmap(m_pic);
}

void NodeEmbedWidget::ModifyNodeTitle(const QString& tile)
{
	this->nodeTitle = tile;
	titleQLabel->setText(tile);
}

void NodeEmbedWidget::ModifyNodeTitle(const unsigned int& node_id, const QString& tile)
{
	this->nodeTitle = tile;
	unsigned int id = node_id;
	QString str = QString::number(id) + ":" + tile;
	titleQLabel->setText(str);
}

void NodeEmbedWidget::ModifyNodeTime(const float& time)
{
	QString str = "耗时:" + QString::number(time, 'f', 2) + "ms";
	timeQLbale->setText(str);
}

void NodeEmbedWidget::AcceptNodeStateSlot(const float& time, const RUNNINGSTATE& run_state)
{
	switch (run_state)
	{
	case RUNNING_SUCESS: {
		qPen->setWidth(1);
		qPen->setColor(QColor(122, 122, 122));
		qPen->setStyle(Qt::DotLine);
	}
					   break;
	case RUNNING_FAIL: {
		qPen->setColor(QColor(255, 0, 0));
		qPen->setWidth(2);
		qPen->setStyle(Qt::SolidLine);
	}
					 break;
	case DATA_NULL: {
		qPen->setColor(QColor(0, 0, 255));
		qPen->setWidth(2);
		qPen->setStyle(Qt::SolidLine);
	}
				  break;
	case PARAM_EINVAL: {
		qPen->setColor(QColor(0, 255, 255));
		qPen->setWidth(2);
		qPen->setStyle(Qt::SolidLine);
	}
					 break;
	default:
		break;
	}
	ModifyNodeTime(time);
	this->update();
}

void NodeEmbedWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setPen(*qPen);
	painter.drawRect(1, 1, width() - 2, height() - 2); //绘制边框
}