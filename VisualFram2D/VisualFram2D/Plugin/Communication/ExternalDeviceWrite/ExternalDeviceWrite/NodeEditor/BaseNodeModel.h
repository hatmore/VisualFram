#pragma once
#pragma execution_character_set("utf-8")
#include <QtNodes/NodeDelegateModel.hpp>
#include <QtCore/QObject>
#include <QIcon>
#include <iostream>
#include <QMessageBox>
#include "NodeEmbedWidget.h"
#include "BaseNodeData.h"
#include "../Toolnterface/ToolInterface.h"

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

struct FlowNodeAndName {
    std::shared_ptr<BaseNodeData>ptrNodedata; //节点数据
    QString nodeName;               //节点名称
};

class BaseNodeModel : public NodeDelegateModel
{
    Q_OBJECT
public:
    BaseNodeModel() {
        connect(this, &BaseNodeModel::SendNodeStyleErrorSig, []() {
            QMessageBox::critical(NULL, "报错",
                "节点参数类型错误", QMessageBox::Yes, QMessageBox::Yes);
            });
    }


    BaseNodeModel(const QString& name, const QString& caption_name) :thisName(name), thisCaption(caption_name) { ; }

    BaseNodeModel(const QString& name, const QString& caption_name, const uint in_port_number, const uint out_port_number)
        :thisName(name), thisCaption(caption_name), inPortNumber(in_port_number), outPortNumber(out_port_number) {
        ;
    }

    ~BaseNodeModel() {
        if (toolInterface) {
            delete toolInterface;
            toolInterface = nullptr;
        }
    }

public:
    /// <summary>
    /// 模块显示名称
    /// </summary>
    /// <returns></returns>
    QString caption() const override {
        return this->thisCaption;
    }
    /// <summary>
    /// 模块是否显示名称，true为显示，false为不显示
    /// </summary>
    /// <returns></returns>
    virtual bool captionVisible() const override {
        return false;
    }

    /// <summary>
    /// 模块名
    /// </summary>
    /// <returns></returns>
    QString name() const override {
        return this->thisName;
    }

public:
    /// <summary>
    /// 输入、输出参数节点数量
    /// </summary>
    virtual QtNodes::ConnectionPolicy portConnectionPolicy(PortType portType, PortIndex portIndex) const  override {
        switch (portType) {
        case PortType::In: {
            if (portIndex == 1) {
                return  QtNodes::ConnectionPolicy::One;
            }
            else {
                return  QtNodes::ConnectionPolicy::Many;
            }
        }
                         break;
        case PortType::Out: {
            return  QtNodes::ConnectionPolicy::Many;
        }
                          break;
        default:
            break;
        }
        return  QtNodes::ConnectionPolicy::One;
    }

public:
    //序列化
    virtual QJsonObject save() const override
    {
        QJsonObject modelJson;
        modelJson["model-name"] = name();
        modelJson["nodeTitle"] = nodeEmbedWidget->GetNodeTitle();
        QJsonObject serialize = toolInterface->SerializeFunciton();
        modelJson["SerializeFunciton"] = serialize;

        return modelJson;
    }
    //反序列化
    virtual void load(QJsonObject const& json) override {
        QString model_name = json["model-name"].toString();
        QString node_title = json["nodeTitle"].toString();
        QJsonObject exserialize = json["SerializeFunciton"].toObject();
        toolInterface->ExserializeFunciton(exserialize);
        nodeEmbedWidget->ModifyNodeTitle(this->nodeId, node_title);
        //std::cout << model_name.toStdString() << std::endl;
    }
    /// <summary>
    /// 获得节点编号
    /// </summary>
    /// <returns></returns>
    virtual QtNodes::NodeId GetNodeId() const {
        return this->nodeId;
    }
    /// <summary>
    /// 设置节点编号
    /// </summary>
    /// <param name=""></param>
    virtual void SetNodeId(const QtNodes::NodeId& node_id) {
        this->nodeId = node_id;
        toolInterface->SetNodeId(node_id);
        SetBaseNodeDataId(node_id);
        if (nodeEmbedWidget) {
            unsigned int id = node_id;
            QString str = this->thisName;
            nodeEmbedWidget->ModifyNodeTitle(id, str);
        }
    }

public:
    /// <summary>
    /// 设置输入和输出端口数量
    /// </summary>
    /// <param name="portType"> 输入或者输出</param>
    /// <returns> 返回输出或者输入数量 </returns> 
    virtual unsigned int nPorts(PortType portType) const override {
        unsigned int result;
        if (portType == PortType::In)
            result = inPortNumber;
        else
            result = outPortNumber;
        return result;
    };

