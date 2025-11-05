#pragma 
//execution_character_set("utf-8")
#include "FramCommunicateDevice.h"
#include <QCoreApplication>
#include <QInputDialog>
#include <QWidgetAction>
#include <QMessageBox>
#include <QJsonArray>
#include <QMenu>
#include "../StaticGlobalVariable.h"
#include "../Log/frmLog.h"
#include "snap7.h"

FramCommunicateDevice::FramCommunicateDevice(QWidget* parent)
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
    InitialUISet();
    InitialTabWidget();
    InitialPushButton();
    InitialInitialBottonMenu();
    InitialCommunicateDeviceParam();

    FrmLog* ptr_framlog = FrmLog::FrmLogInstance();
    QObject::connect(this, &FramCommunicateDevice::SendLogInfoSynSig, ptr_framlog, &FrmLog::LogMassageShowSlot);
    QObject::connect(this, &FramCommunicateDevice::SendLogInfoSynSig, this, &FramCommunicateDevice::OnLogReceived);
}

void FramCommunicateDevice::InitialUISet()
{
    QStringList items = { tr("S7Client"),tr("S7Server"), tr("TCP/IP") };
    ui.comboBoxCommunication->addItems(items);
    ui.comboBoxCommunication->setCurrentIndex(0);
    connect(ui.comboBoxCommunication, QOverload<int>::of(&QComboBox::currentIndexChanged),
        ui.widgetChange, &QStackedWidget::setCurrentIndex);

    ui.S7IPAddress->setIP("0.0.0.0");
    ui.spinS7Port->setValue(102);
    ui.S7ClientIPAddress->setIP("127.0.0.1");
    ui.spinS7ClientPort->setValue(102);
    ui.spinS7Rack->setValue(0);
    ui.spinS7Slot->setValue(1);

    // 写入数据格式提示
    ui.lineEditS7WriteData->setPlaceholderText("输入十六进制数据，如: 01020304AABBCC");

    // 连接数据大小变化信号，自动调整写入数据验证
    /*connect(ui.spinS7Size, QOverload<int>::of(&QSpinBox::valueChanged),
        this, [this](int value) {
             当数据大小变化时，可以在这里添加验证逻辑
            Q_UNUSED(value)
        });*/

    listModel = new QStringListModel(this);
    ui.listViewClient->setModel(listModel);
}

void FramCommunicateDevice::InitialTabWidget()
{
    this->setWindowTitle("设备通信");
    this->setWindowIcon(QIcon(":/MianWindows/Image/RemoteOperation.png"));

    ui.tableWidgetCommunicateSignal->setColumnCount(2);
    ui.tableWidgetCommunicateSignal->setHorizontalHeaderLabels({ "名称", "类型" });
    ui.tableWidgetCommunicateSignal->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.tableWidgetCommunicateSignal->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.tableWidgetCommunicateSignal->setSelectionMode(QAbstractItemView::SingleSelection);
}

