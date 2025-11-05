#include "SimensS7ServerCommunication.h"
#include <QMessageBox>
#include <QDebug>
#include "../Log/frmLog.h"

SimensS7ServerCommunication::SimensS7ServerCommunication()
{
    db1_ = new byte[1024];     // DB1: 1KB
    inputs_ = new byte[128];   // 输入区: 128字节
    outputs_ = new byte[128];  // 输出区: 128字节
    merkers_ = new byte[256];  // 位存储区: 256字节

    // 初始化内存区域
    std::fill(db1_, db1_ + 1024, 0);
    std::fill(inputs_, inputs_ + 128, 0);
    std::fill(outputs_, outputs_ + 128, 0);
    std::fill(merkers_, merkers_ + 256, 0);

    FrmLog* ptr_framlog = FrmLog::FrmLogInstance();
    QObject::connect(this, &SimensS7ServerCommunication::SendLogInfoSynSig, ptr_framlog, &FrmLog::LogMassageShowSlot);
    QObject::connect(this, &SimensS7ServerCommunication::SendLogInfoAsynSig, ptr_framlog, &FrmLog::LogMassageShowSlot, Qt::BlockingQueuedConnection);
}

SimensS7ServerCommunication::~SimensS7ServerCommunication()
{
    delete[] db1_;
    delete[] inputs_;
    delete[] outputs_;
    delete[] merkers_;
    CloseDevice();
}

int SimensS7ServerCommunication::InitalDevice(const CGDevice::Ptr ptr_device)
{
    if (!ptr_device || ptr_device->deviceMode != DeviceMode::S7Server) {
        emit SendLogInfoSynSig("初始化失败：设备配置为空或设备模式不匹配", LOGTYPE::ERRORS);
        return -1;
    }

    ptrS7Server = std::static_pointer_cast<SimensS7Server>(ptr_device);
    deviceName = ptrS7Server->deviceName;
    ipValue = ptrS7Server->ipValue;
    portValue = ptrS7Server->portValue;

    if (ptrS7Server->serverValue == nullptr) {
        ptrS7Server->serverValue = new TS7Server();
    }

    ptrS7Server->serverValue->SetEventsCallback(ServerCallback, this);
    ptrS7Server->serverValue->SetRWAreaCallback(RWAreaCallback, this);
    ptrS7Server->serverValue->SetEventsMask(evcClientAdded | evcClientDisconnected | evcDataRead | evcDataWrite);

    int result = ptrS7Server->serverValue->RegisterArea(srvAreaDB, 1, db1_, sizeof(db1_));
    result |= ptrS7Server->serverValue->RegisterArea(srvAreaPE, 0, inputs_, sizeof(inputs_));
    result |= ptrS7Server->serverValue->RegisterArea(srvAreaPA, 0, outputs_, sizeof(outputs_));
    result |= ptrS7Server->serverValue->RegisterArea(srvAreaMK, 0, merkers_, sizeof(merkers_));
    if (result != 0) {
        emit SendLogInfoSynSig(QString("%1-存储区注册失败，错误码: %2").arg(deviceName).arg(result),
            LOGTYPE::ERRORS);
        return result;
    }

    emit SendLogInfoSynSig(QString("%1-设备初始化成功，DB1、Inputs、Outputs 注册完成").arg(deviceName),
        LOGTYPE::INFO);
    return 0;
}

