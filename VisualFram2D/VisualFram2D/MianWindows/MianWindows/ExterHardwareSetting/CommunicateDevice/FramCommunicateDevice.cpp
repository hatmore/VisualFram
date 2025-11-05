#pragma execution_character_set("utf-8")
#include "FramCommunicateDevice.h"
#include <QCoreApplication>
#include <QInputDialog>
#include <QWidgetAction>
#include <QMessageBox>
#include <QJsonArray>
#include <QPluginLoader>
#include <QDir>

FramCommunicateDevice::FramCommunicateDevice(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	Initial();
}

FramCommunicateDevice::~FramCommunicateDevice()
{

}

void FramCommunicateDevice::Initial()
{
    InitialCommDeviceFactory();
	InitialTableWidget();
	InitialPushButton();
	InitialCombox();
	InitialCamerParam();
	InitialOtherForm();

	FrmLog* ptr_framlog = FrmLog::FrmLogInstance();
	QObject::connect(this, &FramCommunicateDevice::SendLogInfoSig, ptr_framlog, &FrmLog::LogMassageShowSlot);
}

void FramCommunicateDevice::InitialCommDeviceFactory()
{
    ptrCommDeviceFactory = std::make_shared<CommDeviceFactory>();
    QDir pluginsdir = QDir(qApp->applicationDirPath());
    pluginsdir.cd("CommDevice");
    QFileInfoList pluginsInfo = pluginsdir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    //加载插件
    for (QFileInfo fileinfo : pluginsInfo) {
        QString filepath = fileinfo.absoluteFilePath();
        if (!QLibrary::isLibrary(filepath))
            continue;
        QPluginLoader loader(filepath);
        auto is_load = loader.load();
        if (loader.load()) {
            CommDevicePlugin* plugin = qobject_cast<CommDevicePlugin*>(loader.instance());
            if (plugin) {
                PtrCommDevicePlugin ptr_plugin = std::shared_ptr<CommDevicePlugin>(plugin);
                QString name = ptr_plugin->ThisName();
                QString describe = ptr_plugin->ThisDescribe();
                ptr_plugin->CommDeviceRegistry(ptrCommDeviceFactory);
                mapPtrCommDevicePlugin.insert(name, ptr_plugin);
                emit SendLogInfoSig(name + "-- 通信设备加载完毕 ", LOGTYPE::INFO);
            }
        }
    }
}

void FramCommunicateDevice::InitialTableWidget()
{
	ui.tableWidget->setColumnCount(1);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行
	ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);  //设置只能单选	
    connect(ui.tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(TabWidgetDoubleClicked(int, int)));
    connect(ui.tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(TabWidgetDoubleClicked(int, int)));
    ui.tabWidgetCommDevice->setMovable(false);
}

void FramCommunicateDevice::InitialPushButton()
{
    connect(ui.btnAdd, &QPushButton::clicked, this, &FramCommunicateDevice::AddCommDevice);
    connect(ui.pushButtonDelete, &QPushButton::clicked, this, &FramCommunicateDevice::DeleteCommDevice);
}

void FramCommunicateDevice::InitialCombox()
{
    std::vector<QString> all_commdevice_kind = ptrCommDeviceFactory->GetAllCameraKind();
	ui.comboInstrument->clear();
	for (auto device : all_commdevice_kind) {
		ui.comboInstrument->addItem(device);
	}

	connect(ui.comboInstrument, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](const int& index) {
	
	});
}

void FramCommunicateDevice::InitialCamerParam()
{

}

void FramCommunicateDevice::InitialOtherForm()
{

}

void FramCommunicateDevice::TabWidgetDoubleClicked(int row, int col)
{
    QString value = ui.tableWidget->item(row, 0)->text();
    if (mapCommDeviceInterface.contains(value)) {
        CommDeviceInterface* device_interface = mapCommDeviceInterface[value];
        ui.tabWidgetCommDevice->setCurrentIndex(ui.tabWidgetCommDevice->indexOf(device_interface));
        ui.lblType->setText(value);
    }
    else {
        QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "不存在该相机!");
    }
}