void FramCommunicateDevice::InitialInitialBottonMenu()
{
    QMenu* m_menu = new QMenu(this);
    QString menuStyle(
        "QMenu::item{"
        "color: rgb(0, 0, 0);"
        "}"
        "QMenu::item:hover{"
        "background-color: rgb(176, 190, 225);"
        "}"
        "QMenu::item:selected{"
        "background-color: rgb(176, 190, 225);"
        "}"
    );
    m_menu->setStyleSheet(menuStyle);
    m_menu->setIcon(QIcon(":/MianWindows/image/muen.png"));

    QMenu* child_menu = m_menu->addMenu("添加");
    child_menu->setIcon(QIcon(":/MianWindows/image/add.png"));
    QIcon icon = QIcon(":/MianWindows/image/int.png");
    child_menu->addAction(icon, "Int", this, SLOT(MenuActionsSlot()));
    m_menu->addAction("删除", this, SLOT(MenuActionsSlot()));

    ui.tableWidgetCommunicateSignal->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui.tableWidgetCommunicateSignal, &QTableWidget::customContextMenuRequested,
        [this, m_menu](const QPoint& pos) {
            m_menu->exec(QCursor::pos());
        });

    connect(ui.pushButtonUpdateDB1, &QPushButton::clicked, this, [this]() {
        if (ui.comboBoxCommunication->currentIndex() != 0) {
            QMessageBox::information(this, "提示", "请先切换到 SimensS7 模式");
            return;
        }

        if (!ptrS7ServerComm->GetCommDiviceState()) {
            QMessageBox::information(this, "提示", "S7 服务端未打开，无法更新数据");
            return;
        }

        // 测试，弹出输入对话框
        bool ok1, ok2;
        int int_val = QInputDialog::getInt(
            this, "输入 INT 值", "DB1.INT0 (范围: -32768 ~ 32767):", 0, -32768, 32767, 1, &ok1);

        double real_input = QInputDialog::getDouble(
            this, "输入 REAL 值", "DB1.REAL2:", 0.0, -1e30, 1e30, 2, &ok2);

        if (!ok1 || !ok2) {
            return; // 用户取消
        }

        float real_val = static_cast<float>(real_input);
        int16_t int_value = static_cast<int16_t>(int_val);

        //安全写入DB1
        {
            std::lock_guard<std::mutex> lock(ptrS7ServerComm->GetMutex());
            auto server = ptrS7ServerComm->GetS7Server();
            if (server) {
                server->LockArea(srvAreaDB, 1);
                byte* db1 = ptrS7ServerComm->GetDB1Buffer();
                if (db1) {
                    memcpy(db1 + 0, &int_value, 2);   // DB1.INT0
                    memcpy(db1 + 2, &real_val, 4);     // DB1.REAL2
                }
                server->UnlockArea(srvAreaDB, 1);
            }
        }

        //记录日志
        emit SendLogInfoSynSig(
            QString("手动更新 DB1: INT=%1, REAL=%2")
            .arg(int_value).arg(real_val, 0, 'f', 2),
            LOGTYPE::INFO);

        QMessageBox::information(this, "成功", "DB1 数据已更新，客户端可读取！");
        });

    // 客户端功能按钮
    connect(ui.pushButtonClientRead, &QPushButton::clicked, this, [this]() {
        if (ui.comboBoxCommunication->currentIndex() != 0) {
            QMessageBox::information(this, "提示", "请先切换到 S7Client 模式");
            return;
        }

        if (!ptrS7ClientComm->GetCommDiviceState()) {
            QMessageBox::information(this, "提示", "S7 客户端未连接");
            return;
        }

        try {
            int dbNumber = GetS7DBNumber();
            int start = GetS7Start();
            int size = GetS7Size();

            // 验证参数
            if (size <= 0) {
                QMessageBox::warning(this, "参数错误", "数据大小必须大于0");
                return;
            }

            if (start < 0) {
                QMessageBox::warning(this, "参数错误", "起始地址不能为负数");
                return;
            }

            QByteArray data;
            int result = ptrS7ClientComm->ReadDB(dbNumber, start, size, data);

            if (result == 0) {
                QString formattedHex = FormatHexData(data);

                // 显示详细结果
                QString resultStr = QString(
                    "DB读取成功!\n\n"
                    "DB号: %1\n"
                    "起始地址: %2\n"
                    "读取大小: %3字节\n"
                    "实际数据: %4\n\n"
                    "十六进制: %5")
                    .arg(dbNumber)
                    .arg(start)
                    .arg(data.size())
                    .arg(QString(data))  // 尝试显示为字符串
                    .arg(formattedHex);

                QMessageBox::information(this, "DB读取结果", resultStr);

                // 记录日志
                emit SendLogInfoSynSig(
                    QString("S7 DB读取成功 - DB%1 [%2:%3] %4字节")
                    .arg(dbNumber).arg(start).arg(start + size - 1).arg(data.size()),
                    LOGTYPE::INFO);

                // 在列表中显示
                QString logEntry = QString("读取 DB%1 [%2+%3]: %4")
                    .arg(dbNumber).arg(start).arg(size).arg(formattedHex.left(100)); // 限制显示长度
                AddToLogList(logEntry);

            }
            else {
                QString errorMsg = QString("DB读取失败 (错误码: %1)").arg(result);
                QMessageBox::warning(this, "读取失败", errorMsg);
                emit SendLogInfoSynSig(errorMsg, LOGTYPE::ERRORS);
            }

        }
        catch (const std::exception& e) {
            QString errorMsg = QString("DB读取异常: %1").arg(e.what());
            QMessageBox::critical(this, "错误", errorMsg);
            emit SendLogInfoSynSig(errorMsg, LOGTYPE::ERRORS);
        }
        });

    connect(ui.pushButtonClientWrite, &QPushButton::clicked, this, [this]() {
        if (ui.comboBoxCommunication->currentIndex() != 0) {
            QMessageBox::information(this, "提示", "请先切换到 S7Client 模式");
            return;
        }

        if (!ptrS7ClientComm->GetCommDiviceState()) {
            QMessageBox::information(this, "提示", "S7 客户端未连接");
            return;
        }

        try {
            int dbNumber = GetS7DBNumber();
            int start = GetS7Start();
            QByteArray writeData = GetS7WriteData();

            // 验证写入数据
            QString originalHex = ui.lineEditS7WriteData->text().trimmed();
            if (!ValidateWriteData(originalHex)) {
                return;
            }

            if (writeData.isEmpty()) {
                QMessageBox::warning(this, "输入错误", "请输入有效的十六进制数据");
                return;
            }

            // 确认对话框
            QMessageBox::StandardButton reply = QMessageBox::question(
                this,
                "确认写入",
                QString("确定要写入数据到 DB%1 [%2] 吗?\n\n数据: %3")
                .arg(dbNumber)
                .arg(start)
                .arg(FormatHexData(writeData)),
                QMessageBox::Yes | QMessageBox::No
            );

            if (reply != QMessageBox::Yes) {
                return;
            }

            int result = ptrS7ClientComm->WriteDB(dbNumber, start, writeData);

            if (result == 0) {
                QString successMsg = QString(
                    "DB写入成功!\n\n"
                    "DB号: %1\n"
                    "起始地址: %2\n"
                    "写入大小: %3字节\n"
                    "写入数据: %4")
                    .arg(dbNumber)
                    .arg(start)
                    .arg(writeData.size())
                    .arg(FormatHexData(writeData));

                QMessageBox::information(this, "DB写入结果", successMsg);

                // 记录日志
                emit SendLogInfoSynSig(
                    QString("S7 DB写入成功 - DB%1 [%2] %3字节")
                    .arg(dbNumber).arg(start).arg(writeData.size()),
                    LOGTYPE::INFO);

                // 在列表中显示
                QString logEntry = QString("写入 DB%1 [%2]: %3")
                    .arg(dbNumber).arg(start).arg(FormatHexData(writeData));
                AddToLogList(logEntry);

            }
            else {
                QString errorMsg = QString("DB写入失败 (错误码: %1)").arg(result);
                QMessageBox::warning(this, "写入失败", errorMsg);
                emit SendLogInfoSynSig(errorMsg, LOGTYPE::ERRORS);
            }

        }
        catch (const std::exception& e) {
            QString errorMsg = QString("DB写入异常: %1").arg(e.what());
            QMessageBox::critical(this, "错误", errorMsg);
            emit SendLogInfoSynSig(errorMsg, LOGTYPE::ERRORS);
        }
        });
}

