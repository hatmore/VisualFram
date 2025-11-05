#include "PoseC3DNodeModel.h"

PoseC3DNodeModel::PoseC3DNodeModel()
{
    toolInterface = new FramPoseC3D();
    nodeEmbedWidget = new NodeEmbedWidget();
    FramPoseC3D* fram_dialog = dynamic_cast<FramPoseC3D*>(toolInterface);

    mapCGObjectFlowOutNodeAndName = fram_dialog->mapCGObjectFlowOutNodeAndName;
    mapCGObjectFlowInNodeAndName = fram_dialog->mapCGObjectFlowInNodeAndName;

    connect(fram_dialog, &FramPoseC3D::SendNodeRuningStateAsychSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot, Qt::BlockingQueuedConnection);
    connect(fram_dialog, &FramPoseC3D::SendNodeRuningStateSyncSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot);

    connect(this, &PoseC3DNodeModel::SendNodeBindingSucceedSig, fram_dialog, &FramPoseC3D::AcceptNodeBindingSucceedSlot);

    QIcon icon = QIcon(":/FramPoseC3D/Image/iconRecognation.png");
    nodeEmbedWidget->SetImageNode(icon);

    thisName = "行为识别模型";
    thisCaption = "行为识别模型";
    inPortNumber = mapCGObjectFlowInNodeAndName.size();
    outPortNumber = mapCGObjectFlowOutNodeAndName.size();

}

PoseC3DNodeModel::~PoseC3DNodeModel()
{


}