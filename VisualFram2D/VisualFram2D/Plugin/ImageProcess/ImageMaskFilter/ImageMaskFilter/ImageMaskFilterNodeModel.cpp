#include "ImageMaskFilterNodeModel.h"

ImageMaskFilterNodeModel::ImageMaskFilterNodeModel()
{
    toolInterface = new FramImageMaskFilter();
    nodeEmbedWidget = new NodeEmbedWidget();
    FramImageMaskFilter* fram_dialog = dynamic_cast<FramImageMaskFilter*>(toolInterface);

    mapCGObjectFlowOutNodeAndName = fram_dialog->mapCGObjectFlowOutNodeAndName;
    mapCGObjectFlowInNodeAndName = fram_dialog->mapCGObjectFlowInNodeAndName;

    connect(fram_dialog, &FramImageMaskFilter::SendNodeRuningStateAsychSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot, Qt::BlockingQueuedConnection);
    connect(fram_dialog, &FramImageMaskFilter::SendNodeRuningStateSyncSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot);

    connect(this, &ImageMaskFilterNodeModel::SendNodeBindingSucceedSig, fram_dialog, &FramImageMaskFilter::AcceptNodeBindingSucceedSlot);

    QIcon icon = QIcon(":/FramImageMaskFilter/Image/ImageMaskFilter.png");
    nodeEmbedWidget->SetImageNode(icon);

    thisName = "Í¼ÏñÑÚÄ¤";
    thisCaption = "Í¼ÏñÑÚÄ¤";
    inPortNumber = mapCGObjectFlowInNodeAndName.size();
    outPortNumber = mapCGObjectFlowOutNodeAndName.size();

}

ImageMaskFilterNodeModel::~ImageMaskFilterNodeModel()
{


}