void FramCommunicateDevice::AddToLogList(const QString& message)
{
    QString logEntry = QString("[%1]").arg(message);

    QStringList list = listModel->stringList();
    list.append(logEntry);
    listModel->setStringList(list);
    ui.listView->scrollToBottom();
}

void FramCommunicateDevice::InitialPushButton()
{
    connect(ui.pushButtonOpen, &QPushButton::clicked, this, &FramCommunicateDevice::OpenCommDevice);
    connect(ui.pushButtonClose, &QPushButton::clicked, this, &FramCommunicateDevice::CloseCommDevice);
}

void FramCommunicateDevice::InitialCommunicateDeviceParam()
{
    ptrCommDeviceInterface = std::make_shared<SocketTcpServerCommunicate>();
    ptrCGDevice = std::make_shared<SocketTcpServer>();
    ptrS7ServerComm = std::make_shared<SimensS7ServerCommunication>();
    ptrS7ClientComm = std::make_shared<SimensS7ClientCommunication>();

    // 连接服务器客户端数量变化信号
    /*connect(ptrS7ServerComm.get(), &SimensS7ServerCommunication::ClientCountChanged,
        this, &FramCommunicateDevice::OnClientCountChanged);*/
}

int FramCommunicateDevice::GetS7DBNumber() const
{
    return ui.spinS7DBNumber->value();
}

int FramCommunicateDevice::GetS7Start() const
{
    return ui.spinS7Start->value();
}

int FramCommunicateDevice::GetS7Size() const
{
    return ui.spinS7Size->value();
}

