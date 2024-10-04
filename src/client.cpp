#include <QJsonDocument>
#include <QJsonObject>
#include <QHostAddress>
#include "client.h"

DaemonClient::DaemonClient(QObject *parent) : QObject(parent) {
    QObject::connect(&this->m_tKeepAlive, SIGNAL(timeout()), this, SLOT(tKeepAliveTick()));
    QObject::connect(&m_socket, SIGNAL(connected()), this, SLOT(clientConnected()));
    QObject::connect(&m_socket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    QObject::connect(&m_socket, SIGNAL(readyRead()), this, SLOT(clientReadyRead()));

#if QT_VERSION < 0x060000
    QObject::connect(&m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
                     this, SLOT(clientErrorOccured(QAbstractSocket::SocketError)));
#else
    QObject::connect(&m_socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)),
                     this, SLOT(clientErrorOccured(QAbstractSocket::SocketError)));
#endif
}

void DaemonClient::connect(const QHostAddress &addr, quint16 port, bool keepAlive) {
    //log("Connecting to MTB daemon server: "+addr.toString()+":"+QString::number(port)+" ...", LogLevel::Info);
    this->m_socket.connectToHost(addr, port);
    if (keepAlive)
        this->m_tKeepAlive.start(CLIENT_KEEP_ALIVE_SEND_PERIOD_MS);
}

void DaemonClient::disconnect() {
    this->m_socket.abort();
}

bool DaemonClient::connected() const {
    return this->m_socket.state() == QAbstractSocket::SocketState::ConnectedState;
}

void DaemonClient::clientConnected() {
    emit onConnected();
}

void DaemonClient::clientDisconnected() {
    this->m_tKeepAlive.stop();
    // client->deleteLater();
    emit onDisconnected();
}

void DaemonClient::clientErrorOccured(QAbstractSocket::SocketError) {
    //log("Daemon server socket error occured: "+m_socket.errorString(), LogLevel::Error);
    if (this->connected())
        this->disconnect();
    emit onDisconnected();
}

void DaemonClient::clientReadyRead() {
    while (this->m_socket.canReadLine()) {
        QByteArray data = this->m_socket.readLine();
        if (data.size() > 0) {
            QJsonObject json = QJsonDocument::fromJson(data).object();
            emit jsonReceived(json);
        }
    }
}

void DaemonClient::send(const QJsonObject &jsonObj) {
    if (!this->connected())
        return; // TODO: throw exception?
    QByteArray data = QJsonDocument(jsonObj).toJson(QJsonDocument::Compact);
    data.push_back("\n\n");
    this->m_socket.write(data);
}

void DaemonClient::tKeepAliveTick() {
    if (this->connected())
        this->send({});
}
