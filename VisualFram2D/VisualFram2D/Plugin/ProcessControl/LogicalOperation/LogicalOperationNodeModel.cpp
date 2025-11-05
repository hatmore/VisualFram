#include "LogicalOperationNodeModel.h"

LogicalOperationNodeModel::LogicalOperationNodeModel()
{
    toolInterface = new FramLogicalOperation();
    nodeEmbedWidget = new NodeEmbedWidget();
    FramLogicalOperation* fram_dialog = dynamic_cast<FramLogicalOperation*>(toolInterface);

    mapCGObjectFlowOutNodeAndName = fram_dialog->mapCGObjectFlowOutNodeAndName;
    mapCGObjectFlowInNodeAndName = fram_dialog->mapCGObjectFlowInNodeAndName;

    connect(fram_dialog, &FramLogicalOperation::SendNodeRuningStateAsychSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot, Qt::BlockingQueuedConnection);
    connect(fram_dialog, &FramLogicalOperation::SendNodeRuningStateSyncSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot);

    connect(this, &LogicalOperationNodeModel::SendNodeBindingSucceedSig, fram_dialog, &FramLogicalOperation::AcceptNodeBindingSucceedSlot);

    QIcon icon = QIcon(":/FramLogicalOperation/Image/LogicalOperation.png");
    nodeEmbedWidget->SetImageNode(icon);

    thisName = "Âß¼­ÔËËã";
    thisCaption = "Âß¼­ÔËËã";
    inPortNumber = mapCGObjectFlowInNodeAndName.size();
    outPortNumber = mapCGObjectFlowOutNodeAndName.size();

}

LogicalOperationNodeModel::~LogicalOperationNodeModel()
{


}