    /// <summary>
    /// 输入输出端口名称
    /// </summary>
    /// <param name="portType"> 输入或者输出</param>
    /// <param name="portIndex">端口号</param>
    /// <returns> 端口名称 </returns>
    virtual QString portCaption(PortType portType, PortIndex portIndex) const override {
        // if(portType == )
        switch (portType) {
        case PortType::In: {
            auto iter = mapCGObjectFlowInNodeAndName.find(portIndex);
            return iter->second.nodeName;
        }
                         break;
        case PortType::Out: {
            auto iter = mapCGObjectFlowOutNodeAndName.find(portIndex);
            return iter->second.nodeName;
        }
                          break;
        default:
            break;
        }
        return QString();
    }

    /// <summary>
    /// 设置端口是否能被显示 
    /// </summary>
    /// <param name="portType"> 输入或者输出 </param>
    /// <param name="portIndex"> 端口号 </param>
    /// <returns> 是否被显示 </returns>
    virtual bool portCaptionVisible(PortType portType, PortIndex portIndex) const override {
        if (portType == PortType::In)
            if (portIndex == 0) {
                return true;
            }
        return true;
    }

    /// <summary>
    /// 输入输出端口数据类型
    /// </summary>
    /// <param name="portType">输入或者输出端口</param>
    /// <param name="portIndex">端口号</param>
    /// <returns> 数据类型 </returns>
    virtual NodeDataType dataType(PortType portType, PortIndex portIndex) const override {
        return BaseNodeData().type();
    };

    /// <summary>
    /// 输入和输出端口
    /// </summary>
    /// <param name="port"></param>
    /// <returns></returns>
    virtual std::shared_ptr<NodeData> outData(PortIndex port) override {
        auto ptr_node = FindMapCGObjectFlowNode(mapCGObjectFlowOutNodeAndName, port);
        //todo
        ptr_node->SetNodeId(this->nodeId);
        // ptrResult = std::make_shared<BaseNodeData>();
        return std::static_pointer_cast<NodeData>(ptr_node);
    }

    /// <summary>
    /// 输入数据接口
    /// </summary>
    /// <param name="data"> 输入数据，需要内部转化为子类 </param>
    /// <param name="portIndex"> 模块输入的ID号，第几个端口 </param>
    virtual void setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) override {
        Q_UNUSED(portIndex);
        auto ptr_node = FindMapCGObjectFlowNode(mapCGObjectFlowInNodeAndName, portIndex);
        if (data == nullptr) {
            if (portIndex == 1) {
                ptr_node->SetCGObjectNodeFlow(nullptr);
            }
            else {
                auto ptr_multi_node = std::dynamic_pointer_cast<MultiInputNodeData>(ptr_node);
            }
            return;
        }

        auto in_node_data = std::dynamic_pointer_cast<BaseNodeData>(data);
        std::shared_ptr<VMObject> ptr_data = in_node_data->GetCGObjectNodeFlow<VMObject>();
        if (portIndex == 1) {
            //判断输入类型是否符合条件
            bool is = ptr_node->sameType(*in_node_data);
            if (!is) {
                emit portsAboutToBeDeleted(PortType::In, portIndex, portIndex);
                emit SendNodeStyleErrorSig();
                return;
            }
            ptr_node->SetCGObjectNodeFlow(ptr_data);
            QtNodes::NodeId id = in_node_data->GetNodeId();
            ptr_node->SetNodeId(id);
        }
        else {
            auto ptr_multi_node = std::dynamic_pointer_cast<MultiInputNodeData>(ptr_node);
            bool is = ptr_multi_node->sameType(*in_node_data);
            if (!is) {
                emit portsAboutToBeDeleted(PortType::In, portIndex, portIndex);
                emit SendNodeStyleErrorSig();
                return;
            }
            ptr_multi_node->SetCGObjectNodeFlow(ptr_data);
        }
        emit SendNodeBindingSucceedSig(portIndex);
    }

    /// <summary>
    /// 设置模块中间显示部分，用label显示图像
    /// </summary>
    /// <returns></returns>
    virtual QWidget* embeddedWidget() override {
        return nodeEmbedWidget;
    }

    /// <summary>
    /// 获得控件的指针
    /// </summary>
    /// <returns></returns>
    virtual ToolInterface* GetToolInterface() {
        auto ptr = static_cast<ToolInterface*>(toolInterface);
        return ptr;
    }

