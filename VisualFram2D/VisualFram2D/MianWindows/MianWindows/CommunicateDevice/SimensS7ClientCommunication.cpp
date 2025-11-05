#include "SimensS7ClientCommunication.h"
#include <QMessageBox>
#include <QDebug>
#include "../Log/frmLog.h"

SimensS7ClientCommunication::SimensS7ClientCommunication()
{
    FrmLog* ptr_framlog = FrmLog::FrmLogInstance();
    QObject::connect(this, &SimensS7ClientCommunication::SendLogInfoSynSig, ptr_framlog, &FrmLog::LogMassageShowSlot);
    QObject::connect(this, &SimensS7ClientCommunication::SendLogInfoAsynSig, ptr_framlog, &FrmLog::LogMassageShowSlot, Qt::BlockingQueuedConnection);
}

SimensS7ClientCommunication::~SimensS7ClientCommunication()
{
    CloseDevice();
}

int SimensS7ClientCommunication::InitalDevice(const CGDevice::Ptr ptr_device)
{
    if (!ptr_device || ptr_device->deviceMode != DeviceMode::S7Client) {
        emit SendLogInfoSynSig("初始化失败：设备配置为空或设备模式不匹配", LOGTYPE::ERRORS);
        return -1;
    }

    ptrS7Client = std::static_pointer_cast<SimensS7Client>(ptr_device);
    deviceName = ptrS7Client->deviceName;
    ipValue = ptrS7Client->ipValue;
    portValue = ptrS7Client->portValue;
    rackValue = ptrS7Client->rackValue;
    slotValue = ptrS7Client->slotValue;

    if (ptrS7Client->clientValue == nullptr) {
        ptrS7Client->clientValue = new TS7Client();
    }

    emit SendLogInfoSynSig(QString("%1-客户端初始化成功，目标: %2:%3, 机架: %4, 插槽: %5")
        .arg(deviceName).arg(ipValue).arg(portValue).arg(rackValue).arg(slotValue),
        LOGTYPE::INFO);
    return 0;
}

int SimensS7ClientCommunication::OpenDevice()
{
    if (commDiviceState) {
        emit SendLogInfoSynSig("客户端已经连接，请勿重复连接", LOGTYPE::INFO);
        return -1;
    }

    if (ptrS7Client->clientValue == nullptr) {
        emit SendLogInfoSynSig(QString("%1-客户端连接失败：clientValue 未初始化").arg(deviceName),
            LOGTYPE::ERRORS);
        return -1;
    }

    int result = ptrS7Client->clientValue->ConnectTo(ipValue.toStdString().c_str(),
        rackValue, slotValue);
    if (result == 0) {
        commDiviceState = true;
        ptrS7Client->connectState = connectState;
        emit SendLogInfoSynSig(QString("%1-客户端连接成功，远程地址: %2, 机架: %3, 插槽: %4")
            .arg(deviceName).arg(ipValue).arg(rackValue).arg(slotValue),
            LOGTYPE::INFO);
    }
    else {
        commDiviceState = false;
        ptrS7Client->connectState = disconnectState;
        emit SendLogInfoSynSig(QString("%1-客户端连接失败，错误码: %2")
            .arg(deviceName).arg(result),
            LOGTYPE::ERRORS);
    }
    return result;
}

int SimensS7ClientCommunication::CloseDevice()
{
    if (ptrS7Client && ptrS7Client->clientValue) {
        ptrS7Client->clientValue->Disconnect();
    }
    commDiviceState = false;
    if (ptrS7Client) {
        ptrS7Client->connectState = disconnectState;
    }
    emit SendLogInfoSynSig(QString("%1-客户端已断开").arg(deviceName), LOGTYPE::INFO);
    return 0;
}

int SimensS7ClientCommunication::Write(const QByteArray& byte_array)
{
    if (!commDiviceState) {
        emit SendLogInfoSynSig(QString("%1-写入失败：客户端未连接").arg(deviceName),
            LOGTYPE::ERRORS);
        return -1;
    }

    // 默认写入DB1
    return WriteDB(12, 0, byte_array);
}

int SimensS7ClientCommunication::Read(QByteArray& byte_array)
{
    if (!commDiviceState) {
        emit SendLogInfoSynSig(QString("%1-读取失败：客户端未连接").arg(deviceName),
            LOGTYPE::ERRORS);
        return -1;
    }

    // 默认读取DB1的前100字节
    return ReadDB(12, 0, 100, byte_array);
}

int SimensS7ClientCommunication::ReadDB(int dbNumber, int start, int size, QByteArray& data)
{
    if (!commDiviceState) {
        return -1;
    }

    QByteArray buffer(size, 0);
    int result = ptrS7Client->clientValue->DBRead(dbNumber, start, size, buffer.data());
    if (result == 0) {
        data = buffer;
        emit SendLogInfoSynSig(QString("%1-读取 DB%2，起始: %3, 大小: %4, 数据: %5")
            .arg(deviceName).arg(dbNumber).arg(start).arg(size).arg(data.toHex().constData()),
            LOGTYPE::INFO);
    }
    else {
        emit SendLogInfoSynSig(QString("%1-读取 DB%2 失败，错误码: %3")
            .arg(deviceName).arg(dbNumber).arg(result),
            LOGTYPE::ERRORS);
    }
    return result;
}

