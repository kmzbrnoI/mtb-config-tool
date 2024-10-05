#include <QJsonDocument>
#include <QJsonObject>
#include <QHostAddress>
#include <QVector>
#include "client.h"

DaemonClient::DaemonClient(QObject *parent) : QObject(parent) {
    QObject::connect(&this->m_tKeepAlive, SIGNAL(timeout()), this, SLOT(tKeepAliveTick()));
    QObject::connect(&this->m_tSent, SIGNAL(timeout()), this, SLOT(tSentTick()));
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
    this->m_tSent.start(CLIENT_SENT_POLL_PERIOD_MS);
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
    this->m_tSent.stop();

    for (const auto& sent : this->m_sent)
        this->callError(sent.onError, DaemonClientError::Disconnected);
    this->m_sent.clear();

    // client->deleteLater();
    emit onDisconnected();
}

void DaemonClient::clientErrorOccured(QAbstractSocket::SocketError) {
    qDebug() << tr("Daemon server socket error occured: ")+m_socket.errorString();
    if (this->connected())
        this->disconnect();
    emit onDisconnected();
}

void DaemonClient::clientReadyRead() {
    while (this->m_socket.canReadLine()) {
        QByteArray data = this->m_socket.readLine();
        if (data.size() > 0) {
            QJsonObject json = QJsonDocument::fromJson(data).object();
            this->msgReceived(json);
        }
    }
}

void DaemonClient::msgReceived(const QJsonObject& json) {
    if ((!json.contains("command")) || (!json.contains("type")))
        return;
    if ((json["type"] == "response") && (json.contains("id"))) {
        const unsigned id = json["id"].toInt();
        if ((id < 0x10000) && (this->m_sent.contains(id)) && (json.contains("status"))) {
            const SentCommand sent = this->m_sent[id];
            this->m_sent.remove(id);
            if (json["status"] == "ok") {
                sent.onOk(json);
            } else if (json["status"] == "error") {
                sent.onError(json["error"]["code"].toInt(), json["error"]["message"].toString());
            } else {
                qDebug() << "Unknown json['status']: " << json["status"] << Qt::endl;
            }
        }
    }

    emit jsonReceived(json);
}

void DaemonClient::tSentTick() {
    QVector<uint16_t> idsToRemove;

    for (const auto& [id, sent] : this->m_sent.asKeyValueRange()) {
        if (QDateTime::currentDateTime() >= sent.timeout) {
            this->callError(sent.onError, DaemonClientError::Timeout);
            idsToRemove.append(id);
        }
    }

    for (const uint16_t id : idsToRemove)
        this->m_sent.remove(id);
}

void DaemonClient::send(const QJsonObject &jsonObj) {
    if (!this->connected())
        throw EDisconnected();
    QByteArray data = QJsonDocument(jsonObj).toJson(QJsonDocument::Compact);
    data.push_back("\n\n");
    this->m_socket.write(data);
}

void DaemonClient::tKeepAliveTick() {
    if (this->connected())
        this->send({});
}

void DaemonClient::send(QJsonObject jsonObj, ResponseOkEvent&& onOk, ResponseErrorEvent&& onErr) {
    if (!jsonObj.contains("command"))
        throw EInvalidRequest(tr("No 'command' present in the outgoing json!"));

    const int id = this->m_sendNextId;
    this->m_sendNextId++;
    if (this->m_sendNextId == 0)
        this->m_sendNextId = 1;

    jsonObj["id"] = id;
    jsonObj["type"] = "request";
    const QDateTime timeout = QDateTime::currentDateTime().addSecs(this->timeoutSec(jsonObj));

    this->send(jsonObj); // in case of exception, do not add to m_sent
    this->m_sent.insert(id, SentCommand(std::move(onOk), std::move(onErr), timeout));
}

void DaemonClient::sendNoExc(const QJsonObject& jsonObj, ResponseOkEvent&& onOk, ResponseErrorEvent&& onErr) {
    try {
        this->send(jsonObj, std::move(onOk), std::move(onErr));
    } catch (const EInvalidRequest&) {
        DaemonClient::callError(onErr, DaemonClientError::InvalidJson);
    } catch (const EDisconnected&) {
        DaemonClient::callError(onErr, DaemonClientError::Disconnected);
    } catch (...) {
        onErr(DaemonClientError::General, tr("Unknown exception when calling DaemonClient::send!"));
    }
}

unsigned DaemonClient::timeoutSec(const QJsonObject& jsonObj) const {
    if (!jsonObj.contains("command"))
        throw EInvalidRequest(tr("No 'command' present in the outgoing json!"));
    return (jsonObj["command"] == "module_upgrade_fw") ? 60 : 1; // long timeout for FW upgrade
}

void DaemonClient::callError(const ResponseErrorEvent& f, DaemonClientError err) {
    f(err, _ERROR_TEXT[err]);
}
