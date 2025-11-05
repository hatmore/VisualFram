#include "ImageROISelectNodeModel.h"

ImageROISelectNodeModel::ImageROISelectNodeModel()
{
    
    toolInterface = new FramImageROISelection();
    nodeEmbedWidget = new NodeEmbedWidget();
    FramImageROISelection* fram_dialog = dynamic_cast<FramImageROISelection*>(toolInterface);

    //获取输入 / 输出节点映射表
    mapCGObjectFlowOutNodeAndName = fram_dialog->mapCGObjectFlowOutNodeAndName;
    mapCGObjectFlowInNodeAndName = fram_dialog->mapCGObjectFlowInNodeAndName;

    connect(fram_dialog, &FramImageROISelection::SendNodeRuningStateAsychSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot, Qt::BlockingQueuedConnection);
    connect(fram_dialog, &FramImageROISelection::SendNodeRuningStateSyncSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot);
    connect(this, &ImageROISelectNodeModel::SendNodeBindingSucceedSig, fram_dialog, &FramImageROISelection::AcceptNodeBindingSucceedSlot); //完成的槽函数，用于显示

    //设置节点基本信息
    QIcon icon = QIcon(":/ImageROISelection/Image/region.png");// 设置节点图标
    nodeEmbedWidget->SetImageNode(icon);

    thisName = "图像ROI";
    thisCaption = "图像ROI";
    inPortNumber = mapCGObjectFlowInNodeAndName.size();//输入节点数量
    outPortNumber = mapCGObjectFlowOutNodeAndName.size();//输出节点数量

}

ImageROISelectNodeModel::~ImageROISelectNodeModel()
{


}