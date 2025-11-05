#include "ImageShowNodeModel.h"

ImageShowNodeModel::ImageShowNodeModel()
{
    toolInterface = new FramImageShow();
    nodeEmbedWidget = new NodeEmbedWidget();
    FramImageShow* fram_dialog = dynamic_cast<FramImageShow*>(toolInterface);

    mapCGObjectFlowOutNodeAndName = fram_dialog->mapCGObjectFlowOutNodeAndName;
    mapCGObjectFlowInNodeAndName = fram_dialog->mapCGObjectFlowInNodeAndName;

    connect(fram_dialog, &FramImageShow::SendNodeRuningStateAsychSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot, Qt::BlockingQueuedConnection);
    connect(fram_dialog, &FramImageShow::SendNodeRuningStateSyncSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot);

    connect(this, &ImageShowNodeModel::SendNodeBindingSucceedSig, fram_dialog, &FramImageShow::AcceptNodeBindingSucceedSlot);

    QIcon icon = QIcon(":/FramImageShow/Image/ImageShow.png");
    nodeEmbedWidget->SetImageNode(icon);

    thisName = "Í¼ÏñÏÔÊ¾";
    thisCaption = "Í¼ÏñÏÔÊ¾";
    inPortNumber = mapCGObjectFlowInNodeAndName.size();
    outPortNumber = mapCGObjectFlowOutNodeAndName.size();

}

ImageShowNodeModel::~ImageShowNodeModel()
{


}