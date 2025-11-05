#pragma once

#include <QtWidgets/QDialog>
#include "ui_FramYoloV11Detect.h"
#include "PublicFunction.h"
#include "ImageView/QGraphicsViews2d.h"
#include "SerializeStructParam.h"
#include "YoloV11DetectAlgorithm.h"
#include <QStandardItemModel>
#include "YoloV11Src/utils.h"
#include "YoloV11Src/YoloV11_Detect.h"

class FramYoloV11Detect : public ToolInterface, private PublicFunction
{
    Q_OBJECT

public:
    FramYoloV11Detect(QWidget *parent = nullptr);
    ~FramYoloV11Detect();

public:
    //Execute
    virtual int Execute() override;

    /// <summary>
    /// 运行一次
    /// </summary>
    /// <returns></returns>
    virtual int OnceExecute() override;
    /// <summary>
    /// 
    /// </summary>
    /// <param name="node_id"></param>
    virtual void SetNodeId(const int node_id) override;
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

    void UpdateToolParam();


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
    void AcceptSelfRunringStateSlot(const float& time, const RUNNINGSTATE& run_state);

    void PushButtonOpenModelClicked();
    void PushButtonSetConfirmClicked();
    void PushButtonModelParaConfirmClicked();
    void SpinBoxValueChange(int value);

public:
    /// <summary>
    /// 输出节点的数据和节点名称
    /// </summary>
    std::unordered_map<PortIndex, FlowNodeAndName> mapCGObjectFlowOutNodeAndName;
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
    //输出节点map数据
    PtrVMUnorderedMap<int, PtrVMVector<PtrVMRectangle>> ptrOutVMUnorderedMap{ nullptr };
    BaseNodeData::Ptr ptrOutNodeVMUnorderedMap;

private:
    //输入节点图像数据
    PtrVMMat ptrInVMMat{ nullptr };
    BaseNodeData::Ptr ptrInNodeVMMat;
    
private:
    //节点图像显示
    cv::Mat displayMat;
    //2D显示画图
    QGraphicsViews2d* ptrGraphicsViews2d{ nullptr };
    //算法实现
    YoloV11DetectAlgorithm::Ptr ptrYoloV11DetectAlgorithm;

private:
    //序列化参数变量保存
    int actionNum;
    int classNum;
    QList<int> modelClassId;
    QStringList modelClassName;
    QString modelFilePath;
    int kInputH;
    int kInputW;
    float kConfThresh;
    QStringList inputModelLabels;
    FunctionParam functionParam; //算法结构参数
    UiParam uiParam; //界面参数

private:
    //变量
    bool isSelfRunring{ true };
    bool inferResultShow{ false };

    //VMRectangle vmRectangle;
    YoloDetector detector2;
    QString filePath;
    QStandardItemModel* qTableViewModel;

private:
    Ui::FramYoloV11DetectClass ui;
};

