#include "ResultStateJudgmentNodeModel.h"

ResultStateJudgmentNodeModel::ResultStateJudgmentNodeModel()
{
    toolInterface = new FramResultStateJudgment();
    nodeEmbedWidget = new NodeEmbedWidget();
    FramResultStateJudgment* fram_dialog = dynamic_cast<FramResultStateJudgment*>(toolInterface);

    mapCGObjectFlowOutNodeAndName = fram_dialog->mapCGObjectFlowOutNodeAndName;
    mapCGObjectFlowInNodeAndName = fram_dialog->mapCGObjectFlowInNodeAndName;

    connect(fram_dialog, &FramResultStateJudgment::SendNodeRuningStateAsychSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot, Qt::BlockingQueuedConnection);
    connect(fram_dialog, &FramResultStateJudgment::SendNodeRuningStateSyncSig, nodeEmbedWidget, &NodeEmbedWidget::AcceptNodeStateSlot);

    connect(this, &ResultStateJudgmentNodeModel::SendNodeBindingSucceedSig, fram_dialog, &FramResultStateJudgment::AcceptNodeBindingSucceedSlot);

    QIcon icon = QIcon(":/FramResultStateJudgment/Image/logicJudgement.png");
    nodeEmbedWidget->SetImageNode(icon);

    thisName = "Âß¼­ÅÐ¶Ï";
    thisCaption = "Âß¼­ÅÐ¶Ï";
    inPortNumber = mapCGObjectFlowInNodeAndName.size();
    outPortNumber = mapCGObjectFlowOutNodeAndName.size();

}

ResultStateJudgmentNodeModel::~ResultStateJudgmentNodeModel()
{


}