#pragma execution_character_set("utf-8")
#include "FramCameraSetParam.h"
#include <QPushButton>
#include <QInputDialog>
#include <QPluginLoader>
#include <QMessageBox>
#include <QJsonArray>
#include <QCoreApplication>
#include <QDir>


FramCameraSetParam::FramCameraSetParam(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	Initial();
}

FramCameraSetParam::~FramCameraSetParam()
{}

void FramCameraSetParam::Initial()
{
     InitialTableWidget();
	 InitialPushButton();
	 InitialCamerParam();
	 InitialOtherForm();
     InitialCameraFactory();
     InitialCombox();

	 FrmLog* ptr_framlog = FrmLog::FrmLogInstance();
	 QObject::connect(this, &FramCameraSetParam::SendLogInfoSig, ptr_framlog, &FrmLog::LogMassageShowSlot);
}

void FramCameraSetParam::InitialTableWidget()
{
	ui.tableWidget->setColumnCount(1);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行
	ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);  //设置只能单选	
    
	connect(ui.tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(TabWidgetDoubleClicked(int, int)));
    connect(ui.tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(TabWidgetDoubleClicked(int, int)));
}

void FramCameraSetParam::InitialCameraFactory()
{
    ptrCameraFactory = std::make_shared<CameraFactory>();

    QDir pluginsdir = QDir(qApp->applicationDirPath());
    pluginsdir.cd("Camera");
    QFileInfoList pluginsInfo = pluginsdir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    //加载插件
    for (QFileInfo fileinfo : pluginsInfo) {
        QString filepath = fileinfo.absoluteFilePath();
        if (!QLibrary::isLibrary(filepath))
            continue;
        QLibrary camera_lib(filepath);
        CameraRegister camra_register = (CameraRegister)camera_lib.resolve("CameraRegister");
        camra_register(ptrCameraFactory.get());
    }
}

void FramCameraSetParam::InitialPushButton()
{
	/// <summary>
	/// 添加相机
	/// </summary>
	connect(ui.btnAddCamera, &QPushButton::clicked, this, &FramCameraSetParam::AddCameraDevice);

	/// <summary>
	/// 寻找相机
	/// </summary>
	connect(ui.btnSearchCamera, &QPushButton::clicked, this, &FramCameraSetParam::SearchCameraDevice);
	
	/// <summary>
	/// 链接相机
	/// </summary>
	connect(ui.btnConnect, &QPushButton::clicked, this, &FramCameraSetParam::ConnectCamera);

	/// <summary>
	/// 断开链接相机
	/// </summary>
	connect(ui.btnDisconnect, &QPushButton::clicked, this, &FramCameraSetParam::DisConnectCamera);
}

void FramCameraSetParam::InitialCombox()
{
    std::vector<std::string> all_camera_kind = ptrCameraFactory->GetAllCameraKind();
	ui.comboCamera->clear();
	for (auto camera : all_camera_kind) {
		ui.comboCamera->addItem(QString::fromStdString(camera));
	}

	connect(ui.comboCameraName, QOverload<const QString&>::of(&QComboBox::currentTextChanged), [=](const QString& str) {
		ui.labelCameraName->setText(str); });
}

void FramCameraSetParam::InitialOtherForm()
{

}

void FramCameraSetParam::InitialCamerParam()
{

}

QJsonObject FramCameraSetParam::SerializeCameraDevice()
{
	QJsonObject cameras_json;
	QJsonArray cameras_json_array;
	for (auto iter = qMapCameraDevices.constBegin(); iter != qMapCameraDevices.constEnd(); ++iter) {
		QString key = iter.key();
		BaseCamera::Ptr ptr = iter.value();
        StdCameraParam std_cam_param = ptr->MvGetConfig();
        CameraParam cam_param = StdParam2QParam(std_cam_param);
		QJsonObject camera_json;
		camera_json["MapKey"] = key;
		camera_json["CameraName"] = QString::fromStdString(ptr->GetDeviceName());
		QByteArray data_array;
		QDataStream stream(&data_array, QIODevice::WriteOnly);
		stream << cam_param;
		QString str = QString(data_array.toHex());
		camera_json["CamerParam"] = str;
		cameras_json_array.append(camera_json);
		emit SendLogInfoSig(QString::fromStdString(ptr->GetDeviceName()) + "相机序列化完成", LOGTYPE::INFO);
	}
	emit SendLogInfoSig("相机序列化结束", LOGTYPE::INFO);
	cameras_json["CamerasSet"] = cameras_json_array;
	return cameras_json;
}