QByteArray FramCommunicateDevice::GetS7WriteData() const
{
    QString hexString = ui.lineEditS7WriteData->text().trimmed();
    if (hexString.isEmpty()) {
        return QByteArray();
    }

    // 移除可能的分隔符
    hexString.remove(' ');
    hexString.remove('-');
    hexString.remove(':');

    // 检查是否为有效的十六进制字符串
    QRegularExpression hexRegExp("^[0-9A-Fa-f]*$");
    if (!hexRegExp.match(hexString).hasMatch()) {
        return QByteArray();
    }

    // 确保长度为偶数
    if (hexString.length() % 2 != 0) {
        hexString.prepend('0'); // 在前面补0
    }

    // 转换为字节数组
    QByteArray data = QByteArray::fromHex(hexString.toLatin1());
    return data;
}

QString FramCommunicateDevice::FormatHexData(const QByteArray& data) const
{
    if (data.isEmpty()) {
        return QString();
    }

    QString hexData = data.toHex().toUpper();
    QString formattedHex;

    // 每2个字符加一个空格
    for (int i = 0; i < hexData.length(); i += 2) {
        if (!formattedHex.isEmpty()) {
            formattedHex += " ";
        }
        formattedHex += hexData.mid(i, 2);
    }

    return formattedHex;
}

bool FramCommunicateDevice::ValidateWriteData(const QString& hexData) const
{
    if (hexData.isEmpty()) {
        //QMessageBox::warning(this, "输入错误", "请输入要写入的数据");
        return false;
    }

    // 检查是否为有效的十六进制字符串
    QRegularExpression hexRegExp("^[0-9A-Fa-f\\s:-]*$");
    if (!hexRegExp.match(hexData).hasMatch()) {
        //QMessageBox::warning(this, "输入错误", "请输入有效的十六进制数据");
        return false;
    }

    // 计算实际数据长度
    QString cleanHex = hexData;
    cleanHex.remove(' ').remove('-').remove(':');

    if (cleanHex.length() % 2 != 0) {
        //QMessageBox::warning(this, "输入错误", "十六进制数据长度必须为偶数");
        return false;
    }

    int dataSize = cleanHex.length() / 2;
    int expectedSize = ui.spinS7Size->value();

    if (dataSize > expectedSize) {
        /*QMessageBox::warning(this, "输入错误",
            QString("输入数据大小(%1字节)超过指定大小(%2字节)")
            .arg(dataSize).arg(expectedSize));*/
        return false;
    }

    return true;
}

void FramCommunicateDevice::OnClientCountChanged(int count)
{
    ui.labelClientCountValue->setText(QString::number(count));
}

void FramCommunicateDevice::AddSignaleEvnet(const QString& item_name)
{
    int count = ui.tableWidgetCommunicateSignal->rowCount();
    ui.tableWidgetCommunicateSignal->setRowCount(count + 1);
    QTableWidgetItem* name_item = new QTableWidgetItem(item_name);
    name_item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    name_item->setFlags(name_item->flags() & (~Qt::ItemIsEditable));
    QTableWidgetItem* type_item = new QTableWidgetItem("Int");
    type_item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    type_item->setFlags(type_item->flags() & (~Qt::ItemIsEditable));
    ui.tableWidgetCommunicateSignal->setItem(count, 0, name_item);
    ui.tableWidgetCommunicateSignal->setItem(count, 1, type_item);

    qvStrCommunicateName.append(item_name);
    std::shared_ptr<tbb::concurrent_queue<int>> ptr_queue_device_communication_data =
        std::make_shared<tbb::concurrent_queue<int>>();
    StaticGlobalVariable::ptrToolNodeDataInteract->ptrMapQueueDeviceCommunicationData->insert(
        std::make_pair(item_name, ptr_queue_device_communication_data));
}

void FramCommunicateDevice::MenuActionsSlot()
{
    QAction* action = qobject_cast<QAction*>(sender());
    QString action_text = action->text();
    int row_index = ui.tableWidgetCommunicateSignal->currentRow();

    if (action_text == "删除") {
        if (row_index != -1) {
            auto item = ui.tableWidgetCommunicateSignal->item(row_index, 0);
            QString value = item->text();
            ui.tableWidgetCommunicateSignal->removeRow(row_index);
            qvStrCommunicateName.removeAll(value);
            auto iter = StaticGlobalVariable::ptrToolNodeDataInteract->ptrMapQueueDeviceCommunicationData->find(value);
            if (iter != StaticGlobalVariable::ptrToolNodeDataInteract->ptrMapQueueDeviceCommunicationData->end()) {
                StaticGlobalVariable::ptrToolNodeDataInteract->ptrMapQueueDeviceCommunicationData->erase(iter);
            }
        }
    }
    else {
        bool ok = false;
        QString item_name = QInputDialog::getText(this, tr("触发名称"),
            tr("输入名称："), QLineEdit::Normal, "trige", &ok);
        if (!ok || item_name.isEmpty()) {
            return;
        }

        AddSignaleEvnet(item_name);
    }
}

