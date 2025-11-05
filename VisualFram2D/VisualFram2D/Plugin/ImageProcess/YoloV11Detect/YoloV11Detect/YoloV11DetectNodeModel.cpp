#include "YoloV11DetectNodeModel.h"

YoloV11DetectNodeModel::YoloV11DetectNodeModel()
{
    toolInterface = new FramYoloV11Detect();
    nodeEmbedWidget = new NodeEmbedWidget();
    FramYoloV11Detect* fram_dialog = dynamic_cast<FramYoloV11Detect*>(toolInterface);

    mapCGObjectFlowOutNodeAndName = fram_dialog->mapCGObjectFlowOutNodeAndName;
    mapCGObjectFlowInNodeAndName = fram_dialog->mapCGObjectFlowInNodeAndName;

    connect(fram_dialog, &FramYoloV11Detect::SendNodeRuningStateAsychSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot, Qt::BlockingQueuedConnection);
    connect(fram_dialog, &FramYoloV11Detect::SendNodeRuningStateSyncSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot);

    connect(this, &YoloV11DetectNodeModel::SendNodeBindingSucceedSig, fram_dialog, &FramYoloV11Detect::AcceptNodeBindingSucceedSlot);

    QIcon icon = QIcon(":/FramYoloV11Detect/Image/YoloDetect.png");
    nodeEmbedWidget->SetImageNode(icon);

    thisName = "目标检测模型";
    thisCaption = "目标检测模型";
    inPortNumber = mapCGObjectFlowInNodeAndName.size();
    outPortNumber = mapCGObjectFlowOutNodeAndName.size();

}

YoloV11DetectNodeModel::~YoloV11DetectNodeModel()
{


}