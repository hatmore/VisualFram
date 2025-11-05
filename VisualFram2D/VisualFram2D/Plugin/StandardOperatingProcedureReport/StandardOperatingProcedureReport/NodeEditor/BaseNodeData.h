#pragma once
#include <iostream>
#include <memory>
#include <vector>>
#include <QtNodes/NodeData.hpp>
#include "../GeneralStruct.h"

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class BaseNodeData : public NodeData
{
public:
    BaseNodeData() { ; }
    using Ptr = std::shared_ptr<BaseNodeData>;
    NodeDataType type() const override {
        if (ptrVMObjectNodeFlow) {
            return ptrVMObjectNodeFlow->nodeDataType;
        }
        else {
            return nodeDataType;
        }
    }

    /// <summary>
    /// 判断是否符合输入类型
    /// </summary>
    /// <param name="node_data"></param>
    /// <returns></returns>
    bool sameType(NodeData const& node_data) const override {
        for (auto iter = vScopeNodeDataType.begin(); iter != vScopeNodeDataType.end(); iter++) {
            NodeDataType node_type = node_data.type();
            if (node_type.id == iter->id) {
                return true;
            }
        }
        return false;
    }

    /// <summary>
    /// 添加该节点可输入类型
    /// </summary>
    /// <param name="data_type"></param>
    void AddScopeNodeDataType(const NodeDataType& data_type) {
        vScopeNodeDataType.emplace_back(data_type);
    }

    /// <summary>
    /// 设置该节点ID
    /// </summary>
    /// <param name="id"></param>
    void SetNodeId(const QtNodes::NodeId& id) {
        this->nodeId = id;
        ptrVMObjectNodeFlow->nodeID = id;
    }
    /// <summary>
    /// 输出该节点ID
    /// </summary>
    /// <returns></returns>
    const QtNodes::NodeId& GetNodeId()const {
        return this->nodeId;
    }

    template<typename T>
    std::shared_ptr<T> GetCGObjectNodeFlow() {
        auto ptr = std::dynamic_pointer_cast<T>(ptrVMObjectNodeFlow);
        return ptr;
    }

    void SetCGObjectNodeFlow(const std::shared_ptr<VMObject>& ptr) {
        ptrVMObjectNodeFlow = ptr;
    }

private:
    QtNodes::NodeDataType nodeDataType{ NodeDataType{ "0", "null"} };
    QtNodes::NodeId nodeId{ 0 };                                  //节点ID号
    std::vector<NodeDataType> vScopeNodeDataType;
    std::shared_ptr<VMObject> ptrVMObjectNodeFlow{ nullptr };   //每个节点的数据
};

class MultiInputNodeData :public BaseNodeData
{
public:
    using Ptr = std::shared_ptr<MultiInputNodeData>;
    MultiInputNodeData() {
        ptrVMVectorObjectNodeFlow = std::make_shared<VMVector<PtrVMObject>>();
    }
    ~MultiInputNodeData() {
    }
    NodeDataType type() const override {
        if (ptrVMVectorObjectNodeFlow) {
            return ptrVMVectorObjectNodeFlow->nodeDataType;
        }
        else {
            return nodeDataType;
        }
    }

    bool sameType(NodeData const& node_data) const override {
        for (auto iter = vScopeNodeDataType.begin(); iter != vScopeNodeDataType.end(); iter++) {
            NodeDataType node_type = node_data.type();
            if (node_type.id == iter->id) {
                return true;
            }
        }
        return false;
    }

    /// <summary>
    /// 添加该节点可输入类型
    /// </summary>
    /// <param name="data_type"></param>
    void AddScopeNodeDataType(const NodeDataType& data_type) {
        vScopeNodeDataType.emplace_back(data_type);
    }

    /// <summary>
    /// 设置该节点ID
    /// </summary>
    /// <param name="id"></param>
    void SetNodeId(const QtNodes::NodeId& id) {
        this->nodeId = id;
        (*ptrVMVectorObjectNodeFlow->vmVector.end())->nodeID = id;
    }
    /// <summary>
    /// 输出该节点ID
    /// </summary>
    /// <returns></returns>
    const QtNodes::NodeId& GetNodeId()const {
        return this->nodeId;
    }

    template<typename T>
    std::shared_ptr<T> GetCGObjectNodeFlow() {
        auto ptr = std::dynamic_pointer_cast<T>(ptrVMVectorObjectNodeFlow);
        return ptr;
    }

    void SetCGObjectNodeFlow(const std::shared_ptr<VMObject>& ptr) {
        ptrVMVectorObjectNodeFlow->vmVector.push_back(ptr);
    }

private:
    QtNodes::NodeDataType nodeDataType{ NodeDataType{ "0", "null"} };
    QtNodes::NodeId nodeId{ 0 };                                  //节点ID号
    std::vector<NodeDataType> vScopeNodeDataType;
    std::shared_ptr<VMVector<PtrVMObject>> ptrVMVectorObjectNodeFlow{ nullptr };   //每个节点的数据
};