void FramCommunicateDevice::OpenCommDevice()
{
    if (ui.comboBoxCommunication->currentIndex() == 0) // S7Client
    {
        auto s7Device = std::make_shared<SimensS7Client>();
        QString ip_adress = ui.S7ClientIPAddress->getIP();
        int port = ui.spinS7ClientPort->value();
        int rack = ui.spinS7Rack->value();
        int slot = ui.spinS7Slot->value();

        s7Device->deviceName = "S7Client";
        s7Device->ipValue = ip_adress;
        s7Device->portValue = port;
        s7Device->rackValue = rack;
        s7Device->slotValue = slot;
        s7Device->deviceId = 2;

        if (ptrS7ClientComm->GetCommDiviceState()) {
            emit SendLogInfoSynSig("S7 客户端已经连接，请勿重复连接!", LOGTYPE::ERRORS);
            return;
        }

        if (ptrS7ClientComm->InitalDevice(s7Device) != 0) {
            emit SendLogInfoSynSig("S7 客户端初始化失败，请检查后重新连接", LOGTYPE::ERRORS);
            return;
        }

        if (ptrS7ClientComm->OpenDevice() != 0) {
            emit SendLogInfoSynSig("S7 客户端连接失败，请检查后重新连接", LOGTYPE::ERRORS);
            return;
        }

        emit SendLogInfoSynSig(QString("S7 客户端连接成功，远程地址: %1, 机架: %2, 插槽: %3")
            .arg(s7Device->ipValue).arg(rack).arg(slot), LOGTYPE::INFO);
    }
    else if (ui.comboBoxCommunication->currentIndex() == 1) // S7Server
    {
        auto s7Device = std::make_shared<SimensS7Server>();
        QString ip_adress = ui.S7IPAddress->getIP();
        int port = ui.spinS7Port->value();
        s7Device->deviceName = "S7Server";
        s7Device->ipValue = ip_adress;
        s7Device->portValue = port;
        s7Device->deviceId = 1;

        if (ptrS7ServerComm->GetCommDiviceState()) {
            emit SendLogInfoSynSig("S7 服务端已经打开，请勿重复打开!", LOGTYPE::ERRORS);
            return;
        }

        if (ptrS7ServerComm->InitalDevice(s7Device) != 0) {
            emit SendLogInfoSynSig("S7 服务端初始化失败，请检查后重新打开", LOGTYPE::ERRORS);
            return;
        }

        if (ptrS7ServerComm->OpenDevice() != 0) {
            emit SendLogInfoSynSig("S7 服务端打开失败，请检查后重新打开", LOGTYPE::ERRORS);
            return;
        }

        emit SendLogInfoSynSig(QString("S7 服务器启动成功，监听地址: %1, 端口: %2")
            .arg(s7Device->ipValue).arg(s7Device->portValue), LOGTYPE::INFO);
    }
    else // TCP/IP
    {
        if (ptrCommDeviceInterface->GetCommDiviceState()) {
            emit SendLogInfoSynSig("TCP 服务端已经打开，请勿重复打开!", LOGTYPE::ERRORS);
            return;
        }

        SocketTcpServer::Ptr ptr_tcp_server = std::static_pointer_cast<SocketTcpServer>(ptrCGDevice);
        QString ip_adress = ui.widgetSocketIP->getIP();
        int port = ui.spinSocketPort->value();
        ptr_tcp_server->deviceName = "TcpServer";
        ptr_tcp_server->ipValue = ip_adress;
        ptr_tcp_server->portValue = port;
        int res = ptrCommDeviceInterface->InitalDevice(ptr_tcp_server);
        if (res != 0) {
            emit SendLogInfoSynSig("TCP 服务端初始化失败，请检查后重新打开", LOGTYPE::ERRORS);
            return;
        }
        res = ptrCommDeviceInterface->OpenDevice();
        if (res != 0) {
            emit SendLogInfoSynSig("TCP 服务端打开失败，请检查后重新打开", LOGTYPE::ERRORS);
            return;
        }

        emit SendLogInfoSynSig(QString("TCP 服务器启动成功，监听地址: %1, 端口: %2")
            .arg(ip_adress).arg(port), LOGTYPE::INFO);
    }
}

