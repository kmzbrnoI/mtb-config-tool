#ifndef MTB_NET_LIB_CLIENT_H
#define MTB_NET_LIB_CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QTimer>
#include <QMap>
#include <functional>
#include "q-str-exception.h"

enum DaemonClientError { // there are more error codes (received from MTB Daemon)
    Timeout = 110,
    Disconnected = 111,
    General = 112,
    InvalidJson = 1000,
};

const QString STR_CLIENT_NOT_CONNECTED = QObject::tr("Client is not connected to the server!");

const QMap<DaemonClientError, QString> _ERROR_TEXT = {
    {DaemonClientError::Timeout, QObject::tr("Timeout waiting for response!")},
    {DaemonClientError::Disconnected, STR_CLIENT_NOT_CONNECTED},
    {DaemonClientError::General, QObject::tr("General error!")},
    {DaemonClientError::InvalidJson, QObject::tr("Invalid json!")},
};

using ResponseOkEvent = std::function<void(const QJsonObject& content)>;
using ResponseErrorEvent = std::function<void(unsigned errorCode, QString errorMessage)>;

struct EDaemonClientError : public QStrException {
    EDaemonClientError(const QString& str) : QStrException(str) {}
};
struct EInvalidRequest : public EDaemonClientError {
    EInvalidRequest(const QString& str) : EDaemonClientError(str) {}
};
struct EDisconnected : public EDaemonClientError {
    EDisconnected(const QString& str = STR_CLIENT_NOT_CONNECTED) : EDaemonClientError(str) {}
};

struct SentCommand {
    QDateTime timeout;
    ResponseOkEvent onOk;
    ResponseErrorEvent onError;

    SentCommand() {};
    SentCommand(ResponseOkEvent&& onOk, ResponseErrorEvent&& onError, const QDateTime timeout)
        : timeout(timeout), onOk(std::move(onOk)), onError(std::move(onError)) {}
};

constexpr unsigned CLIENT_KEEP_ALIVE_SEND_PERIOD_MS = 1000;
constexpr unsigned CLIENT_SENT_POLL_PERIOD_MS = 100;

class DaemonClient : public QObject {
    Q_OBJECT

public:
    static DaemonClient* instance;

    DaemonClient(QObject *parent = nullptr);
    void connect(const QHostAddress&, quint16 port, bool keepAlive=true);
    void disconnect();
    void send(QJsonObject, ResponseOkEvent&&, ResponseErrorEvent&&);
    void sendNoExc(const QJsonObject&, ResponseOkEvent&&, ResponseErrorEvent&&);
    bool connected() const;

    static QString standardErrrorMessage(const QString& command, unsigned errorCode, QString errorMessage);

private slots:
    void clientConnected();
    void clientDisconnected();
    void clientReadyRead();
    void tKeepAliveTick();
    void tSentTick();
    void clientErrorOccured(QAbstractSocket::SocketError);

private:
    QTcpSocket m_socket;
    QTimer m_tKeepAlive;
    QTimer m_tSent;
    uint16_t m_sendNextId = 1; // smaller type for sure
    QMap<unsigned, SentCommand> m_sent; // sent & waiting for response
    bool connecting = false;

    void send(const QJsonObject&);
    unsigned timeoutSec(const QJsonObject&) const;
    void msgReceived(const QJsonObject&);

    static void callError(const ResponseErrorEvent&, DaemonClientError);

signals:
    void jsonReceived(const QJsonObject&);
    void onConnected();
    void onDisconnected();
    void connectError(const QString& msg);

};

#endif