int SimensS7ServerCommunication::OpenDevice()
{
    if (commDiviceState) {
        QMessageBox::information(nullptr, "提示", "服务器已经打开，请勿重复打开");
        return -1;
    }

    if (ptrS7Server->serverValue == nullptr) {
        emit SendLogInfoSynSig(QString("%1-服务器启动失败：serverValue 未初始化").arg(deviceName),
            LOGTYPE::ERRORS);
        return -1;
    }

    //ptrS7Server->serverValue->SetServerTSAP(0x0100); // 设置 TSAP 兼容 S7 Basic
    int result = ptrS7Server->serverValue->StartTo(ipValue.toStdString().c_str());
    if (result == 0) {
        commDiviceState = true;
        ptrS7Server->connectState = connectState;
        int clients = ptrS7Server->serverValue->ClientsCount();
        emit SendLogInfoSynSig(QString("%1-服务器启动成功，监听地址: %2, 端口: %3, 客户端数: %4")
            .arg(deviceName).arg(ipValue).arg(portValue).arg(clients),
            LOGTYPE::INFO);
    }
    else {
        commDiviceState = false;
        ptrS7Server->connectState = disconnectState;
        emit SendLogInfoSynSig(QString("%1-服务器启动失败，错误码: %2")
            .arg(deviceName).arg(result),
            LOGTYPE::ERRORS);
    }
    return result;
}

int SimensS7ServerCommunication::CloseDevice()
{
    ptrS7Server->Close();
    commDiviceState = false;
    ptrS7Server->connectState = disconnectState;
    emit SendLogInfoSynSig(QString("%1-服务器已关闭").arg(deviceName), LOGTYPE::INFO);
    return 0;
}

int SimensS7ServerCommunication::Write(const QByteArray& byte_array)
{
    if (!commDiviceState) {
        emit SendLogInfoSynSig(QString("%1-写入失败：服务器未打开").arg(deviceName),
            LOGTYPE::ERRORS);
        return -1;
    }

    std::lock_guard<std::mutex> lock(ptrS7Server->mtx);
    ptrS7Server->serverValue->LockArea(srvAreaDB, 1);
    size_t size = std::min(static_cast<size_t>(byte_array.size()), sizeof(db1_));
    std::memcpy(db1_, byte_array.constData(), size);
    ptrS7Server->serverValue->UnlockArea(srvAreaDB, 1);

    emit SendLogInfoSynSig(QString("%1-写入 DB1，数据大小: %2, 数据: %3")
        .arg(deviceName).arg(size).arg(byte_array.toHex().constData()),
        LOGTYPE::INFO);
    return 0;
}

int SimensS7ServerCommunication::WriteOutputs(const QByteArray& byte_array)
{
    if (!commDiviceState) {
        emit SendLogInfoSynSig(QString("%1-写入 Outputs 失败：服务器未打开").arg(deviceName),
            LOGTYPE::ERRORS);
        return -1;
    }

    std::lock_guard<std::mutex> lock(ptrS7Server->mtx);
    ptrS7Server->serverValue->LockArea(srvAreaPA, 0);
    size_t size = std::min(static_cast<size_t>(byte_array.size()), sizeof(outputs_));
    std::memcpy(outputs_, byte_array.constData(), size);
    ptrS7Server->serverValue->UnlockArea(srvAreaPA, 0);

    emit SendLogInfoSynSig(QString("%1-写入 Outputs，数据大小: %2, 数据: %3")
        .arg(deviceName).arg(size).arg(byte_array.toHex().constData()),
        LOGTYPE::INFO);
    return 0;
}

int SimensS7ServerCommunication::WriteInputs(const QByteArray& byte_array)
{
    if (!commDiviceState) {
        emit SendLogInfoSynSig(QString("%1-写入 Inputs 失败：服务器未打开").arg(deviceName),
            LOGTYPE::ERRORS);
        return -1;
    }

    std::lock_guard<std::mutex> lock(ptrS7Server->mtx);
    ptrS7Server->serverValue->LockArea(srvAreaPE, 0);
    size_t size = std::min(static_cast<size_t>(byte_array.size()), static_cast<size_t>(128));
    std::memcpy(inputs_, byte_array.constData(), size);
    ptrS7Server->serverValue->UnlockArea(srvAreaPE, 0);

    emit SendLogInfoSynSig(QString("%1-写入 Inputs，数据大小: %2, 数据: %3")
        .arg(deviceName).arg(size).arg(byte_array.toHex().constData()),
        LOGTYPE::INFO);
    return 0;
}

