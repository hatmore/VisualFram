#pragma execution_character_set("utf-8")
#include "OverrideDataFlowGraphicsScene.h"
#include "QtNodes/DataFlowGraphicsScene.hpp"

#include "QtNodes/ConnectionGraphicsObject.hpp"
#include "QtNodes/GraphicsView.hpp"
#include "QtNodes/NodeDelegateModelRegistry.hpp"
#include "QtNodes/NodeGraphicsObject.hpp"
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGraphicsSceneMoveEvent>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidgetAction>

#include <QtCore/QBuffer>
#include <QtCore/QByteArray>
#include <QtCore/QDataStream>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QtGlobal>

#include <stdexcept>
#include <utility>

#define DRAG_DATA QStringLiteral("Drag/DragTree")

namespace QtNodes {

    DataFlowGraphicsSceneOverride::DataFlowGraphicsSceneOverride(DataFlowGraphModelOverride& graphModel, QObject* parent)
        : BasicGraphicsScene(graphModel, parent)
        , _graphModel(graphModel)
    {
        connect(&_graphModel,
            &DataFlowGraphModelOverride::inPortDataWasSet,
            [this](NodeId const nodeId, PortType const, PortIndex const) { onNodeUpdated(nodeId); });

    }

    // TODO constructor for an empyt scene?

    std::vector<NodeId> DataFlowGraphicsSceneOverride::selectedNodes() const
    {
        QList<QGraphicsItem*> graphicsItems = selectedItems();

        std::vector<NodeId> result;
        result.reserve(graphicsItems.size());

        for (QGraphicsItem* item : graphicsItems) {
            auto ngo = qgraphicsitem_cast<NodeGraphicsObject*>(item);

            if (ngo != nullptr) {
                result.push_back(ngo->nodeId());
            }
        }

        return result;
    }

    QMenu* DataFlowGraphicsSceneOverride::createSceneMenu(QPointF const scenePos)
    {
        QMenu* modelMenu = new QMenu();
        // Add filterbox to the context menu
        auto* txtBox = new QLineEdit(modelMenu);
        txtBox->setPlaceholderText("搜索");
        txtBox->setClearButtonEnabled(true);

        auto* txtBoxAction = new QWidgetAction(modelMenu);
        txtBoxAction->setDefaultWidget(txtBox);
        txtBox->activateWindow();
        // 1.

        // Add result treeview to the context menu
        QTreeWidget* treeView = new QTreeWidget(modelMenu);
        treeView->header()->close();

        auto* treeViewAction = new QWidgetAction(modelMenu);
        treeViewAction->setDefaultWidget(treeView);

        // 2.
        modelMenu->addAction(treeViewAction);

        auto registry = _graphModel.dataModelRegistry();

        for (auto const& cat : registry->categories()) {
            auto item = new QTreeWidgetItem(treeView);
            item->setText(0, cat);
            item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        }

        for (auto const& assoc : registry->registeredModelsCategoryAssociation()) {
            QList<QTreeWidgetItem*> parent = treeView->findItems(assoc.second, Qt::MatchExactly);

            if (parent.count() <= 0)
                continue;

            auto item = new QTreeWidgetItem(parent.first());
            item->setText(0, assoc.first);
        }

        treeView->expandAll();

        connect(treeView,
            &QTreeWidget::itemClicked,
            [this, modelMenu, scenePos](QTreeWidgetItem* item, int) {
                if (!(item->flags() & (Qt::ItemIsSelectable))) {
                    return;
                }
                /////撤销相关代码，暂时屏蔽，防止节点撤回，参数未撤回情况
               // this->undoStack().push(new CreateCommand(this, item->text(0), scenePos));

                NodeId nodeId = _graphModel.addNode(item->text(0));
                if (nodeId != InvalidNodeId) {
                    _graphModel.setNodeData(nodeId, NodeRole::Position, scenePos);
                }

                modelMenu->close();
            });

        //Setup filtering
        connect(txtBox, &QLineEdit::textChanged, [treeView](const QString& text) {
            QTreeWidgetItemIterator categoryIt(treeView, QTreeWidgetItemIterator::HasChildren);
            while (*categoryIt)
                (*categoryIt++)->setHidden(true);
            QTreeWidgetItemIterator it(treeView, QTreeWidgetItemIterator::NoChildren);
            while (*it) {
                auto modelName = (*it)->text(0);
                const bool match = (modelName.contains(text, Qt::CaseInsensitive));
                (*it)->setHidden(!match);
                if (match) {
                    QTreeWidgetItem* parent = (*it)->parent();
                    while (parent) {
                        parent->setHidden(false);
                        parent = parent->parent();
                    }
                }
                ++it;
            }
            });

        // make sure the text box gets focus so the user doesn't have to click on it
        txtBox->setFocus();

        // QMenu's instance auto-destruction
        modelMenu->setAttribute(Qt::WA_DeleteOnClose);

        return modelMenu;
    }