void FramCommunicateDevice::AddCommDevice()
{
	  QString device_kind = ui.comboInstrument->currentText();
	  bool ok; QString value;
	  int count = ui.tableWidget->rowCount();
    loop: value = QInputDialog::getText(this, tr("通讯设备名称"),
	    tr("请输入自定义名称"), QLineEdit::Normal, QString("通讯设备" + QString::number(count + 1)), &ok);

    if (mapCommDeviceInterface.contains(value)) {
	    QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "通讯设备重命名!");
	    goto loop;
    }
    if (!ok)
        return;

    QTableWidgetItem* item_type = new QTableWidgetItem(value);
    ui.tableWidget->setRowCount(count + 1);  //设置行数
    item_type->setFlags(item_type->flags() & (~Qt::ItemIsEditable));  //设置列不可编辑	
    ui.tableWidget->setItem(count, 0, item_type);

    QWidget *qwidget = ptrCommDeviceFactory->CommDeviceProduce(device_kind);
    mapCommDeviceInterface.insert(value, (CommDeviceInterface*)qwidget);
    ui.tabWidgetCommDevice->addTab(qwidget, value);

    emit SendLogInfoSig(device_kind + ": -- " + value + "-- 添加通信设备 ", LOGTYPE::INFO);
}

void FramCommunicateDevice::DeleteCommDevice()
{
    int index = ui.tableWidget->currentRow();
    if (index < 0) {
        QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "未选择有效通讯设备!");
        return;
    }

    auto result = QMessageBox::warning(this, tr("警告"),  tr("是否删除通讯设备"),
                                   QMessageBox::Ok | QMessageBox::No, QMessageBox::Ok);
    if (result == QMessageBox::No) {
        return;
    }

    QString value = ui.tableWidget->item(index, 0)->text();
    ui.tableWidget->removeRow(index);
    auto it = mapCommDeviceInterface.find(value);
    if (it != mapCommDeviceInterface.end()) {
        mapCommDeviceInterface.erase(it);
    }

    emit SendLogInfoSig(value + "-- 删除通信设备 ", LOGTYPE::INFO);
}

QJsonObject FramCommunicateDevice::SerializeCommunicateDevice()
{
    QJsonObject devices_json;
    QJsonArray devices_json_array;
    for (auto iter = mapCommDeviceInterface.constBegin(); iter != mapCommDeviceInterface.constEnd(); ++iter) {
        QString key = iter.key();
        CommDeviceInterface* ptr = iter.value();
        QJsonObject device_json;
        device_json["key"] = key;
        device_json["value"] =  ptr->SerializeDevice();
        device_json["Kind"] = ptr->GetDeviceName();
        devices_json_array.append(device_json);

        emit SendLogInfoSig(key + "-- 通信设备序列化完毕", LOGTYPE::INFO);
    }
    devices_json["CommunicateDevices"] = devices_json_array;
    emit SendLogInfoSig("通信设备序列化结束", LOGTYPE::INFO);
    return devices_json;
}

void FramCommunicateDevice::DeserializeCommunicateDevice(QJsonObject const& devices_json)
{
    QJsonArray devices_json_array = devices_json["CommunicateDevices"].toArray();
    for (QJsonValue node : devices_json_array) {
        QJsonObject device_json = node.toObject();
        QString key = device_json["key"].toString();
        auto device = device_json["value"].toObject();
        auto kind = device_json["Kind"].toString();

        QWidget* qwidget = ptrCommDeviceFactory->CommDeviceProduce(kind);
        mapCommDeviceInterface.insert(key, (CommDeviceInterface*)qwidget);
        ((CommDeviceInterface*)qwidget)->ExserializeDevice(device);
        ui.tabWidgetCommDevice->addTab(qwidget, key);

        int count = ui.tableWidget->rowCount();
        QTableWidgetItem* item_type = new QTableWidgetItem(key);
        ui.tableWidget->setRowCount(count + 1);  //设置行数
        item_type->setFlags(item_type->flags() & (~Qt::ItemIsEditable));  //设置列不可编辑	
        ui.tableWidget->setItem(count, 0, item_type);

        emit SendLogInfoSig(key + "-- 通信设备序反列化完毕", LOGTYPE::INFO);
    }
    emit SendLogInfoSig("通信设备序反列化结束", LOGTYPE::INFO);
}