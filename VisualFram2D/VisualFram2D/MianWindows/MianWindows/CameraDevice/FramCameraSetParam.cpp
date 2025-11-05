#pragma execution_character_set("utf-8")
#include "FramCameraSetParam.h"
#include <QPushButton>
#include <QInputDialog>
#include <QPluginLoader>
#include <QMessageBox>
#include <QJsonArray>
#include <QCoreApplication>
#include <QDir>
#include "VirtualCamera.h"
#include "HCNetCamera.h"
#include "FramCameraLogin.h"
#include "../Log/frmLog.h" 
#include "../StaticGlobalVariable.h"


FramCameraSetParam::FramCameraSetParam(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	Initial();
}

FramCameraSetParam::~FramCameraSetParam()
{
    for (auto iter = ptrMapIndexCameraDevices->constBegin(); iter != ptrMapIndexCameraDevices->constEnd(); ++iter) {
        BaseCamera::Ptr ptr = iter.value();
        ptr->MvCloeDevice();
        ptr.reset();
    }
    this->deleteLater();
}

void FramCameraSetParam::Initial()
{
    ptrMapIndexCameraDevices = std::make_shared<QMap<int, BaseCamera::Ptr>>();
     InitialTabWidget();
	 InitialPushButton();
	 InitialCommunicateDeviceParam();
	 InitialOtherForm();
     InitialCameraFactory();
     InitialCombox();

	 FrmLog* ptr_framlog = FrmLog::FrmLogInstance();
	 QObject::connect(this, &FramCameraSetParam::SendLogInfoSynSig, ptr_framlog, &FrmLog::LogMassageShowSlot);
}

void FramCameraSetParam::InitialTabWidget()
{
	ui.tableWidget->setColumnCount(6);
    ui.tableWidget->setColumnWidth(0, 30);  //编号
    ui.tableWidget->setColumnWidth(1, 80);  //名称
    ui.tableWidget->setColumnWidth(2, 80);  //采样频率
    ui.tableWidget->setColumnWidth(3, 80);  //是否保存
    ui.tableWidget->setColumnWidth(4, 275);  //URL
    ui.tableWidget->setColumnWidth(5, 275); //保存路径
	ui.tableWidget->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行
	ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);  //设置只能单选	
    ui.tableWidget->setEditTriggers(QAbstractItemView::CurrentChanged);
    ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);  //不可以编辑
    ui.tableWidget->verticalHeader()->setVisible(false);

	//connect(ui.tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(TabWidgetDoubleClicked(int, int)));
    connect(ui.tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(TabWidgetClicked(int, int)));
}

void FramCameraSetParam::InitialCameraFactory()
{
 
}

void FramCameraSetParam::InitialPushButton()
{	
	/// <summary>
	/// 链接相机
	/// </summary>
	connect(ui.btnAddCamera, &QPushButton::clicked, this, &FramCameraSetParam::AddCamera);

	/// <summary>
	/// 断开链接相机
	/// </summary>
	connect(ui.btnDisconnect, &QPushButton::clicked, this, &FramCameraSetParam::DisConnectCamera);
}

void FramCameraSetParam::InitialCombox()
{
  
}

void FramCameraSetParam::InitialOtherForm()
{

}

void FramCameraSetParam::InitialCommunicateDeviceParam()
{

}

QJsonObject FramCameraSetParam::SerializeCameraDevice()
{
	QJsonObject cameras_json;
	QJsonArray cameras_json_array;

    for (auto iter = ptrMapIndexCameraDevices->constBegin(); iter != ptrMapIndexCameraDevices->constEnd(); ++iter) {
        QJsonObject json_camera;
        int key = iter.key();
        BaseCamera::Ptr ptr = iter.value();
        CameraParam camera_param = ptr->GetDeviceParam();
        json_camera["Key"] = key;
        json_camera["Name"] = camera_param.deviceName;
        json_camera["URL"] = camera_param.strURL;
        json_camera["savePath"] = camera_param.savePath;
        json_camera["isSave"] = camera_param.isSave;
        json_camera["camerIndex"] = camera_param.camerIndex;
        json_camera["timeSpan"] = camera_param.saveTime;
        json_camera["virtualCamera"] = camera_param.virtualCamera;
        json_camera["sampleFrequencyTime"] = camera_param.sampleFrequencyTime;
        cameras_json_array.append(json_camera);
    }

	cameras_json["CamerasSet"] = cameras_json_array;
	return cameras_json;
}