void FramCommunicateDevice::CloseCommDevice()
{
    if (ui.comboBoxCommunication->currentIndex() == 0) {
        ptrS7ClientComm->CloseDevice();
    }
    else if (ui.comboBoxCommunication->currentIndex() == 1) {
        ptrS7ServerComm->CloseDevice();
    }
    else {
        ptrCommDeviceInterface->CloseDevice();
    }
}

void FramCommunicateDevice::OnReadData()
{
    if (ui.comboBoxCommunication->currentIndex() == 0) // S7Client
    {
        if (!ptrS7ClientComm->GetCommDiviceState()) {
            return;
        }

        QByteArray readData;
        if (ptrS7ClientComm->ReadDB(1, 0, 100, readData) == 0 && readData.size() > 0) {
            QString dataStr = QString("读取远程DB1: %1").arg(readData.toHex().constData());
            QStringList list = listModel->stringList();
            list.append(dataStr);
            listModel->setStringList(list);
            ui.listViewClient->scrollToBottom();
        }
    }
    else if (ui.comboBoxCommunication->currentIndex() == 1) // S7Server
    {
        if (!ptrS7ServerComm->GetCommDiviceState()) {
            emit SendLogInfoSynSig("S7 服务端未打开，无法读取数据", LOGTYPE::ERRORS);
            return;
        }

        QByteArray readData;
        if (ptrS7ServerComm->Read(readData) == 0 && readData.size() > 0) {
            int16_t int_value = *reinterpret_cast<const int16_t*>(readData.constData());
            float real_value;
            memcpy(&real_value, readData.constData() + 2, sizeof(float));

            QString dataStr = QString("读取数据: INT=%1, REAL=%2 (Hex: %3)")
                .arg(int_value).arg(real_value).arg(readData.toHex().constData());
            QStringList list = listModel->stringList();
            list.append(dataStr);
            listModel->setStringList(list);
            ui.listView->scrollToBottom();
        }
    }
    else // TCP/IP
    {
        if (!ptrCommDeviceInterface->GetCommDiviceState()) {
            emit SendLogInfoSynSig("TCP 服务端未打开，无法读取数据", LOGTYPE::ERRORS);
            return;
        }

        QByteArray readData;
        if (ptrCommDeviceInterface->Read(readData) == 0) {
            QString dataStr = QString("读取数据: %1").arg(readData.toHex().constData());
            QStringList list = listModel->stringList();
            list.append(dataStr);
            listModel->setStringList(list);
            ui.listView->scrollToBottom();
        }
    }
}

void FramCommunicateDevice::OnLogReceived(const QString& info, LOGTYPE type)
{
    QString logStr = QString("[%1] %2").arg(type == LOGTYPE::INFO ? "INFO" : "ERROR").arg(info);
    QStringList list = listModel->stringList();
    list.append(logStr);
    listModel->setStringList(list);
    ui.listView->scrollToBottom();
}

QJsonObject FramCommunicateDevice::SerializeCommunicateDevice()
{
    QJsonObject devices_json;
    QJsonObject device_json;

    //if (ui.comboBoxCommunication->currentIndex() == 0) // S7Client
    //{
    //    SimensS7Client::Ptr ptr_s7_client = std::static_pointer_cast<SimensS7Client>(ptrS7ClientComm->GetCGDevicePtr());
    //    device_json["Type"] = "S7Client";
    //    device_json["IP"] = ptr_s7_client->ipValue;
    //    device_json["Port"] = ptr_s7_client->portValue;
    //    device_json["Rack"] = ptr_s7_client->rackValue;
    //    device_json["Slot"] = ptr_s7_client->slotValue;
    //    device_json["Name"] = ptr_s7_client->deviceName;
    //    device_json["Mode"] = "Client";
    //}
    //else if (ui.comboBoxCommunication->currentIndex() == 1) // S7Server
    //{
    //    SimensS7Server::Ptr ptr_s7_server = std::static_pointer_cast<SimensS7Server>(ptrS7ServerComm->GetCGDevicePtr());
    //    device_json["Type"] = "S7Server";
    //    device_json["IP"] = ptr_s7_server->ipValue;
    //    device_json["Port"] = ptr_s7_server->portValue;
    //    device_json["Name"] = ptr_s7_server->deviceName;
    //}
    //else // TCP/IP
    //{
    //    SocketTcpServer::Ptr ptr_tcp_server = std::static_pointer_cast<SocketTcpServer>(ptrCGDevice);
    //    device_json["Type"] = "TCP/IP";
    //    device_json["IP"] = ptr_tcp_server->ipValue;
    //    device_json["Port"] = ptr_tcp_server->portValue;
    //    device_json["Name"] = ptr_tcp_server->deviceName;
    //}

    //QJsonArray remote_opterate_array;
    //for (const QString& name : qvStrCommunicateName) {
    //    QJsonObject remote_opterate_json;
    //    remote_opterate_json["Name"] = name;
    //    remote_opterate_json["Type"] = "Int";
    //    remote_opterate_array.append(remote_opterate_json);
    //}
    //device_json["OpterateArray"] = remote_opterate_array;
    //devices_json["CommunicateDevices"] = device_json;
    //emit SendLogInfoSynSig("通信设备序列化结束", LOGTYPE::INFO);
    return devices_json;
}

