#pragma once

#include "QtNodes/AbstractGraphModel.hpp"
#include "QtNodes/ConnectionIdUtils.hpp"
#include "QtNodes/NodeDelegateModelRegistry.hpp"
#include "QtNodes/Serializable.hpp"
#include "QtNodes/StyleCollection.hpp"
#include "BaseNodeModel.h"
#include "../GeneralStruct.h"
#include "../Toolnterface/ToolNodeDataInteract.h"
#include <QJsonObject>
#include <memory>

namespace QtNodes {

    class DataFlowGraphModelOverride : public AbstractGraphModel, public Serializable
    {
        Q_OBJECT

    public:
        struct NodeGeometryData
        {
            QSize size;
            QPointF pos;
        };

    public:
        DataFlowGraphModelOverride(std::shared_ptr<NodeDelegateModelRegistry> registry);

        std::shared_ptr<NodeDelegateModelRegistry> dataModelRegistry() { return _registry; }

    public:
        /// <summary>
        /// 设置子节点与主界面数据交互指针
        /// </summary>
        /// <param name="ptr">数据交互指针</param>
        void SetToolNodeDataInteract(ToolNodeDataInteract::Ptr ptr) {
            toolNodeDataInteract = ptr;
        }

        /// <summary>
        /// 设置子节点与主界面数据交互指针
        /// </summary>
        /// <param name="ptr">数据交互指针</param>
        void SetToolNodeMapData(std::shared_ptr<std::unordered_map<int, std::vector<PtrVMObject>>> ptr) {
            ptrMapNodesOutData = ptr;
        }

    public:
        std::unordered_set<NodeId> allNodeIds() const override;

        std::unordered_set<ConnectionId> allConnectionIds(NodeId const nodeId) const override;

        std::unordered_set<ConnectionId> connections(NodeId nodeId,
            PortType portType,
            PortIndex portIndex) const override;

        bool connectionExists(ConnectionId const connectionId) const override;

        NodeId addNode(QString const nodeType) override;

        NodeId addNode(std::unique_ptr<NodeDelegateModel> & model);

        bool connectionPossible(ConnectionId const connectionId) const override;

        void addConnection(ConnectionId const connectionId) override;

        bool nodeExists(NodeId const nodeId) const override;

        QVariant nodeData(NodeId nodeId, NodeRole role) const override;

        NodeFlags nodeFlags(NodeId nodeId) const override;

        bool setNodeData(NodeId nodeId, NodeRole role, QVariant value) override;

        QVariant portData(NodeId nodeId,
            PortType portType,
            PortIndex portIndex,
            PortRole role) const override;

        bool setPortData(NodeId nodeId,
            PortType portType,
            PortIndex portIndex,
            QVariant const& value,
            PortRole role = PortRole::Data) override;

        bool deleteConnection(ConnectionId const connectionId) override;

        bool deleteNode(NodeId const nodeId) override;

        QJsonObject saveNode(NodeId const) const override;

        QJsonObject save() const override;

        void loadNode(QJsonObject const& nodeJson) override;

        void load(QJsonObject const& json) override;

        /**
       * Fetches the NodeDelegateModel for the given `nodeId` and tries to cast the
       * stored pointer to the given type
       */
        template<typename NodeDelegateModelType>
        NodeDelegateModelType* delegateModel(NodeId const nodeId)
        {
            auto it = _models.find(nodeId);
            if (it == _models.end())
                return nullptr;

            auto model = dynamic_cast<NodeDelegateModelType*>(it->second.get());

            return model;
        }

    Q_SIGNALS:
        void inPortDataWasSet(NodeId const, PortType const, PortIndex const);

    private:
        NodeId newNodeId() override { return _nextNodeId++; }

        void sendConnectionCreation(ConnectionId const connectionId);

        void sendConnectionDeletion(ConnectionId const connectionId);


signals:
    /// <summary>
    /// 同步信号
    /// </summary>
    void SendLogInfoSynSig(const QString& info, const LOGTYPE type);

    private Q_SLOTS:
        /**
       * Fuction is called in three cases:
       *
       * - By underlying NodeDelegateModel when a node has new data to propagate.
       *   @see DataFlowGraphModel::addNode
       * - When a new connection is created.
       *   @see DataFlowGraphModel::addConnection
       * - When a node restored from JSON an needs to send data downstream.
       *   @see DataFlowGraphModel::loadNode
       */
        void onOutPortDataUpdated(NodeId const nodeId, PortIndex const portIndex);

        /// Function is called after detaching a connection.
        void propagateEmptyDataTo(NodeId const nodeId, PortIndex const portIndex);

    private:
        std::shared_ptr<NodeDelegateModelRegistry> _registry;

        NodeId _nextNodeId;

        std::unordered_map<NodeId, std::unique_ptr<NodeDelegateModel>> _models;

        std::unordered_set<ConnectionId> _connectivity;

        mutable std::unordered_map<NodeId, NodeGeometryData> _nodeGeometryData;

        /// <summary>
        /// 获得交互指针
        /// </summary>
        ToolNodeDataInteract::Ptr toolNodeDataInteract{ nullptr };
        /// <summary>
        /// 自身流程的节点数据
        /// </summary>
        std::shared_ptr<std::unordered_map<int, std::vector<PtrVMObject>>> ptrMapNodesOutData{ nullptr };
    };

} // namespace QtNodes
