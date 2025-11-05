#include "SocketTcpServerCommunicate.h"
#include <QMessageBox>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QtNetwork>
#include "../Log/frmLog.h" 

SocketTcpServerCommunicate::SocketTcpServerCommunicate()
{
    FrmLog* ptr_framlog = FrmLog::FrmLogInstance();
    QObject::connect(this, &SocketTcpServerCommunicate::SendLogInfoSynSig, ptr_framlog, &FrmLog::LogMassageShowSlot);
}

SocketTcpServerCommunicate::~SocketTcpServerCommunicate()
{

}

int SocketTcpServerCommunicate::InitalDevice(const CGDevice::Ptr ptr_device)
{
    if (ptr_device == nullptr) {
        return -1;
    }
    ptrSocketTcpServer =   std::static_pointer_cast<SocketTcpServer>(ptr_device);
    if (ptrSocketTcpServer->serverValue == nullptr) { 
        ptrSocketTcpServer->serverValue = new QTcpServer();
    }
    tcpSocketSever = ptrSocketTcpServer->serverValue;
    ipValue = ptrSocketTcpServer->ipValue;
    portValue = ptrSocketTcpServer->portValue;
    deviceName = ptrSocketTcpServer->deviceName;
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    return 0;
}

int SocketTcpServerCommunicate::OpenDevice()
{
    bool server_state;
    if (ipValue == "255.255.255.0") {
        server_state = tcpSocketSever->listen(QHostAddress::Any, portValue);
    }
    else {
        server_state = tcpSocketSever->listen(QHostAddress(ipValue), portValue);
    }
    QString try_str = tcpSocketSever->errorString();
    emit SendLogInfoSynSig("Sever打开失败：" + try_str, LOGTYPE::INFO);

    connect(tcpSocketSever, &QTcpServer::newConnection, this, [this]() {
        auto	tcp_socket = tcpSocketSever->nextPendingConnection();
        if (tcp_socket == NULL) { 
            emit SendLogInfoSynSig("Client设备为空，请检查服务端是否存在问题后，重新链接! ", LOGTYPE::ERRORS);
            return;
        }
        ptrSocketTcpServer->qVclientValue.append(tcp_socket);
        QString str = QString(deviceName + "-[ip:%1,port:%2]").arg(tcp_socket->peerAddress().toString()).arg(tcp_socket->peerPort());
        emit SendLogInfoSynSig(str, LOGTYPE::INFO);

        connect(tcp_socket, &QTcpSocket::readyRead, this, [this, tcp_socket]() {
            std::lock_guard<std::mutex> lg(ptrSocketTcpServer->mtx);
            ptrSocketTcpServer->clientValue = tcp_socket;
            QByteArray buf = tcp_socket->readAll();//readAll最多接收65532的数据
            QString str = QString(deviceName + "-[ip:%1,port:%2]-data: %3").arg(tcp_socket->peerAddress().toString()).arg(tcp_socket->peerPort()).arg(QString(buf));
            queueReadByteArray.push(buf);
            emit SendLogInfoSynSig(str, LOGTYPE::INFO);
            });
        /// <summary>
        /// 删除指定client
        /// </summary>
        connect(tcp_socket, &QTcpSocket::disconnected, this, [this]() {
            QTcpSocket* qtcp_socket = static_cast<QTcpSocket*>(sender());
            for (int i = 0; i < ptrSocketTcpServer->qVclientValue.size(); ) {
                QTcpSocket* v_tcp = ptrSocketTcpServer->qVclientValue[i];
                if (v_tcp == qtcp_socket) {
                    ptrSocketTcpServer->qVclientValue.removeAt(i);
                }
                else {
                    i++;
                }
            }
            qtcp_socket->deleteLater();
            qtcp_socket = nullptr;
            });
        });

    if (server_state == true) {
        commDiviceState = true;
        return 0;
    }
    else {
        QMessageBox::StandardButton result = QMessageBox::information(nullptr, "提示", "服务器已经打开，请勿重复打开");
        return -1;
    }
}

int SocketTcpServerCommunicate::CloseDevice()
{
    ptrSocketTcpServer->Close();
    commDiviceState = false;
    return 0;
}

int SocketTcpServerCommunicate::Write(const QByteArray& byte_array)
{
    std::unique_lock<std::mutex> thread_locker(ptrSocketTcpServer->mtx);
    QTcpSocket* tcp_client = ptrSocketTcpServer->clientValue;
    if (tcp_client == nullptr) {
        for (auto client : ptrSocketTcpServer->qVclientValue){
            if (client == nullptr) {
                continue;
            }
            client->write(byte_array);
            client->waitForBytesWritten();
        }
        return 0;
    }
    tcp_client->write(byte_array);
    tcp_client->waitForBytesWritten();
    return 0;
}

int SocketTcpServerCommunicate::Read(QByteArray& byte_array)
{
    if (queueReadByteArray.try_pop(byte_array)) {
        return 0;
    }
    return -1;
}

