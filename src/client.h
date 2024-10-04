#ifndef MTB_NET_LIB_CLIENT_H
#define MTB_NET_LIB_CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QTimer>

constexpr size_t CLIENT_KEEP_ALIVE_SEND_PERIOD_MS = 5000;

class DaemonClient : public QObject {
    Q_OBJECT

public:
    DaemonClient(QObject *parent = nullptr);
    void connect(const QHostAddress&, quint16 port, bool keepAlive=true);
    void disconnect();
    void send(const QJsonObject&);
    bool connected() const;

private slots:
    void clientConnected();
    void clientDisconnected();
    void clientReadyRead();
    void tKeepAliveTick();
    void clientErrorOccured(QAbstractSocket::SocketError);

private:
    QTcpSocket m_socket;
    QTimer m_tKeepAlive;

signals:
    void jsonReceived(const QJsonObject&);
    void onConnected();
    void onDisconnected();

};

#endif