void FramCameraSetParam::DeserializeCameraDevice(const QJsonObject & cameras_json)
{
	emit SendLogInfoSynSig("开始反序列化相机", LOGTYPE::INFO);
    QJsonArray cameras_json_array = cameras_json["CamerasSet"].toArray();
    for (QJsonValue node : cameras_json_array) {
        CameraParam camera_param;
        int key =                 node["Key"].toInt();
        camera_param.deviceName = node["Name"].toString();
        camera_param.strURL =     node["URL"].toString();
        camera_param.savePath =   node["savePath"].toString();
        camera_param.isSave =     node["isSave"].toBool();
        camera_param.camerIndex = node["camerIndex"].toInt();
        camera_param.saveTime =   node["timeSpan"].toInt();
        camera_param.virtualCamera = node["virtualCamera"].toBool();
        camera_param.sampleFrequencyTime = node["sampleFrequencyTime"].toInt();

        QTableWidgetItem* item_id = new QTableWidgetItem(QString::number(camera_param.camerIndex));
        QTableWidgetItem* item_name = new QTableWidgetItem(camera_param.deviceName);
        QTableWidgetItem* item_url = new QTableWidgetItem(camera_param.strURL);
        QTableWidgetItem* item_save_path = new QTableWidgetItem(camera_param.savePath);
        QTableWidgetItem* item_timespan = new QTableWidgetItem(QString::number(camera_param.saveTime));
        QTableWidgetItem* item_issava = new QTableWidgetItem(QString::number(camera_param.isSave));
        QTableWidgetItem* item_sampleFrequencyTime = new QTableWidgetItem(QString::number(camera_param.sampleFrequencyTime));
        int count = ui.tableWidget->rowCount();
        ui.tableWidget->setRowCount(count + 1);  //设置行数

        //item_id->setFlags(item_id->flags() & (~Qt::ItemIsEditable));  //设置列不可编辑	
        //item_name->setFlags(item_name->flags() & (~Qt::ItemIsEditable));  
        //item_url->setFlags(item_url->flags() & (~Qt::ItemIsEditable)); 	
        ui.tableWidget->setItem(count, 0, item_id);
        ui.tableWidget->setItem(count, 1, item_name);
        ui.tableWidget->setItem(count, 2, item_sampleFrequencyTime);
        ui.tableWidget->setItem(count, 3, item_issava);
        ui.tableWidget->setItem(count, 4, item_url);
        ui.tableWidget->setItem(count, 5, item_save_path);

        BaseCamera::Ptr ptr_base_camera = nullptr;
        int res = false;
        //如果是虚拟相机，直接采用本地文件夹
        if (camera_param.virtualCamera) {
            ptr_base_camera = std::make_shared<VirtualCamera>();
            res = ptr_base_camera->MvOpenDevice(camera_param);
        }
        else {
            ptr_base_camera = std::make_shared<HCNetCamera>();
            res = ptr_base_camera->MvOpenDevice(camera_param);
        }
        if (!res) {
            emit SendLogInfoSynSig(camera_param.deviceName + ":  相机打开失败！", LOGTYPE::ERRORS);
            // 设置当前行所有列的背景色为红色
            for (int col = 0; col < ui.tableWidget->columnCount(); ++col) {
                QTableWidgetItem* fail_item = ui.tableWidget->item(count, col);
                if (fail_item) {
                    fail_item->setBackground(QBrush(Qt::red));
                }
            }
            //失败会报警，但是不能return出去，否则相机就没了
            //return;
        }
        ptrMapIndexCameraDevices->insert(key, ptr_base_camera);
        StaticGlobalVariable::ptrToolNodeDataInteract->MapInserExDevice(camera_param.deviceName);
    }
}

/// <summary>
/// 不可以双击修改，修改会出问题，只能删除，重新添加
/// </summary>
/// <param name="row"></param>
/// <param name="col"></param>
void FramCameraSetParam::TabWidgetDoubleClicked(int row, int col)
{
    QTableWidgetItem* item = ui.tableWidget->item(row, col);
    if (item == nullptr) return;    

    FramCameraLogin fram_camera_login;
    fram_camera_login.exec();
    CameraParam camera_param  = fram_camera_login.GetCameraInfo();

    if (camera_param.deviceName.isEmpty() || camera_param.strURL.isEmpty()) {
        emit SendLogInfoSynSig("相机信息不能为空", LOGTYPE::ERRORS);
        return;
    }   

    if (ptrMapIndexCameraDevices->contains(camera_param.camerIndex)) {
        QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "相机ID已经存在，请重新输入ID");
        emit SendLogInfoSynSig("相机ID已经存在，请检查后重新输出", LOGTYPE::ERRORS);
        return;
    }

    BaseCamera::Ptr ptr = std::make_shared<VirtualCamera>();
    bool res = ptr->MvOpenDevice(camera_param);
    if (!res) {
        QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "相机打开失败，请检查URL是否正确！");
        emit SendLogInfoSynSig("相机打开失败！", LOGTYPE::ERRORS);
        return;
    }

    QTableWidgetItem* item_id = ui.tableWidget->item(row, 0);
    QTableWidgetItem* item_name = ui.tableWidget->item(row, 1);
    QTableWidgetItem* item_url = ui.tableWidget->item(row, 2);
    item_name->setText(QString::number(camera_param.camerIndex));
    item_name->setText(camera_param.deviceName);
    item_url->setText(camera_param.strURL);

    ptrMapIndexCameraDevices->insert(camera_param.camerIndex, ptr);
    StaticGlobalVariable::ptrToolNodeDataInteract->MapInserExDevice(camera_param.deviceName);
}