void FramCameraSetParam::DeserializeCameraDevice(const QJsonObject & cameras_json)
{
	emit SendLogInfoSig("开始反序列化相机", LOGTYPE::INFO);
	QJsonArray cameras_json_array = cameras_json["CamerasSet"].toArray();
	QMap<QString, std::pair<std::string, CameraParam>> qmap_camera_param;
	for (QJsonValue node : cameras_json_array) {
		CameraParam cam_param;
		QJsonObject camera_json = node.toObject();
		QString key = camera_json["MapKey"].toString();
		QString camera_name = camera_json["CameraName"].toString();
		QString camer_param = camera_json["CamerParam"].toString();

		QByteArray by = QByteArray::fromHex(camer_param.toLatin1());
		// 创建一个数据流对象，并设置数据容器和打开方式
		QDataStream stream(&by, QIODevice::ReadOnly);
		stream >> cam_param;
		qmap_camera_param.insert(key, std::make_pair(camera_name.toStdString(), cam_param));
	}

	for (auto iter = qmap_camera_param.constBegin(); iter != qmap_camera_param.constEnd(); ++iter) {
		QString key = iter.key();
		auto cam_name_param = iter.value();
		std::string camera_name = cam_name_param.first;
		QString camera_type = cam_name_param.second.cameraType;  //类型

		std::shared_ptr<BaseCamera> ptr_camera = ptrCameraFactory->CameraProduce(camera_type.toStdString());
		std::vector<std::string> device_list;
		int res = ptr_camera->MvInitialize(device_list);
		if (res != 0) {
			emit SendLogInfoSig(camera_type + "反序列化失败 - 相机初始化失败", LOGTYPE::ERRORS);
		}
		res = ptr_camera->MvOpenDevice(camera_name);
		if (res != 0) {
			emit SendLogInfoSig(QString::fromStdString(camera_name) + "反序列化失败 - 相机打开失败", LOGTYPE::ERRORS);
		}


        StdCameraParam std_cam_param = QParam2StdParam(cam_name_param.second);
		res = ptr_camera->MvSetConfig(std_cam_param);
		if (res != 0) {
			emit SendLogInfoSig(QString::fromStdString(camera_name) + "反序列化失败 - 相机设置参数失败", LOGTYPE::ERRORS);
		}

		int count = ui.tableWidget->rowCount();
		QTableWidgetItem* item_type = new QTableWidgetItem(key);
		ui.tableWidget->setRowCount(count + 1);  //设置行数
		item_type->setFlags(item_type->flags() & (~Qt::ItemIsEditable));  //设置列不可编辑	
		ui.tableWidget->setItem(count, 0, item_type);

        ///添加相机设备
        StaticGlobalVariable::ptrToolNodeDataInteract->MapInserExDevice(key);

		qMapCameraDevices.insert(key, ptr_camera);
	}
}

void FramCameraSetParam::TabWidgetDoubleClicked(int row, int col)
{
	QString value = ui.tableWidget->item(row, 0)->text();
	if (qMapCameraDevices.contains(value)) {
		auto ptr = qMapCameraDevices[value];
		std::string name = ptr->GetDeviceName();
		if (name.empty()) {
			return;
		}
		ui.labelCameraName->setText(QString::fromStdString(ptr->GetDeviceName()));
	}
	else {
		QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "不存在该相机,请绑定相机!");
	}

}

void FramCameraSetParam::AddCameraDevice()
{
	QString camera_kind = ui.comboCamera->currentText();
	std::shared_ptr<BaseCamera> ptr_camera = ptrCameraFactory->CameraProduce(camera_kind.toStdString());
	std::vector<std::string> device_list;
	int res = ptr_camera->MvInitialize(device_list);
	if (res != 0) {
		QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "设备相机故障，检查后，重连！");
		return;
	}

	QString camera_name = ui.comboCameraName->currentText();
	ui.labelCameraName->setText(camera_name);
	int count = ui.tableWidget->rowCount();
	bool ok; QString value;