    bool DataFlowGraphicsSceneOverride::save() const
    {
        QString fileName = QFileDialog::getSaveFileName(nullptr,
            tr("Open Flow Scene"),
            QDir::homePath(),
            tr("Flow Scene Files (*.flow)"));

        if (!fileName.isEmpty()) {
            if (!fileName.endsWith("flow", Qt::CaseInsensitive))
                fileName += ".flow";

            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(QJsonDocument(_graphModel.save()).toJson());
                return true;
            }
        }
        return false;
    }

    bool DataFlowGraphicsSceneOverride::load()
    {
        QString fileName = QFileDialog::getOpenFileName(nullptr,
            tr("Open Flow Scene"),
            QDir::homePath(),
            tr("Flow Scene Files (*.flow)"));

        if (!QFileInfo::exists(fileName))
            return false;

        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly))
            return false;

        clearScene();

        QByteArray const wholeFile = file.readAll();

        _graphModel.load(QJsonDocument::fromJson(wholeFile).object());

        Q_EMIT sceneLoaded();

        return true;
    }

    //void QtNodes::DataFlowGraphicsSceneOverride::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
    //{
    //    if (event->mimeData()->hasFormat(DRAG_DATA)) {
    //        QPoint scene_pos = QCursor::pos();
    //        event->acceptProposedAction();
    //    }
    //}

    //void QtNodes::DataFlowGraphicsSceneOverride::dropEvent(QGraphicsSceneDragDropEvent* event)
    //{
    //    if (event->mimeData()->hasFormat(DRAG_DATA)) {    
    //        QByteArray pieceData = event->mimeData()->data(DRAG_DATA);
    //        QDataStream dataStream(&pieceData, QIODevice::ReadOnly);
    //        QPixmap pixmap;
    //        QString str_text;
    //        dataStream >> pixmap >> str_text;

    //        NodeId nodeId = _graphModel.addNode(str_text);
    //        if (nodeId != InvalidNodeId) {
    //            QPoint scene_pos = QCursor::pos();
    //            _graphModel.setNodeData(nodeId, NodeRole::Position, scene_pos);
    //        }
    //    }
    //}

    //void QtNodes::DataFlowGraphicsSceneOverride::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
    //{
    //    QPoint scene_pos = QCursor::pos();
    //    event->accept();
    //}

    CreateCommand::CreateCommand(BasicGraphicsScene* scene,
        QString const name,
        QPointF const& mouseScenePos)
        : _scene(scene)
        , _sceneJson(QJsonObject())
    {
        _nodeId = _scene->graphModel().addNode(name);
        if (_nodeId != InvalidNodeId) {
            _scene->graphModel().setNodeData(_nodeId, NodeRole::Position, mouseScenePos);
        }
        else {
            setObsolete(true);
        }
    }

    void CreateCommand::undo()
    {
        QJsonArray nodesJsonArray;
        nodesJsonArray.append(_scene->graphModel().saveNode(_nodeId));
        _sceneJson["nodes"] = nodesJsonArray;

        _scene->graphModel().deleteNode(_nodeId);
    }

    void CreateCommand::redo()
    {
        if (_sceneJson.empty() || _sceneJson["nodes"].toArray().empty())
            return;

        insertSerializedItems(_sceneJson, _scene);
    }


    static void insertSerializedItems(QJsonObject const& json, BasicGraphicsScene* scene)
    {
        AbstractGraphModel& graphModel = scene->graphModel();

        QJsonArray const& nodesJsonArray = json["nodes"].toArray();

        for (QJsonValue node : nodesJsonArray) {
            QJsonObject obj = node.toObject();

            graphModel.loadNode(obj);

            auto id = obj["id"].toInt();
            scene->nodeGraphicsObject(id)->setZValue(1.0);
            scene->nodeGraphicsObject(id)->setSelected(true);
        }

        QJsonArray const& connJsonArray = json["connections"].toArray();

        for (QJsonValue connection : connJsonArray) {
            QJsonObject connJson = connection.toObject();

            ConnectionId connId = fromJson(connJson);

            // Restore the connection
            graphModel.addConnection(connId);

            scene->connectionGraphicsObject(connId)->setSelected(true);
        }
    }

} // namespace QtNodes
