#include "YoloV11PoseNodeModel.h"

YoloV11PoseNodeModel::YoloV11PoseNodeModel()
{
    toolInterface = new FramYoloV11Pose();
    nodeEmbedWidget = new NodeEmbedWidget();
    FramYoloV11Pose* fram_dialog = dynamic_cast<FramYoloV11Pose*>(toolInterface);

    mapCGObjectFlowOutNodeAndName = fram_dialog->mapCGObjectFlowOutNodeAndName;
    mapCGObjectFlowInNodeAndName = fram_dialog->mapCGObjectFlowInNodeAndName;

    connect(fram_dialog, &FramYoloV11Pose::SendNodeRuningStateAsychSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot, Qt::BlockingQueuedConnection);
    connect(fram_dialog, &FramYoloV11Pose::SendNodeRuningStateSyncSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot);

    connect(this, &YoloV11PoseNodeModel::SendNodeBindingSucceedSig, fram_dialog, &FramYoloV11Pose::AcceptNodeBindingSucceedSlot);

    QIcon icon = QIcon(":/FramYoloV11Pose/Image/YoloPose.png");
    nodeEmbedWidget->SetImageNode(icon);

    thisName = "关键点检测模型";
    thisCaption = "关键点检测模型";
    inPortNumber = mapCGObjectFlowInNodeAndName.size();
    outPortNumber = mapCGObjectFlowOutNodeAndName.size();

}

YoloV11PoseNodeModel::~YoloV11PoseNodeModel()
{


}