int SimensS7ServerCommunication::WriteMerkers(const QByteArray& byte_array)
{
    if (!commDiviceState) {
        emit SendLogInfoSynSig(QString("%1-写入 Merkers 失败：服务器未打开").arg(deviceName),
            LOGTYPE::ERRORS);
        return -1;
    }

    std::lock_guard<std::mutex> lock(ptrS7Server->mtx);
    ptrS7Server->serverValue->LockArea(srvAreaMK, 0);
    size_t size = std::min(static_cast<size_t>(byte_array.size()), static_cast<size_t>(256));
    std::memcpy(merkers_, byte_array.constData(), size);
    ptrS7Server->serverValue->UnlockArea(srvAreaMK, 0);

    emit SendLogInfoSynSig(QString("%1-写入 Merkers，数据大小: %2, 数据: %3")
        .arg(deviceName).arg(size).arg(byte_array.toHex().constData()),
        LOGTYPE::INFO);
    return 0;
}

int SimensS7ServerCommunication::Read(QByteArray& byte_array)
{
    if (!commDiviceState) {
        emit SendLogInfoSynSig(QString("%1-读取失败：服务器未打开").arg(deviceName),
            LOGTYPE::ERRORS);
        return -1;
    }

    if (queueReadByteArray.try_pop(byte_array)) {
        emit SendLogInfoSynSig(QString("%1-从客户端队列读取数据，数据大小: %2, 数据: %3")
            .arg(deviceName).arg(byte_array.size()).arg(byte_array.toHex().constData()),
            LOGTYPE::INFO);
        qDebug() << "DEBUG: Queue popped, size:" << byte_array.size() << ", data:" << byte_array.toHex();
        return 0;
    }else {
        // 如果没有数据，返回错误
        emit SendLogInfoSynSig(QString("%1-队列为空").arg(deviceName),
            LOGTYPE::ERRORS);
        return 0;
    }
}

void SimensS7ServerCommunication::ServerCallback(void* usrPtr, PSrvEvent Event, int Size)
{
    auto* self = static_cast<SimensS7ServerCommunication*>(usrPtr);
    QString msg;
    switch (Event->EvtCode) {
    case evcClientAdded:
        msg = QString("%1-客户端连接，IP: %2").arg(self->deviceName).arg(Event->EvtSender);
        qDebug() << "DEBUG: Client connected, IP:" << Event->EvtSender;
        break;
    case evcClientDisconnected:
        msg = QString("%1-客户端断开，IP: %2").arg(self->deviceName).arg(Event->EvtSender);
        qDebug() << "DEBUG: Client disconnected, IP:" << Event->EvtSender;
        break;
    case evcDataWrite:
        msg = QString("%1-客户端写入 DB%2，偏移: %3，长度: %4")
            .arg(self->deviceName).arg(Event->EvtParam1).arg(Event->EvtParam2).arg(Event->EvtParam3);
        qDebug() << "DEBUG: Data write, DB:" << Event->EvtParam1 << ", offset:" << Event->EvtParam2;
        break;
    case evcDataRead:
        msg = QString("%1-客户端读取 DB%2，偏移: %3，长度: %4")
            .arg(self->deviceName).arg(Event->EvtParam1).arg(Event->EvtParam2).arg(Event->EvtParam3);
        qDebug() << "DEBUG: Data read, DB:" << Event->EvtParam1 << ", offset:" << Event->EvtParam2;
        break;
    default:
        msg = QString("%1-未知事件，代码: %2").arg(self->deviceName).arg(Event->EvtCode);
        qDebug() << "DEBUG: Unknown event, code:" << Event->EvtCode;
        break;
    }

    // 更新客户端数量
    /*if (Event->EvtCode == evcClientAdded ||
        Event->EvtCode == evcClientDisconnected) {
        int clients = self->ptrS7Server->serverValue->ClientsCount();
        QMetaObject::invokeMethod(self, [self, clients]() {
            emit self->ClientCountChanged(clients);
            }, Qt::QueuedConnection);
    }*/
    emit self->SendLogInfoAsynSig(msg, LOGTYPE::INFO);
}