void FramCommunicateDevice::DeserializeCommunicateDevice(QJsonObject const& devices_json)
{
    //QJsonObject device_json = devices_json["CommunicateDevices"].toObject();
    //QString type = device_json["Type"].toString();

    //if (type == "S7Client") {
    //    //ui.comboBoxCommunication->setCurrentIndex(0);
    //    SimensS7Client::Ptr ptr_s7_client = std::make_shared<SimensS7Client>();
    //    ptr_s7_client->ipValue = device_json["IP"].toString();
    //    ptr_s7_client->portValue = device_json["Port"].toInt();
    //    ptr_s7_client->rackValue = device_json["Rack"].toInt();
    //    ptr_s7_client->slotValue = device_json["Slot"].toInt();
    //    ptr_s7_client->deviceName = device_json["Name"].toString();

    //    ui.S7ClientIPAddress->setIP(ptr_s7_client->ipValue);
    //    ui.spinS7ClientPort->setValue(ptr_s7_client->portValue);
    //    ui.spinS7Rack->setValue(ptr_s7_client->rackValue);
    //    ui.spinS7Slot->setValue(ptr_s7_client->slotValue);

    //    ptrS7ClientComm->InitalDevice(ptr_s7_client);
    //    ptrS7ClientComm->OpenDevice();
    //}
    //if (type == "S7Server") {
    //    //ui.comboBoxCommunication->setCurrentIndex(1);
    //    SimensS7Server::Ptr ptr_s7_server = std::make_shared<SimensS7Server>();
    //    ptr_s7_server->ipValue = device_json["IP"].toString(); // 固定 IP
    //    ptr_s7_server->portValue = device_json["Port"].toInt(); // 固定端口
    //    ptr_s7_server->deviceName = device_json["Name"].toString();
    //    ptrS7ServerComm->InitalDevice(ptr_s7_server);
    //    // 不更新 UI，保持默认值
    //    ptrS7ServerComm->OpenDevice();
    //}
    //else {
    //    //ui.comboBoxCommunication->setCurrentIndex(2);
    //    SocketTcpServer::Ptr ptr_tcp_server = std::static_pointer_cast<SocketTcpServer>(ptrCGDevice);
    //    ptr_tcp_server->ipValue = device_json["IP"].toString();
    //    ptr_tcp_server->portValue = device_json["Port"].toInt();
    //    ptr_tcp_server->deviceName = device_json["Name"].toString();
    //    ui.spinSocketPort->setValue(ptr_tcp_server->portValue);
    //    ui.widgetSocketIP->setIP(ptr_tcp_server->ipValue);
    //    ptrCommDeviceInterface->InitalDevice(ptr_tcp_server);
    //    ptrCommDeviceInterface->OpenDevice();
    //}

    //QJsonArray remote_opterate_array = device_json["OpterateArray"].toArray();
    //qvStrCommunicateName.clear();
    //ui.tableWidgetCommunicateSignal->setRowCount(0);
    //for (QJsonValue remote_opterate : remote_opterate_array) {
    //    QString name = remote_opterate["Name"].toString();
    //    AddSignaleEvnet(name);
    //}

    emit SendLogInfoSynSig("通信设备反序列化结束", LOGTYPE::INFO);
}