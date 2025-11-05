#include "YoloV11SegmentNodeModel.h"

YoloV11SegmentNodeModel::YoloV11SegmentNodeModel()
{
    toolInterface = new FramYoloV11Segment();
    nodeEmbedWidget = new NodeEmbedWidget();
    FramYoloV11Segment* fram_dialog = dynamic_cast<FramYoloV11Segment*>(toolInterface);

    mapCGObjectFlowOutNodeAndName = fram_dialog->mapCGObjectFlowOutNodeAndName;
    mapCGObjectFlowInNodeAndName = fram_dialog->mapCGObjectFlowInNodeAndName;

    connect(fram_dialog, &FramYoloV11Segment::SendNodeRuningStateAsychSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot, Qt::BlockingQueuedConnection);
    connect(fram_dialog, &FramYoloV11Segment::SendNodeRuningStateSyncSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot);

    connect(this, &YoloV11SegmentNodeModel::SendNodeBindingSucceedSig, fram_dialog, &FramYoloV11Segment::AcceptNodeBindingSucceedSlot);

    QIcon icon = QIcon(":/FramYoloV11Segment/Image/YoloSegment.png");
    nodeEmbedWidget->SetImageNode(icon);

    thisName = "分割检测模型";
    thisCaption = "分割检测模型";
    inPortNumber = mapCGObjectFlowInNodeAndName.size();
    outPortNumber = mapCGObjectFlowOutNodeAndName.size();

}

YoloV11SegmentNodeModel::~YoloV11SegmentNodeModel()
{


}