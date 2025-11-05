#pragma once
#include <QUndoCommand>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsScene>
#include <QtNodes/BasicGraphicsScene.hpp>
#include <QtNodes/DataFlowGraphModel.hpp>

#include "OverrideDataFlowGraphModel.h"

namespace QtNodes {

    /// @brief An advanced scene working with data-propagating graphs.
    /**
     * The class represents a scene that existed in v2.x but built wit the
     * new model-view approach in mind.
     */
    class  DataFlowGraphicsSceneOverride : public BasicGraphicsScene
    {
        Q_OBJECT
    public:
        DataFlowGraphicsSceneOverride(DataFlowGraphModelOverride& graphModel, QObject* parent = nullptr);

        ~DataFlowGraphicsSceneOverride() = default;

    public:
        std::vector<NodeId> selectedNodes() const;

    public:
        QMenu* createSceneMenu(QPointF const scenePos) override;

    protected:
       ///// <summary>
       ///// 拖拽接收事件
       ///// </summary>
       ///// <param name="event"></param>
       //virtual void dragEnterEvent(QGraphicsSceneDragDropEvent* event)override;
       //virtual void dropEvent(QGraphicsSceneDragDropEvent* event)override;
       //virtual void dragMoveEvent(QGraphicsSceneDragDropEvent* event)override;

    public Q_SLOTS:
        bool save() const;

        bool load();

    Q_SIGNALS:
        void sceneLoaded();

    private:
        DataFlowGraphModelOverride& _graphModel;
    };

    static void insertSerializedItems(QJsonObject const& json, BasicGraphicsScene* scene);

    class CreateCommand : public QUndoCommand
    {
    public:
        CreateCommand(BasicGraphicsScene* scene, QString const name, QPointF const& mouseScenePos);

        void undo() override;
        void redo() override;

    private:
        BasicGraphicsScene* _scene;
        NodeId _nodeId;
        QJsonObject _sceneJson;
    };

} // namespace QtNodes