int SimensS7ClientCommunication::WriteDB(int dbNumber, int start, const QByteArray& data)
{
    if (!commDiviceState) {
        return -1;
    }

    int result = ptrS7Client->clientValue->DBWrite(
        dbNumber,
        start,
        data.size(),
        const_cast<void*>(static_cast<const void*>(data.constData()))
    );

    if (result == 0) {
        emit SendLogInfoSynSig(QString("%1-写入 DB%2，起始: %3, 大小: %4, 数据: %5")
            .arg(deviceName).arg(dbNumber).arg(start).arg(data.size()).arg(data.toHex().constData()),
            LOGTYPE::INFO);
    }
    else {
        emit SendLogInfoSynSig(QString("%1-写入 DB%2 失败，错误码: %3")
            .arg(deviceName).arg(dbNumber).arg(result),
            LOGTYPE::ERRORS);
    }
    return result;
}

int SimensS7ClientCommunication::ReadInputs(int start, int size, QByteArray& data)
{
    if (!commDiviceState) {
        return -1;
    }

    QByteArray buffer(size, 0);
    // 修复：添加WordLen参数，对于字节数据使用S7WLByte
    int result = ptrS7Client->clientValue->ReadArea(
        S7AreaPE,           // Area
        0,                  // DBNumber (对于PE/PA/MK区域为0)
        start,              // Start
        size,               // Amount (字节数量)
        S7WLByte,           // WordLen (字节)
        static_cast<void*>(buffer.data())  // pUsrData
    );

    if (result == 0) {
        data = buffer;
        emit SendLogInfoSynSig(QString("%1-读取输入区，起始: %2, 大小: %3, 数据: %4")
            .arg(deviceName).arg(start).arg(size).arg(data.toHex().constData()),
            LOGTYPE::INFO);
    }
    else {
        emit SendLogInfoSynSig(QString("%1-读取输入区失败，错误码: %2")
            .arg(deviceName).arg(result),
            LOGTYPE::ERRORS);
    }
    return result;
}

int SimensS7ClientCommunication::WriteOutputs(int start, const QByteArray& data)
{
    if (!commDiviceState) {
        return -1;
    }

    // 修复：添加WordLen参数
    int result = ptrS7Client->clientValue->WriteArea(
        S7AreaPA,           // Area
        0,                  // DBNumber (对于PA区域为0)
        start,              // Start
        data.size(),        // Amount (字节数量)
        S7WLByte,           // WordLen (字节)
        const_cast<void*>(static_cast<const void*>(data.constData()))  // pUsrData
    );

    if (result == 0) {
        emit SendLogInfoSynSig(QString("%1-写入输出区，起始: %2, 大小: %3, 数据: %4")
            .arg(deviceName).arg(start).arg(data.size()).arg(data.toHex().constData()),
            LOGTYPE::INFO);
    }
    else {
        emit SendLogInfoSynSig(QString("%1-写入输出区失败，错误码: %2")
            .arg(deviceName).arg(result),
            LOGTYPE::ERRORS);
    }
    return result;
}

int SimensS7ClientCommunication::ReadMerkers(int start, int size, QByteArray& data)
{
    if (!commDiviceState) {
        return -1;
    }

    QByteArray buffer(size, 0);
    // 修复：添加WordLen参数
    int result = ptrS7Client->clientValue->ReadArea(
        S7AreaMK,           // Area
        0,                  // DBNumber (对于MK区域为0)
        start,              // Start
        size,               // Amount (字节数量)
        S7WLByte,           // WordLen (字节)
        static_cast<void*>(buffer.data())  // pUsrData
    );

    if (result == 0) {
        data = buffer;
        emit SendLogInfoSynSig(QString("%1-读取位存储区，起始: %2, 大小: %3, 数据: %4")
            .arg(deviceName).arg(start).arg(size).arg(data.toHex().constData()),
            LOGTYPE::INFO);
    }
    else {
        emit SendLogInfoSynSig(QString("%1-读取位存储区失败，错误码: %2")
            .arg(deviceName).arg(result),
            LOGTYPE::ERRORS);
    }
    return result;
}

int SimensS7ClientCommunication::WriteMerkers(int start, const QByteArray& data)
{
    if (!commDiviceState) {
        return -1;
    }

    // 修复：添加WordLen参数
    int result = ptrS7Client->clientValue->WriteArea(
        S7AreaMK,           // Area
        0,                  // DBNumber (对于MK区域为0)
        start,              // Start
        data.size(),        // Amount (字节数量)
        S7WLByte,           // WordLen (字节)
        const_cast<void*>(static_cast<const void*>(data.constData()))  // pUsrData
    );

    if (result == 0) {
        emit SendLogInfoSynSig(QString("%1-写入位存储区，起始: %2, 大小: %3, 数据: %4")
            .arg(deviceName).arg(start).arg(data.size()).arg(data.toHex().constData()),
            LOGTYPE::INFO);
    }
    else {
        emit SendLogInfoSynSig(QString("%1-写入位存储区失败，错误码: %2")
            .arg(deviceName).arg(result),
            LOGTYPE::ERRORS);
    }
    return result;
}