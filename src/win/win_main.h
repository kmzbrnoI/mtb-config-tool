#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QLabel>
#include <array>
#include <optional>
#include "ui_mainwindow.h"
#include "settings.h"
#include "win_settings.h"
#include "client.h"
#include "win_log.h"

constexpr unsigned MTBBUS_ADDR_COUNT = 256;
const QVector<QString> DAEMON_SUPPORTED_VERSIONS{"1.5"};
QString daemonSupportedVersionsStr();

struct DaemonVersion {
    unsigned major;
    unsigned minor;

    DaemonVersion(unsigned major, unsigned minor) : major(major), minor(minor) {}
    QString str() const { return QString::number(major)+"."+QString::number(minor); }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Settings& s, QWidget *parent = nullptr);
    void retranslate();

private:
    Ui::MainWindow ui;
    SettingsWindow m_settingsWindow;
    LogWindow m_logWindow;

    Settings& s;
    DaemonClient m_client;
    std::array<QTreeWidgetItem*, MTBBUS_ADDR_COUNT> m_tw_lines; // [0] is not valid
    std::optional<DaemonVersion> m_daemonVersion;

    QLabel m_sb_connection;
    QLabel m_sb_mtbusb;

    void connectedUpdateGui();
    QString daemonHostPort() const;

    void connectingVersionReceived(const QJsonObject&);
    void connectingMtbUsbReceived(const QJsonObject&);
    void clientReceivedMtbUsb(const QJsonObject&);
    void clientReceivedModule(const QJsonObject&);
    void clientReceivedModules(const QJsonObject&);

    void ui_updateModule(const QJsonObject&);
    unsigned ui_twModulesInsertIndex(unsigned addr);
    void ui_twModulesClear();

    void closeEvent(QCloseEvent *event) override;

private slots:
    void ui_MAboutTriggered(bool);
    void ui_AOptionsTriggered(bool);
    void ui_AConnectTriggered(bool);
    void ui_ADisconnectTriggered(bool);
    void ui_AMtbUsbSettingsTriggered(bool);
    void ui_AModulesRefreshTriggered(bool);
    void ui_ADaemonConnectSettingsTriggered(bool);
    void ui_ALogTriggered(bool);

    void clientJsonReceived(const QJsonObject&);
    void clientConnected();
    void clientConnectError(const QString&);
    void clientDisconnected();

}; // class MainWindow

#endif // MAINWINDOW_H
