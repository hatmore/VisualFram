#include"ImageActionOrderNodeModel.h"

ImageActionOrderNodeModel::ImageActionOrderNodeModel()
{
    
    toolInterface = new FramImageActionOrder();
    nodeEmbedWidget = new NodeEmbedWidget();
    FramImageActionOrder* fram_dialog = dynamic_cast<FramImageActionOrder*>(toolInterface);

    mapCGObjectFlowOutNodeAndName = fram_dialog->mapCGObjectFlowOutNodeAndName;
    mapCGObjectFlowInNodeAndName = fram_dialog->mapCGObjectFlowInNodeAndName;

    connect(fram_dialog, &FramImageActionOrder::SendNodeRuningStateAsychSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot, Qt::BlockingQueuedConnection);
    connect(fram_dialog, &FramImageActionOrder::SendNodeRuningStateSyncSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot);
    connect(this, &ImageActionOrderNodeModel::SendNodeBindingSucceedSig, fram_dialog, &FramImageActionOrder::AcceptNodeBindingSucceedSlot); 

    //设置节点基本信息
    QIcon icon = QIcon(":/FramImageActionOrder/Image/actionOrder.png");// 设置节点图标
    nodeEmbedWidget->SetImageNode(icon);

    thisName = "动作顺序";
    thisCaption = "动作顺序";
    inPortNumber = mapCGObjectFlowInNodeAndName.size();//输入节点数量
    outPortNumber = mapCGObjectFlowOutNodeAndName.size();//输出节点数量

}

ImageActionOrderNodeModel::~ImageActionOrderNodeModel()
{


}