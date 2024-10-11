#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QLabel>
#include <QMenu>
#include <array>
#include <optional>
#include "ui_mainwindow.h"
#include "settings.h"
#include "win_settings.h"
#include "client.h"
#include "win_log.h"
#include "win_mtbusb.h"
#include "win_config.h"
#include "win_moduleadd.h"

constexpr unsigned MTBBUS_ADDR_COUNT = 256;
const QVector<QString> DAEMON_SUPPORTED_VERSIONS{"1.5"};
QString daemonSupportedVersionsStr();

const QString TEXT_BEACON_OFF = "-";
const QString TEXT_BEACON_ON = QObject::tr("YES");

enum TwModulesColumns {
    twAddrDec,
    twAddrHex,
    twAddrBin,
    twName,
    twType,
    twState,
    twFw,
    twBootloader,
    twError,
    twWarning,
    twBeacon,
};

struct EHexParseError : public QStrException {
    EHexParseError(const QString& str) : QStrException(str) {}
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MainWindow* instance;

    MainWindow(Settings& s, QWidget *parent = nullptr);
    void retranslate();
    void criticalError(const QString&);

private:
    Ui::MainWindow ui;

    Settings& s;
    DaemonClient m_client;
    std::array<QTreeWidgetItem*, MTBBUS_ADDR_COUNT> m_tw_lines; // [0] is not valid
    std::array<QJsonObject, MTBBUS_ADDR_COUNT> m_modules;
    std::array<std::unique_ptr<MtbModuleConfigDialog>, MTBBUS_ADDR_COUNT> m_configWindows;
    std::optional<DaemonVersion> m_daemonVersion;
    bool m_mtbUsbConnected = false;
    std::optional<MtbUsbStatus> m_mtbUsbStatus;

    SettingsWindow m_settingsWindow;
    MtbUsbWindow m_mtbUsbWindow;
    LogWindow m_logWindow;
    ModuleAddDialog m_moduleAddDialog;

    QLabel m_sb_connection;
    QLabel m_sb_mtbusb;
    QLabel m_sb_error;
    QMenu twModulesContextMenu;

    void connectedUpdate();
    QString daemonHostPort() const;

    void connectingVersionReceived(const QJsonObject&);
    void connectingMtbUsbReceived(const QJsonObject&);
    void connectingModulesReceived(const QJsonObject&);

    void clientReceivedMtbUsb(const QJsonObject&);
    void clientReceivedModule(const QJsonObject&);
    void clientReceivedModules(const QJsonObject&);
    void clientReceivedModuleDeleted(const QJsonObject&);

    void moduleDeleted(uint8_t addr);

    void ui_updateModule(const QJsonObject&);
    unsigned ui_twModulesInsertIndex(unsigned addr);
    void ui_twModulesClear();
    void ui_setupModulesContextMenu();

    void closeEvent(QCloseEvent *event) override;
    static QJsonObject loadFwHex(const QString& filename);
    void fwUpgraded(const QJsonObject&);

    void checkModuleTypeChanged(const QJsonObject& module);

private slots:
    void ui_MAboutTriggered(bool);
    void ui_AOptionsTriggered(bool);
    void ui_AConnectTriggered(bool);
    void ui_ADisconnectTriggered(bool);
    void ui_AMtbUsbSettingsTriggered(bool);
    void ui_AModulesRefreshTriggered(bool);
    void ui_ADaemonConnectSettingsTriggered(bool);
    void ui_ADaemonSaveConfigTriggered(bool);
    void ui_ALogTriggered(bool);
    void ui_twCustomContextMenu(const QPoint&);
    void ui_AClearErrorSb();

    void ui_twModulesSelectionChanged();
    void ui_AModuleConfigure();
    void ui_AModuleReboot();
    void ui_AModuleBeacon();
    void ui_AModuleFwUpgrade();
    void ui_AModuleDiagnostics();
    void ui_AModuleAdd();
    void ui_AModuleDelete();

    void clientJsonReceived(const QJsonObject&);
    void clientConnected();
    void clientConnectError(const QString&);
    void clientDisconnected();

}; // class MainWindow

#endif // MAINWINDOW_H
