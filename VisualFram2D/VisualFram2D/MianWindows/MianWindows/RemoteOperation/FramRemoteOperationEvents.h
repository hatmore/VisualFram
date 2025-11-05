#pragma once

#include <QDialog>
#include "../Log/frmLog.h"
#include "RemoteSignalStruct.h"
#include "ui_FramRemoteOperationEvents.h"

class FramRemoteOperationEvents : public QDialog
{
	Q_OBJECT

public:
	FramRemoteOperationEvents(QWidget *parent = nullptr);
	~FramRemoteOperationEvents();

    /// <summary>
    /// 序列化
    /// </summary>
    /// <returns></returns>
    QJsonObject SerializeRemoteOperationEvents();
    /// <summary>
    /// 反序列化化
    /// </summary>
    /// <param name="json"></param>
    void DeserializeRemoteOperationEvents(QJsonObject const& devices_json);

    /// <summary>
    /// 获得外部硬件的指针
    /// </summary>
    /// <param name="dialog"></param>
    void GetExterHardwareDialog(QDialog *dialog);

    /// <summary>
    /// 获得外部TaskFlow指针
    /// </summary>
    /// <param name="qwidget"></param>
    void SetFramTaskFlowViewWidget(QWidget * qwidget);

    /// <summary>
    /// 获得所有解析数据
    /// </summary>
    /// <returns></returns>
    QVector<RemoteOpterateParam>& GetRemoteOpterateParam() {
        QVector<RemoteOpterateParam> qvector;
        for (auto iter = qMapRemoteOpterateParam.begin(); iter != qMapRemoteOpterateParam.end(); ++iter) {
            RemoteOpterateParam param = iter.value();
            qvector.append(param);
        }
        return qvector;
    }

private:
    void Initial();

    void InitialSetUI();

    void InitialTabWidget();

    void InitialPushButton();

    /// <summary>
    /// 菜单栏
    /// </summary>
    void InitialInitialBottonMenu();

    void InitialCombox();

private slots:

    /// <summary>
    /// 添加远程设备
    /// </summary>
    void AddRemoteOpterate();

    /// <summary>
    /// 重新加载通信设备
    /// </summary>
    void ReloadRemoteOpterate();

    /// <summary>
    /// 所有的相机名称
    /// </summary>
    void ReloadCameraDeviceNames();

    /// <summary>
    /// 添加信号菜单栏
    /// </summary>
    void MenuActionsSlot();

    /// <summary>
    /// 远程操作列表，鼠标按键
    /// </summary>
    /// <param name="row"></param>
    /// <param name="col"></param>
    void TableWidgetRemoteOperationClicked(int row, int col);

    /// <summary>
    /// 绑定远程操作事件
    /// </summary>
    void RemoteOperationCompleteBind();


private:
    /// <summary>
    /// 判断名称是否重复
    /// </summary>
    /// <param name="name">名称</param>
    /// <returns></returns>
    bool IsJudgeRemoteOpterateName(const QString &name);

    /// <summary>
    /// 添加Combox
    /// </summary>
    /// <param name="combox"></param>
    void CreateComboBoxDataType(QComboBox*& combox);

    /// <summary>
    /// 添加Combox
    /// </summary>
    /// <param name="combox"></param>
    void CreateComboBoxSignalProperty(QComboBox*& combox);

    /// <summary>
    /// 选择UI界面显示
    /// </summary>
    /// <param name="name"></param>
    void SelectRemoteOpterateSetParamUI(const QString &name);
    /// <summary>
    /// 选择信号事件，显示
    /// </summary>
    /// <param name="signal_param">信号参数</param>
    void SignalEventParamShow(const SignalParseParam &signal_param);

    /// <summary>
    /// 添加信号
    /// </summary>
    /// <param name="action_text"> 添加类型 </param>
    void AddSignaleEvnet(const QString & action_text);
    
    /// <summary>
    /// 删除信号 
    /// </summary>
    /// <param name="value"></param>
    void DeleteSignaleEvnet(const QString& value);
    
    /// <summary>
    /// 得到所有Map的name
    /// </summary>
    /// <param name="map_name"></param>
    void GetMapSignalPropertyName(QMap<SignalProperty, QList<QString>> &map_name);
    /// <summary>
    /// 插入全局信号数据
    /// </summary>
    /// <param name="name"></param>
    /// <param name="type"></param>
    /// <param name="property"></param>
    void NewExDeviceDataQueue(const QString &name, const SignalType type, const SignalProperty property);


    template<class T>
    inline void SerializeParament(QJsonObject& json_object, const QString& name, const T& param);
    template<class T>
    inline void ExserializeFunciton(const QString& json, T& param);

signals:
    void SendLogInfoSynSig(const QString& info, const LOGTYPE type);

private:
	Ui::FramRemoteOperationEventsClass ui;
    QList<QString> qListCommunicateDeviceName;
    int remoteOpterateIndex{0};
    QMap<int, RemoteOpterateParam> qMapRemoteOpterateParam;

    QDialog* framCameraSetParam{nullptr};
    QWidget* framTaskFlowViewWidget{nullptr};
};
