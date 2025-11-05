//#pragma execution_character_set("utf-8")
//#pragma once
//#include <QtCore/QObject>
//#include <QtWidgets/QLabel>
//#include <QtNodes/NodeDelegateModel.hpp>
//#include "BaseNodeModel.h"
//#include "BaseNodeData.h"
//#include "NodeEmbedWidget.h"
//
//
//class TestNodeModel:public BaseNodeModel
//{
//public:
//    TestNodeModel() { 
//       thisName =  "测试模块" ;
//       thisCaption = ": 触发模块";
//       inPortNumber = 3;
//       outPortNumber = 1;
//    }
//
//    TestNodeModel(const QString& name, const QString& caption_name, const uint in_port_number, const uint out_port_number)
//        : BaseNodeModel(name, caption_name, in_port_number, out_port_number){ ; }
//
//    ~TestNodeModel() { ; }
//
//
//public:
//    QString caption() const override {
//        return this->thisCaption; 
//    }
//
//    virtual bool captionVisible() const override {
//        return false;
//    }
//
//    QString name() const override { 
//        return this->thisName; 
//    }
//
//    virtual bool portCaptionVisible(PortType portType, PortIndex portIndex) const override   {
//       if( portType  == PortType::In)
//          if (portIndex == 0) {
//             return true;
//          }
//       return true;
//    }
//
//public:
//
//    virtual QString portCaption(PortType portType, PortIndex portIndex) const override  {
//       // if(portType == )
//        switch (portType) {
//        case PortType::In:
//            if (portIndex == 0)
//                return QString("测试接口1");
//            else if (portIndex == 1)
//                return QString("测试接口2");
//            else if (portIndex == 2)
//                return QString("测试接口3");
//            break;
//
//        case PortType::Out:
//            return QString("输出接口1");
//
//        default:
//            break;
//        }
//        return QString();
//    }
//
//    virtual unsigned int nPorts(PortType portType) const override {
//        unsigned int result;
//        if (portType == PortType::In)
//            result = inPortNumber;
//        else
//            result = outPortNumber;
//        return result;
//    };
//
//    virtual NodeDataType dataType(PortType portType, PortIndex portIndex) const override {
//        return BaseNodeData().type();
//    };
//
//    /// <summary>
//    /// 输入和输出端口
//    /// </summary>
//    /// <param name="port"></param>
//    /// <returns></returns>
//    //virtual std::shared_ptr<NodeData> outData(PortIndex port) override {
//    //    ptrResult = std::make_shared<BaseNodeData>();
//    //    return std::static_pointer_cast<NodeData>(ptrResult);
//    //}
//
//    //virtual void setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) override {
//    //    Q_UNUSED(portIndex);
//    //    auto in_node_data = std::dynamic_pointer_cast<BaseNodeData>(data);
//    //}
//
//    virtual std::shared_ptr<NodeData> outData(PortIndex port) override {
//        auto ptr_node = FindMapCGObjectFlowNode(mapCGObjectFlowOutNode, port);
//       // ptrResult = std::make_shared<BaseNodeData>();
//        return std::static_pointer_cast<NodeData>(ptr_node);
//    }
//
//    virtual void setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) override {
//        Q_UNUSED(portIndex);
//        auto ptr_node = std::dynamic_pointer_cast<BaseNodeData>(data);
//        auto pari_node = std::pair<PortIndex, std::shared_ptr<BaseNodeData>>(portIndex, ptr_node);
//        InsertMapCGObjectFlowNode(mapCGObjectFlowInNode, pari_node);
//    }
//
//    virtual QWidget* embeddedWidget() override {
//        if (!nodeEmbedWidget) {
//            nodeEmbedWidget = new NodeEmbedWidget();
//        }
//        return nodeEmbedWidget;
//    }
//
//    /// <summary>
//    /// 获得控件的指针
//    /// </summary>
//    /// <returns></returns>
//    virtual ToolInterface* GetToolInterface() {
//        return nullptr;
//    }
//
//private:
//
//    inline std::shared_ptr<BaseNodeData> FindMapCGObjectFlowNode(const std::unordered_map<PortIndex, std::shared_ptr<BaseNodeData>>& map_node,
//        const PortIndex &port_index) {
//        auto iter = map_node.find(port_index);
//        if (iter == map_node.end())
//            return nullptr;
//
//        auto ptr = std::static_pointer_cast<BaseNodeData>(iter->second);
//        return ptr;
//    }
//
//    inline void InsertMapCGObjectFlowNode(std::unordered_map<PortIndex, std::shared_ptr<BaseNodeData>>& map_node,
//        const std::pair<PortIndex, std::shared_ptr<BaseNodeData>> &node) {
//        PortIndex index = node.first;
//        auto iter = map_node.find(index);
//        if (iter != map_node.end()) {
//            map_node.erase(iter);
//            map_node.insert(node);
//        }
//        else{
//            map_node.insert(node);
//        }
//    }
//
//
//public:
//    NodeEmbedWidget* nodeEmbedWidget{ nullptr };
//    std::shared_ptr<BaseNodeData> ptrResult;
//
//    /// <summary>
//    /// 输出节点的数据
//    /// </summary>
//    std::unordered_map<PortIndex, std::shared_ptr<BaseNodeData>> mapCGObjectFlowOutNode;
//    /// <summary>
//    /// 输入节点数据
//    /// </summary>
//    std::unordered_map<PortIndex, std::shared_ptr<BaseNodeData>> mapCGObjectFlowInNode;
//};
