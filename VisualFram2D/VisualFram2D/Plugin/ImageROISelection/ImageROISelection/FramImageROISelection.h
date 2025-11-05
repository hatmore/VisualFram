#pragma once
#include <QtWidgets/QDialog>
#include "PublicFunction.h"
#include "ImageView/QGraphicsViews2d.h"
#include "SerializeStructParam.h"
#include"ImageROISelectAlgorithm.h"
#include"ui_FramImageROISelection.h"
#include <QtWidgets/qgraphicsitem.h>
#include <QStandardItem>


class FramImageROISelection : public ToolInterface, private PublicFunction
{
    Q_OBJECT

public:
    FramImageROISelection(QWidget* parent = nullptr);
    ~FramImageROISelection();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override; // 添加 override 关键字
public:
    //Execute
    virtual int Execute() override;
    virtual void SetNodeId(const int node_id);
    /// <summary>
    /// 运行一次
    /// </summary>
    /// <returns></returns>
    virtual int OnceExecute() override;
    /// <summary>
    /// 序列化文件
    /// </summary>
    /// <param name="qdom_doc">根文件</param>
    /// <param name="root_element">上一级文件，目录</param>
    /// <returns></returns>
    virtual QJsonObject SerializeFunciton() override;
    /// <summary>
    /// 反序列化文件
    /// </summary>
    /// <returns></returns>
    virtual void ExserializeFunciton(const QJsonObject& json) override;

private:
    void Initial();

    void InitialNodeData();

    void InitilaToolParamUI();

    void InitialPushButton();

    void InitialImageShow();

    void InitialExserializeUi();

    int RunningTool();

    void updateTableDisplay();

public slots:
    /// <summary>
    /// 接收绑定完成槽函数，用于显示，那个数据绑定上了
    /// </summary>
    void AcceptNodeBindingSucceedSlot(const  QtNodes::PortIndex port_index);

private slots:
    /// <summary>
    /// 接收自己发送的信息，显示一部分信息
    /// </summary>
    /// <param name="time"></param>
    /// <param name="run_state"></param>
    void AcceptSelfRunringStateSlot(const float& time, const RUNNINGSTATE& run_state);//运行状态显示

    void PushButtonModelParaConfirmClicked();

public:
    /// <summary>
    /// 输出节点的数据和节点名称
    /// </summary>
    std::unordered_map<PortIndex, FlowNodeAndName> mapCGObjectFlowOutNodeAndName; //存入输出映射表
    /// <summary>
    /// 输入节点的数据和节点名称
    /// </summary>
    std::unordered_map<PortIndex, FlowNodeAndName> mapCGObjectFlowInNodeAndName;

signals:
    /// <summary>
    /// 循环运行时，线程调用，信号
    /// </summary>
    void SendNodeRuningStateAsychSig(const float& time, const RUNNINGSTATE& run_state);
    /// <summary>
    /// 在本线程运行时，信号发送
    /// </summary>
    /// <param name="time">时间</param>
    /// <param name="run_state">运行状态</param>
    void SendNodeRuningStateSyncSig(const float& time, const RUNNINGSTATE& run_state);
    /// <summary>
    /// 序列化完成信号
    /// </summary>
    void SendExserializeFinishSig();
    

private:

    //输出
    PtrVMUnorderedMap<int, PtrVMVector<VMRectangle>> ptrOutVec = { nullptr };
    BaseNodeData::Ptr ptrOutNodeData;
    
    //输入
    PtrVMMat ptrInVMMat{ nullptr };
    BaseNodeData::Ptr ptrInNodeVMMat;

    QGraphicsViews2d* ptrGraphicsViews2d{ nullptr };//视图显示（包括鼠标操作事件）

    void onPositionIdChanged(QStandardItem* item);
    void onTableItemChanged(QStandardItem* item);
    /// <summary>
    /// 方法
    /// </summary> 
  
    QList<BaseItem*> m_rectangleItems;  // 存储所有矩形
    std::vector<int>classIds;

    //动作类别对应中文标签
    QStringList inputModelLabels;

    //int LabelParam = 0;                  // 类别个数
    FunctionParam functionParam; //算法结构参数
    UiParam uiParam; //界面参数
    bool isSelfRunring{ true };
    ImageROISelectAlgorithm::Ptr ptrImageROISelectAlgorithm;

 private slots:
    void onRectItemDeleteRequested(int index); // 添加删除槽函数
 private:
    void clearAllSelections(); // 清除所有选中状态
    int m_maxRectIndex = 0; // 记录最大使用过的索引

    bool useInputModelLabels{ false };

private:
    void saveRectItemsState(); // 保存矩形状态
    void restoreRectItemsState(); // 恢复矩形状态
    QList<QRectF> m_savedRects; // 保存的矩形位置和大小
    QList<int> m_savedClassIds; // 保存的类别ID
    QList<int> m_savedIndices; // 保存的索引

private:
    Ui::ImageROISelectionClass ui;
};

