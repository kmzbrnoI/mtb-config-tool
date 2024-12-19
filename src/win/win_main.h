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
#include "win_change_adress.h"
#include "win_diag.h"
#include "win_io.h"

constexpr unsigned MTBBUS_ADDR_COUNT = 256;
const QVector<QString> DAEMON_SUPPORTED_VERSIONS{"1.5", "1.6"};
QString daemonSupportedVersionsStr();

// Use #define instead of const so QObject::tr is applied in each use
#define TEXT_BEACON_OFF "-"
#define TEXT_BEACON_ON QObject::tr("YES")

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
    std::array<std::unique_ptr<MtbModuleIODialog>, MTBBUS_ADDR_COUNT> m_ioWindows;
    std::array<std::unique_ptr<DiagDialog>, MTBBUS_ADDR_COUNT> m_diagWindows;
    std::optional<DaemonVersion> m_daemonVersion;
    bool m_mtbUsbConnected = false;
    std::optional<MtbUsbStatus> m_mtbUsbStatus;

    SettingsWindow m_settingsWindow;
    MtbUsbWindow m_mtbUsbWindow;
    LogWindow m_logWindow;
    ModuleAddDialog m_moduleAddDialog;
    ChangeAddressDialog m_changeAddressDialog;

    QLabel m_sb_connection;
    QLabel m_sb_mtbusb;
    QLabel m_sb_error;
    QMenu twModulesContextMenu;

    struct {
        QAction* aConfigure;
        QAction* aIO;
        QAction* aReboot;
        QAction* aBeacon;
        QAction* aFwUpgrade;
        QAction* aDelete;
        QAction* aDiagnostics;
        QAction* aChangeAddr;
    } twModulesActions;

    void connectedUpdate();
    QString daemonHostPort() const;

    void connectingVersionReceived(const QJsonObject&);
    void connectingMtbUsbReceived(const QJsonObject&);
    void connectingModulesReceived(const QJsonObject&);

    void clientReceivedMtbUsb(const QJsonObject&);
    void clientReceivedModule(const QJsonObject&);
    void clientReceivedModules(const QJsonObject&);
    void clientReceivedModuleDeleted(const QJsonObject&);
    void clientReceivedModuleInputsChanged(const QJsonObject&);
    void clientReceivedModuleOutputsChanged(const QJsonObject&);

    void moduleDeleted(uint8_t addr);

    void moduleReceived(const QJsonObject&);
    void ui_updateModule(const QJsonObject&);
    void ui_updateAllModulesFromMModules();
    unsigned ui_twModulesInsertIndex(unsigned addr);
    void ui_twModulesClear();
    void ui_setupModulesContextMenu();
    void ui_fillModulesContextMenu();

    void closeEvent(QCloseEvent *event) override;
    static QJsonObject loadFwHex(const QString& filename);
    void fwUpgraded(const QJsonObject&);

    void checkModuleTypeChanged(const QJsonObject& module);

private slots:
    void ui_MAboutTriggered();
    void ui_AOptionsTriggered();
    void ui_AConnectTriggered();
    void ui_ADisconnectTriggered();
    void ui_AMtbUsbSettingsTriggered();
    void ui_AModulesRefreshTriggered();
    void ui_ADaemonConnectSettingsTriggered();
    void ui_ADaemonSaveConfigTriggered();
    void ui_ALogTriggered();
    void ui_twCustomContextMenu(const QPoint&);
    void ui_AClearErrorSb();

    void ui_twModulesSelectionChanged();
    void ui_twItemDblClicked(QTreeWidgetItem*, int column);
    void ui_AModuleConfigure();
    void ui_AModuleIO();
    void ui_AModuleReboot();
    void ui_AModuleBeacon();
    void ui_AModuleFwUpgrade();
    void ui_AModuleDiagnostics();
    void ui_AModuleAdd();
    void ui_AModuleDelete();
    void ui_AModuleChangeAddr();

    void clientJsonReceived(const QJsonObject&);
    void clientConnected();
    void clientConnectError(const QString&);
    void clientDisconnected();

}; // class MainWindow

#endif // MAINWINDOW_H