Q_SIGNALS:
    //节点参数错误，发送信号
    void SendNodeStyleErrorSig();
    /// <summary>
    /// 发送节点绑定完成信号
    /// </summary>
    /// <param name="portIndex"> 节点绑定处ID</param>
    void SendNodeBindingSucceedSig(const PortIndex port_index);

protected:
    inline std::shared_ptr<BaseNodeData> FindMapCGObjectFlowNode(const std::unordered_map<PortIndex, FlowNodeAndName>& map_node,
        const PortIndex& port_index) {
        auto iter = map_node.find(port_index);
        if (iter == map_node.end())
            return nullptr;
        auto ptr = std::static_pointer_cast<BaseNodeData>(iter->second.ptrNodedata);
        return ptr;
    }

    inline void InsertMapCGObjectFlowNode(std::unordered_map<PortIndex, FlowNodeAndName>& map_node,
        const std::pair<PortIndex, FlowNodeAndName>& node) {
        PortIndex index = node.first;
        auto iter = map_node.find(index);
        if (iter != map_node.end()) {
            map_node.erase(iter);
            map_node.insert(node);
        }
        else {
            map_node.insert(node);
        }
    }
    /// <summary>
    /// 设置节点数据ID，node的节点
    /// </summary>
    /// <param name="node_id"></param>
    inline void SetBaseNodeDataId(const QtNodes::NodeId node_id) {
        for (auto node_data : mapCGObjectFlowOutNodeAndName) {
            node_data.second.ptrNodedata->SetNodeId(node_id);
        }
    }

public Q_SLOTS:
    virtual void inputConnectionDeleted(QtNodes::ConnectionId const& index) {
        std::cout << index.inNodeId << std::endl;
        if (index.inPortIndex == 0) {
            auto ptr_node = FindMapCGObjectFlowNode(mapCGObjectFlowInNodeAndName, index.inPortIndex);
            auto ptr_multi_node = std::dynamic_pointer_cast<MultiInputNodeData>(ptr_node);
            auto vmvector_object = ptr_multi_node->GetCGObjectNodeFlow<VMVector<PtrVMObject>>();
            for (size_t i = 0; i < vmvector_object->vmVector.size(); i++) {
                PtrVMObject ptr_object = vmvector_object->vmVector[i];
                unsigned int id = ptr_object->nodeID;
                if (id == index.outNodeId) {
                    vmvector_object->vmVector.erase(vmvector_object->vmVector.begin() + i);
                }
            }
        }
    }

public:
    NodeEmbedWidget* nodeEmbedWidget{ nullptr };
    std::shared_ptr<BaseNodeData> ptrResult;
    ToolInterface* toolInterface{ nullptr };
    /// <summary>
    /// 输出节点的数据和节点名称
    /// </summary>
    std::unordered_map<PortIndex, FlowNodeAndName> mapCGObjectFlowOutNodeAndName;
    /// <summary>
    /// 输入节点的数据和节点名称
    /// </summary>
    std::unordered_map<PortIndex, FlowNodeAndName> mapCGObjectFlowInNodeAndName;

protected:
    QString thisName{ "BaseDataModel" };
    QString thisCaption{ "BaseDataModel" };
    unsigned int inPortNumber{ 1 };
    unsigned int outPortNumber{ 1 };
    QtNodes::NodeId nodeId{ 0 };
};