void FramCameraSetParam::TabWidgetClicked(int row, int col)
{
    QTableWidgetItem* item = ui.tableWidget->item(row, col);
    if (item == nullptr) return;
}

void FramCameraSetParam::AddCameraDevice()
{

}

void FramCameraSetParam::SearchCameraDevice()
{

}

void FramCameraSetParam::AddCamera()
{
    FramCameraLogin fram_camera_login;
    fram_camera_login.exec();
    CameraParam camera_param = fram_camera_login.GetCameraInfo();
    if (camera_param.deviceName.isEmpty() || camera_param.strURL.isEmpty()) {
        emit SendLogInfoSynSig("相机信息不能为空", LOGTYPE::ERRORS);
        return;
    }
    if (ptrMapIndexCameraDevices->contains(camera_param.camerIndex)) {
        QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "相机ID已经存在，请重新输入ID");
        emit SendLogInfoSynSig("相机ID已经存在，请检查后重新输出", LOGTYPE::ERRORS);
        return;
    }
    BaseCamera::Ptr ptr_base_camera = nullptr;
    int res = false;

    //如果是虚拟相机，直接采用本地文件夹
    if (camera_param.virtualCamera) {
        ptr_base_camera = std::make_shared<VirtualCamera>();
        res = ptr_base_camera->MvOpenDevice(camera_param);
    }
    else  {
        ptr_base_camera = std::make_shared<HCNetCamera>();
        res = ptr_base_camera->MvOpenDevice(camera_param);
    } 


    if (!res) {
        QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "相机打开失败，请检查URL是否正确！");
        emit SendLogInfoSynSig("相机打开失败！", LOGTYPE::ERRORS);
        return;
    }

    int count = ui.tableWidget->rowCount();
    QTableWidgetItem* item_id = new QTableWidgetItem(QString::number(camera_param.camerIndex));
    QTableWidgetItem* item_name = new QTableWidgetItem(camera_param.deviceName);
    QTableWidgetItem* item_url = new QTableWidgetItem(camera_param.strURL);
    QTableWidgetItem* item_save_path = new QTableWidgetItem(camera_param.savePath);
    QTableWidgetItem* item_timespan = new QTableWidgetItem(QString::number(camera_param.saveTime));
    QTableWidgetItem* item_issava = new QTableWidgetItem(QString::number(camera_param.isSave));
    QTableWidgetItem* item_sampleFrequencyTime = new QTableWidgetItem(QString::number(camera_param.sampleFrequencyTime));

    ui.tableWidget->setRowCount(count + 1);  //设置行数
    //item_id->setFlags(item_id->flags() & (~Qt::ItemIsEditable));  //设置列不可编辑	
    //item_name->setFlags(item_name->flags() & (~Qt::ItemIsEditable));
    //item_url->setFlags(item_url->flags() & (~Qt::ItemIsEditable));
    ui.tableWidget->setItem(count, 0, item_id);
    ui.tableWidget->setItem(count, 1, item_name);
    ui.tableWidget->setItem(count, 2, item_sampleFrequencyTime);
    ui.tableWidget->setItem(count, 3, item_issava);
    ui.tableWidget->setItem(count, 4, item_url);
    ui.tableWidget->setItem(count, 5, item_save_path);

    ptrMapIndexCameraDevices->insert(camera_param.camerIndex, ptr_base_camera);
    StaticGlobalVariable::ptrToolNodeDataInteract->MapInserExDevice(camera_param.deviceName);
}

void FramCameraSetParam::DisConnectCamera()
{
    QTableWidgetItem* item = ui.tableWidget->currentItem();
    if (item == nullptr) {
        return;
    }
    int row = item->row();

    QTableWidgetItem* item_id = ui.tableWidget->item(row, 0);
    QTableWidgetItem* item_name = ui.tableWidget->item(row, 1);
    int camer_id = item_id->text().toInt();
    QString camer_name = item_name->text();

    auto iter = ptrMapIndexCameraDevices->find(camer_id);
    if (iter == ptrMapIndexCameraDevices->end()) {
        return; // 未找到对应的键，直接返回
    }
    iter.value()->MvCloeDevice();
    iter.value().reset(); // 重置 BaseCamera::Ptr
    ptrMapIndexCameraDevices->erase(iter); // 删除当前迭代器位置的元素
    StaticGlobalVariable::ptrToolNodeDataInteract->MapEraseExDevice(camer_name);
    ui.tableWidget->removeRow(row);
}