int SimensS7ServerCommunication::RWAreaCallback(void* usrPtr, int Sender, int Operation, PS7Tag PTag, void* pUsrData)
{
    auto* self = static_cast<SimensS7ServerCommunication*>(usrPtr);
    QString op = (Operation == OperationRead) ? "读取" : "写入";
    
    std::lock_guard<std::mutex> lock(self->ptrS7Server->mtx);
    
    // 确定要操作的内存区域 - 使用客户端区域地址
    byte* targetBuffer = nullptr;
    int bufferSize = 0;
    QString areaName;
    QString msgDebug;
    
    switch (PTag->Area) {
    case S7AreaDB:  // 使用客户端定义的DB区域地址 0x84
        if (PTag->DBNumber == 1) {
            targetBuffer = self->db1_;
            bufferSize = 100;
            areaName = "DB1";
        }
        break;
    case S7AreaPE:  // 使用客户端定义的输入区域地址 0x81
        targetBuffer = self->inputs_;
        bufferSize = 10;
        areaName = "Inputs";
        break;
    case S7AreaPA:  // 使用客户端定义的输出区域地址 0x82
        targetBuffer = self->outputs_;
        bufferSize = 10;
        areaName = "Outputs";
        break;
    default:
        areaName = QString("未知区域(0x%1)").arg(PTag->Area, 0, 16);
        break;
    }
    
    if (targetBuffer && PTag->Start + PTag->Size <= bufferSize) {
        if (Operation == OperationRead) {
            // 客户端读取数据：从服务器缓冲区复制到客户端
            memcpy(pUsrData, targetBuffer + PTag->Start, PTag->Size);
            QString dataHex = QByteArray((char*)targetBuffer + PTag->Start, PTag->Size).toHex(' ');
            msgDebug = QString("%1-客户端读取 %2 偏移 %3 大小 %4 数据: %5")
                .arg(self->deviceName)
                .arg(areaName)
                .arg(PTag->Start)
                .arg(PTag->Size)
                .arg(dataHex);
        } else if (Operation == OperationWrite) {
            // 客户端写入数据：从客户端复制到服务器缓冲区
            memcpy(targetBuffer + PTag->Start, pUsrData, PTag->Size);
            QString dataHex = QByteArray((char*)pUsrData, PTag->Size).toHex(' ');
            msgDebug = QString("%1-客户端写入 %2 偏移 %3 大小 %4 数据: %5")
                .arg(self->deviceName)
                .arg(areaName)
                .arg(PTag->Start)
                .arg(PTag->Size)
                .arg(dataHex);
            // 如果是写入DB1，记录到队列
            if (PTag->Area == S7AreaDB && PTag->DBNumber == 1) {
                QByteArray data(static_cast<char*>(pUsrData), PTag->Size);
                self->queueReadByteArray.push(data);
            }
        }
    } else {
        msgDebug = QString("%1-无效访问 - 区域: %2 起始: %3 大小: %4 缓冲区大小: %5")
            .arg(self->deviceName)
            .arg(areaName)
            .arg(PTag->Start)
            .arg(PTag->Size)
            .arg(bufferSize);
    }
    
    QString msg = QString("%1-客户端 %2 %3 区域: %4, DB: %5, 偏移: %6, 大小: %7")
        .arg(self->deviceName).arg(Sender).arg(op).arg(areaName)
        .arg(PTag->DBNumber).arg(PTag->Start).arg(PTag->Size);

    emit self->SendLogInfoAsynSig(msgDebug, LOGTYPE::INFO);
    emit self->SendLogInfoAsynSig(msg, LOGTYPE::INFO);
    
    return 0; // 返回0表示成功
}