loop: value = QInputDialog::getText(this, tr("相机名称"),
		tr("请输入自定义名称"), QLineEdit::Normal, QString("相机" + QString::number(count + 1)), &ok);
	
if (qMapCameraDevices.contains(value)) {
		QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "相机设备重命名!");
		goto loop;
	}
	if (!ok)
	return;

	QTableWidgetItem* item_type = new QTableWidgetItem(value);
	ui.tableWidget->setRowCount(count + 1);  //设置行数
	item_type->setFlags(item_type->flags() & (~Qt::ItemIsEditable));  //设置列不可编辑	
	ui.tableWidget->setItem(count, 0, item_type);
	SearchCameraDevice();
}

void FramCameraSetParam::SearchCameraDevice()
{
	std::vector<std::string> device_list;
	QString camera_kind = ui.comboCamera->currentText();
	std::shared_ptr<BaseCamera> ptr_camera = ptrCameraFactory->CameraProduce(camera_kind.toStdString());
	std::vector<std::string>().swap(device_list);
	ptr_camera->MvInitialize(device_list);
	if (device_list.empty()) {
		QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "相机掉线或该系列相机不存在，请检查后重试！");
		return;
	}

	ui.comboCameraName->clear();
	for (auto camera : device_list) {
		ui.comboCameraName->addItem(QString::fromStdString(camera));
	}
	ui.labelCameraName->setText(QString::fromStdString(*device_list.begin()));
	emit SendLogInfoSig("成功搜索到相机", LOGTYPE::INFO);
}

void FramCameraSetParam::ConnectCamera()
{
	QString camera_name = ui.labelCameraName->text();
	QString camera_kind = ui.comboCamera->currentText();
	for (auto iter = qMapCameraDevices.constBegin(); iter != qMapCameraDevices.constEnd(); ++iter) {
		BaseCamera::Ptr ptr = iter.value();
		QString device_name = QString::fromStdString(ptr->GetDeviceName());
		if (device_name == camera_name) {
			QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "相机已绑定，请勿重复绑定！");
			return;
		}
	}
	std::shared_ptr<BaseCamera> ptr_camera = ptrCameraFactory->CameraProduce(camera_kind.toStdString());
	int res = ptr_camera->MvOpenDevice(camera_name.toStdString());
	if (res != 0) {
		emit SendLogInfoSig(camera_name + ": 相机打开失败", LOGTYPE::ERRORS);
		QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "相机打开失败！");
		return;
	}

    StdCameraParam std_cam_param;
    std_cam_param.cameraType = camera_kind.toStdString();
    std_cam_param.cameraName = camera_name.toStdString();
	ptr_camera->MvSetConfig(std_cam_param);

	auto camra_item = ui.tableWidget->currentItem();
	if (camra_item == nullptr) {
		QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "请双击选择绑定相机！");
		return;
	}
	QString value = camra_item->text();
	qMapCameraDevices.insert(value, ptr_camera);
    ///添加相机设备
    StaticGlobalVariable::ptrToolNodeDataInteract->MapInserExDevice(value);
	emit SendLogInfoSig(value + ":相机连接成功", LOGTYPE::INFO);
}

void FramCameraSetParam::DisConnectCamera()
{
	QString vaule = ui.labelCameraName->text();
	auto camra_item = ui.tableWidget->currentItem();
	if (camra_item == nullptr) {
		QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "请双击选择绑定相机！");
		return;
	}
	QString map_key = camra_item->text();
	if (!qMapCameraDevices.contains(map_key)) {
		QMessageBox::StandardButton result = QMessageBox::warning(nullptr, "错误", "相机不存在，请检查后重试！");
		return;
	}
	int row_index = ui.tableWidget->currentRow();
	ui.tableWidget->removeRow(row_index);
	std::shared_ptr<BaseCamera> ptr_camera = qMapCameraDevices[map_key];
	qMapCameraDevices.remove(map_key);

    ///删除相机设备
    StaticGlobalVariable::ptrToolNodeDataInteract->MapEraseExDevice(map_key);

	emit SendLogInfoSig(map_key + ":相机已断开连接", LOGTYPE::INFO);
}