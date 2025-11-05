#include"ImageDurationNodeModel.h"

ImageDurationNodeModel::ImageDurationNodeModel()
{   
    toolInterface = new FramImageDuration();
    nodeEmbedWidget = new NodeEmbedWidget();
    FramImageDuration* fram_dialog = dynamic_cast<FramImageDuration*>(toolInterface);

    mapCGObjectFlowOutNodeAndName = fram_dialog->mapCGObjectFlowOutNodeAndName;
    mapCGObjectFlowInNodeAndName = fram_dialog->mapCGObjectFlowInNodeAndName;

    connect(fram_dialog, &FramImageDuration::SendNodeRuningStateAsychSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot, Qt::BlockingQueuedConnection);
    connect(fram_dialog, &FramImageDuration::SendNodeRuningStateSyncSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot);
    connect(this, &ImageDurationNodeModel::SendNodeBindingSucceedSig, fram_dialog, &FramImageDuration::AcceptNodeBindingSucceedSlot); 

    //设置节点基本信息
    QIcon icon = QIcon(":/FramImageDuration/Image/filter.png");// 设置节点图标
    nodeEmbedWidget->SetImageNode(icon);

    thisName = "持续时间";
    thisCaption = "持续时间";
    inPortNumber = mapCGObjectFlowInNodeAndName.size();//输入节点数量
    outPortNumber = mapCGObjectFlowOutNodeAndName.size();//输出节点数量

}

ImageDurationNodeModel::~ImageDurationNodeModel()
{

}