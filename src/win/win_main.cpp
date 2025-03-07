#include <QMessageBox>
#include <QJsonArray>
#include <QFileDialog>
#include <QFileInfo>
#include "win_main.h"
#include "version.h"
#include "qjsonsafe.h"
#include "win_mtbuniconfig.h"
#include "win_mtbunisconfig.h"
#include "win_mtbuniio.h"
#include "win_mtbunisio.h"
#include "win_mtbrcio.h"

MainWindow* MainWindow::instance = nullptr;

MainWindow::MainWindow(Settings& s, QWidget *parent)
    : QMainWindow(parent), s(s), m_settingsWindow(s), m_mtbUsbWindow(this->m_mtbUsbStatus) {
    MainWindow::instance = this;
    ui.setupUi(this);

    this->m_sb_error.setStyleSheet("color: red; font-weight: bold;");
    this->ui.sb_main->addWidget(&this->m_sb_connection);
    this->ui.sb_main->addWidget(&this->m_sb_mtbusb);
    this->ui.sb_main->addWidget(&this->m_sb_error);

    this->connectedUpdate();
    this->ui_setupModulesContextMenu();

    QObject::connect(ui.tw_modules, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ui_twCustomContextMenu(const QPoint&)));
    QObject::connect(ui.tw_modules, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(ui_twItemDblClicked(QTreeWidgetItem*, int)));

    QObject::connect(ui.a_about, SIGNAL(triggered(bool)), this, SLOT(ui_MAboutTriggered()));
    QObject::connect(ui.a_options, SIGNAL(triggered(bool)), this, SLOT(ui_AOptionsTriggered()));
    QObject::connect(ui.a_connect, SIGNAL(triggered(bool)), this, SLOT(ui_AConnectTriggered()));
    QObject::connect(ui.a_disconnect, SIGNAL(triggered(bool)), this, SLOT(ui_ADisconnectTriggered()));
    QObject::connect(ui.a_mtb_daemon_save, SIGNAL(triggered(bool)), this, SLOT(ui_ADaemonSaveConfigTriggered()));
    QObject::connect(ui.a_mtbusb_settings, SIGNAL(triggered(bool)), this, SLOT(ui_AMtbUsbSettingsTriggered()));
    QObject::connect(ui.a_daemon_connection_settings, SIGNAL(triggered(bool)), this, SLOT(ui_ADaemonConnectSettingsTriggered()));
    QObject::connect(ui.a_modules_refresh, SIGNAL(triggered(bool)), this, SLOT(ui_AModulesRefreshTriggered()));
    QObject::connect(ui.a_log, SIGNAL(triggered(bool)), this, SLOT(ui_ALogTriggered()));

    QObject::connect(ui.tw_modules, SIGNAL(itemSelectionChanged()), this, SLOT(ui_twModulesSelectionChanged()));
    QObject::connect(ui.a_module_configure, SIGNAL(triggered(bool)), this, SLOT(ui_AModuleConfigure()));
    QObject::connect(ui.a_module_io, SIGNAL(triggered(bool)), this, SLOT(ui_AModuleIO()));
    QObject::connect(ui.a_module_reboot, SIGNAL(triggered(bool)), this, SLOT(ui_AModuleReboot()));
    QObject::connect(ui.a_module_fw_upgrade, SIGNAL(triggered(bool)), this, SLOT(ui_AModuleFwUpgrade()));
    QObject::connect(ui.a_module_beacon, SIGNAL(triggered(bool)), this, SLOT(ui_AModuleBeacon()));
    QObject::connect(ui.a_module_diagnostics, SIGNAL(triggered(bool)), this, SLOT(ui_AModuleDiagnostics()));
    QObject::connect(ui.a_module_add, SIGNAL(triggered(bool)), this, SLOT(ui_AModuleAdd()));
    QObject::connect(ui.a_module_delete, SIGNAL(triggered(bool)), this, SLOT(ui_AModuleDelete()));
    QObject::connect(ui.a_module_change_addr, SIGNAL(triggered(bool)), this, SLOT(ui_AModuleChangeAddr()));
    QObject::connect(ui.a_clear_error_sb, SIGNAL(triggered(bool)), this, SLOT(ui_AClearErrorSb()));

    QObject::connect(ui.a_modules_select_all, SIGNAL(triggered(bool)), this, SLOT(ui_AModulesSelectAll()));
    QObject::connect(ui.a_modules_unselect_all, SIGNAL(triggered(bool)), this, SLOT(ui_AModulesUnselectAll()));
    QObject::connect(ui.a_modules_select_type, SIGNAL(triggered(bool)), this, SLOT(ui_AModulesSelectType()));
    QObject::connect(ui.a_modules_fw_upgrade, SIGNAL(triggered(bool)), this, SLOT(ui_AModulesFwUpgrade()));
    QObject::connect(ui.a_modules_fw_upgrade_stop, SIGNAL(triggered(bool)), this, SLOT(ui_AModulesFwUpgradeStop()));

    QObject::connect(&m_client, SIGNAL(jsonReceived(const QJsonObject&)), this, SLOT(clientJsonReceived(const QJsonObject&)));
    QObject::connect(&m_client, SIGNAL(onConnected()), this, SLOT(clientConnected()));
    QObject::connect(&m_client, SIGNAL(onDisconnected()), this, SLOT(clientDisconnected()));
    QObject::connect(&m_client, SIGNAL(connectError(const QString&)), this, SLOT(clientConnectError(const QString&)));
}

void MainWindow::ui_setupModulesContextMenu() {
    this->twModulesActions.aConfigure = new QAction(this);
    connect(this->twModulesActions.aConfigure, SIGNAL(triggered()), this, SLOT(ui_AModuleConfigure()));
    this->twModulesContextMenu.addAction(this->twModulesActions.aConfigure);

    this->twModulesActions.aIO = new QAction(this);
    connect(this->twModulesActions.aIO, SIGNAL(triggered()), this, SLOT(ui_AModuleIO()));
    this->twModulesContextMenu.addAction(this->twModulesActions.aIO);

    this->twModulesActions.aReboot = new QAction(this);
    connect(this->twModulesActions.aReboot, SIGNAL(triggered()), this, SLOT(ui_AModuleReboot()));
    this->twModulesContextMenu.addAction(this->twModulesActions.aReboot);

    this->twModulesActions.aBeacon = new QAction(this);
    connect(this->twModulesActions.aBeacon, SIGNAL(triggered()), this, SLOT(ui_AModuleBeacon()));
    this->twModulesContextMenu.addAction(this->twModulesActions.aBeacon);

    this->twModulesActions.aFwUpgrade = new QAction(this);
    connect(this->twModulesActions.aFwUpgrade, SIGNAL(triggered()), this, SLOT(ui_AModuleFwUpgrade()));
    this->twModulesContextMenu.addAction(this->twModulesActions.aFwUpgrade);

    this->twModulesActions.aDelete = new QAction(this);
    connect(this->twModulesActions.aDelete, SIGNAL(triggered()), this, SLOT(ui_AModuleDelete()));
    this->twModulesContextMenu.addAction(this->twModulesActions.aDelete);

    this->twModulesActions.aDiagnostics = new QAction(this);
    connect(this->twModulesActions.aDiagnostics, SIGNAL(triggered()), this, SLOT(ui_AModuleDiagnostics()));
    this->twModulesContextMenu.addAction(this->twModulesActions.aDiagnostics);

    this->twModulesActions.aChangeAddr = new QAction(this);
    connect(this->twModulesActions.aChangeAddr, SIGNAL(triggered()), this, SLOT(ui_AModuleChangeAddr()));
    this->twModulesContextMenu.addAction(this->twModulesActions.aChangeAddr);

    this->ui_fillModulesContextMenu();

    this->ui.tw_modules->resizeColumnToContents(twChecked);
    this->ui.tw_modules->resizeColumnToContents(twAddrHex);
    this->ui.tw_modules->resizeColumnToContents(twAddrDec);
    this->ui.tw_modules->setColumnWidth(twName, 150);
    this->ui.tw_modules->resizeColumnToContents(twError);
    this->ui.tw_modules->resizeColumnToContents(twWarning);
    this->ui.tw_modules->resizeColumnToContents(twBeacon);
}

void MainWindow::ui_fillModulesContextMenu() {
    this->twModulesActions.aConfigure->setText(tr("Configure"));
    this->twModulesActions.aIO->setText(tr("Inputs/outputs"));
    this->twModulesActions.aReboot->setText(tr("Reboot"));
    this->twModulesActions.aBeacon->setText(tr("Beacon on/off"));
    this->twModulesActions.aFwUpgrade->setText(tr("Firmware upgrade"));
    this->twModulesActions.aDelete->setText(tr("Delete"));
    this->twModulesActions.aDiagnostics->setText(tr("Diagnostics"));
    this->twModulesActions.aChangeAddr->setText(tr("Change address"));
}

void MainWindow::ui_MAboutTriggered() {
    QMessageBox::information(
        this,
        tr("MTB Configuration Tool"),
        QString(tr("MTB Configuration Tool")+
                "\nv%1\n"+
                tr("Created by Jan Horáček 2024")).arg(MTB_CONFIG_TOOL_VERSION),
        QMessageBox::Ok
    );
}

void MainWindow::ui_AOptionsTriggered() {
    this->m_settingsWindow.open();
}

void MainWindow::ui_ADaemonConnectSettingsTriggered() {
    this->m_settingsWindow.open();
}

void MainWindow::retranslate() {
    this->ui.retranslateUi(this);
    this->m_settingsWindow.retranslate();
    this->m_mtbUsbWindow.retranslate();
    this->m_logWindow.retranslate();
    this->m_moduleAddDialog.retranslate();
    this->m_changeAddressDialog.retranslate();
    for (auto& windowPtr : this->m_configWindows)
        if (windowPtr)
            windowPtr->retranslate();
    for (auto& windowPtr : this->m_ioWindows)
        if (windowPtr)
            windowPtr->retranslate();
    for (auto& windowPtr : this->m_diagWindows)
        if (windowPtr)
            windowPtr->retranslate();

    this->ui_fillModulesContextMenu();
    this->ui_updateAllModulesFromMModules();
    this->setWindowTitle(QString(tr("MTB Configuration Tool")+" v%1.%2").\
                         arg(MTB_CONFIG_TOOL_VERSION_MAJOR).arg(MTB_CONFIG_TOOL_VERSION_MINOR));
}

void MainWindow::clientJsonReceived(const QJsonObject& json) {
    try {
        if (json["command"] == "mtbusb")
            this->clientReceivedMtbUsb(QJsonSafe::safeObject(json["mtbusb"]));
        else if (json["command"] == "module")
            this->clientReceivedModule(QJsonSafe::safeObject(json["module"]));
        else if (json["command"] == "modules")
            this->clientReceivedModules(QJsonSafe::safeObject(json["modules"]));
        else if (json["command"] == "module_deleted")
            this->clientReceivedModuleDeleted(QJsonSafe::safeObject(json));
        else if (json["command"] == "module_inputs_changed")
            this->clientReceivedModuleInputsChanged(QJsonSafe::safeObject(json["module_inputs_changed"]));
        else if (json["command"] == "module_outputs_changed")
            this->clientReceivedModuleOutputsChanged(QJsonSafe::safeObject(json["module_outputs_changed"]));
    } catch (const QStrException& e) {
        log("MainWindow::clientJsonReceived exception: "+e.str(), LogLevel::Error);
    } catch (...) {
        log("MainWindow::clientJsonReceived unknown exception!", LogLevel::Error);
    }
}

void MainWindow::clientConnected() {
    this->connectedUpdate();

    this->m_client.sendNoExc(
        {{"command", "version"}},
        [this](const QJsonObject& content) {
            this->connectingVersionReceived(content);
        },
        [this](unsigned errorCode, QString errorMessage) {
            QApplication::restoreOverrideCursor();
            this->ui_ADisconnectTriggered();
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("version", errorCode, errorMessage)+"\n"+
                                 tr("Are you using MTB Daemon < v1.5? Upgrade!")+"\n"+tr("Closing connection..."));
        }
    );
}

void MainWindow::connectingVersionReceived(const QJsonObject& json) {
    try {
        const QJsonObject& jsonVersion = QJsonSafe::safeObject(json["version"]);
        const QString versionStr = QJsonSafe::safeString(jsonVersion["sw_version"]);
        this->m_daemonVersion.emplace(QJsonSafe::safeUInt(jsonVersion["sw_version_major"]), QJsonSafe::safeUInt(jsonVersion["sw_version_minor"]));
        this->m_sb_connection.setText(this->m_sb_connection.text() + " v"+this->m_daemonVersion->str());

        if (!DAEMON_SUPPORTED_VERSIONS.contains(versionStr)) {
            QApplication::restoreOverrideCursor();
            QMessageBox::StandardButton reply = QMessageBox::question(this, "?", tr("Unsupported MTB Daemon received version: ")+versionStr+"\n"+
                tr("Supported versions: ")+daemonSupportedVersionsStr()+"\n"+tr("Continue?"));
            if (reply == QMessageBox::No) {
                this->ui_ADisconnectTriggered();
                return;
            }
        }
    } catch (const QStrException& e) {
        QApplication::restoreOverrideCursor();
        this->ui_ADisconnectTriggered();
        QMessageBox::warning(this, tr("Error"), tr("Invalid received MTB Daemon version!")+"\n"+e.str()+"\n"+tr("Closing connection..."));
        return;
    }

    // version ok -> ask mtbusb status
    this->m_client.sendNoExc(
        {{"command", "mtbusb"}},
        [this](const QJsonObject& content) {
            this->connectingMtbUsbReceived(content);
        },
        [this](unsigned errorCode, QString errorMessage) {
            QApplication::restoreOverrideCursor();
            this->ui_ADisconnectTriggered();
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("mtbusb", errorCode, errorMessage)+"\n"+tr("Closing connection..."));
        }
    );
}

void MainWindow::connectingMtbUsbReceived(const QJsonObject&) {
    // mtbusb received -> ask modules
    this->m_client.sendNoExc(
        {{"command", "modules"}},
        [this](const QJsonObject& content) {
            this->connectingModulesReceived(content);
        },
        [this](unsigned errorCode, QString errorMessage) {
            this->ui_ADisconnectTriggered();
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("modules", errorCode, errorMessage)+"\n"+tr("Closing connection..."));
        }
    );
}

void MainWindow::connectingModulesReceived(const QJsonObject&) {
    // modules received -> subscribe events
    // This is not an ideal solution as we don't care about state of inputs,
    // however we need to detect changes of warnings/errors/beacon etc.
    // MTB Daemon currently supports no method to register only the events we care about
    this->m_client.sendNoExc(
        {{"command", "module_subscribe"}},
        [this](const QJsonObject& content) {
            // Connecting finished
            (void)content;
            this->ui.tw_modules->setEnabled(true);
            QApplication::restoreOverrideCursor();
        },
        [this](unsigned errorCode, QString errorMessage) {
            this->ui_ADisconnectTriggered();
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("module_subscribe", errorCode, errorMessage)+"\n"+tr("Closing connection..."));
        }
    );

}

void MainWindow::clientConnectError(const QString& msg) {
    QApplication::restoreOverrideCursor();
    QMessageBox::warning(this, tr("Error!"), tr("Unable to connect to ")+this->daemonHostPort()+".\n"+msg);
}

void MainWindow::clientDisconnected() {
    this->connectedUpdate();
    QApplication::restoreOverrideCursor();
}

void MainWindow::ui_AConnectTriggered() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    this->ui.a_connect->setEnabled(false);
    this->ui.a_disconnect->setEnabled(false);
    this->m_sb_connection.setText(tr("Connecting to MTB Daemon ")+this->daemonHostPort()+" ...");

    try {
        this->m_client.connect(s["mtb-daemon"]["host"].toString(), s["mtb-daemon"]["port"].toInt());
    } catch (const QStrException& e) {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, e.str(), tr("Error!"));
        this->connectedUpdate();
    } catch (...) {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, tr("Unknown exception!"), tr("Error!"));
        this->connectedUpdate();
    }
}

void MainWindow::ui_ADisconnectTriggered() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    this->ui.a_connect->setEnabled(false);
    this->ui.a_disconnect->setEnabled(false);
    this->m_sb_connection.setText(tr("Disconnecting from MTB Daemon..."));

    try {
        this->m_client.disconnect();
    } catch (const QStrException& e) {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, e.str(), tr("Error!"));
        this->connectedUpdate();
    } catch (...) {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, tr("Unknown exception!"), tr("Error!"));
        this->connectedUpdate();
    }
}

void MainWindow::connectedUpdate() {
    this->ui.a_connect->setEnabled(!this->m_client.connected());
    this->ui.a_disconnect->setEnabled(this->m_client.connected());
    this->ui.a_modules_refresh->setEnabled(this->m_client.connected());
    this->ui.a_mtb_daemon_save->setEnabled(this->m_client.connected());
    this->ui.a_module_add->setEnabled(this->m_client.connected());
    this->ui.a_module_change_addr->setEnabled(this->m_client.connected());

    this->ui.a_modules_select_all->setEnabled(this->m_client.connected());
    this->ui.a_modules_unselect_all->setEnabled(this->m_client.connected());
    this->ui.a_modules_select_type->setEnabled(this->m_client.connected());
    this->ui.a_modules_fw_upgrade->setEnabled(this->m_client.connected());
    if ((!this->m_client.connected()) || (!this->m_bulkFwUpgrading))
        this->ui.a_modules_fw_upgrade_stop->setEnabled(false);

    this->m_sb_connection.setText((this->m_client.connected()) ? tr("Connected to MTB Daemon ")+this->daemonHostPort(): tr("Disconnected from MTB Daemon"));

    if (!this->m_client.connected()) {
        this->m_bulkFwUpgrading = false;
        this->m_sb_mtbusb.setText("---");
        this->ui.tw_modules->clear();
        this->ui.tw_modules->setEnabled(false);
        this->m_mtbUsbWindow.close();
        this->m_moduleAddDialog.close();
        this->m_changeAddressDialog.close();

        this->m_daemonVersion.reset();
        this->m_mtbUsbStatus.reset();
        this->m_mtbUsbConnected = false;

        for (auto& module : this->m_modules)
            module = {}; // clear everything
        for (auto& window : this->m_configWindows)
            window.reset();
        for (auto& window : this->m_ioWindows)
            window.reset();
        for (auto& window : this->m_diagWindows)
            window.reset();
    }

    this->ui.a_mtbusb_settings->setEnabled(this->m_mtbUsbConnected);
}

QString MainWindow::daemonHostPort() const {
    return s["mtb-daemon"]["host"].toString() + ":" + s["mtb-daemon"]["port"].toString();
}

void MainWindow::ui_updateModule(const QJsonObject& module) {
    if (!module.contains("address")) {
        log("MainWindow::ui_updateModule json does not contain 'address'", LogLevel::Error);
        return;
    }

    const uint8_t address = module["address"].toInt(0);
    if (address == 0)
        return;

    if (m_tw_lines[address] == nullptr) {
        auto newItem = new QTreeWidgetItem();
        newItem->setCheckState(0, Qt::CheckState::Unchecked);
        this->ui.tw_modules->insertTopLevelItem(this->ui_twModulesInsertIndex(address), newItem);
        m_tw_lines[address] = newItem;
    }
    QTreeWidgetItem* item = m_tw_lines[address];

    item->setText(TwModulesColumns::twAddrDec, QString::number(address));
    item->setText(TwModulesColumns::twAddrHex, "0x"+(QString::number(address, 16).rightJustified(2, '0')));
    item->setText(TwModulesColumns::twAddrBin, "0b"+(QString::number(address, 2).rightJustified(8, '0')));
    item->setText(TwModulesColumns::twName, module["name"].toString());
    item->setText(TwModulesColumns::twType, module["type"].toString());
    item->setText(TwModulesColumns::twState, module["state"].toString());
    const QString deprecated = module["fw_deprecated"].toBool() ? tr(" (deprecated)") : "";
    item->setText(TwModulesColumns::twFw, module.contains("firmware_version") ? module["firmware_version"].toString()+deprecated : "N/A");
    item->setText(TwModulesColumns::twBootloader, module.contains("bootloader_version") ? module["bootloader_version"].toString() : "N/A");
    item->setText(TwModulesColumns::twError, module.contains("error") ? (module["error"].toBool() ? tr("ERROR") : "-") : "N/A");
    item->setText(TwModulesColumns::twWarning, module.contains("warning") ? (module["warning"].toBool() ? tr("WARN") : "-") : "N/A");
    item->setText(TwModulesColumns::twBeacon, module.contains("beacon") ? (module["beacon"].toBool() ? TEXT_BEACON_ON : TEXT_BEACON_OFF) : "N/A");

    const QString& state = module["state"].toString();

    if (module["error"].toBool())
        setBackground(*item, QC_LIGHT_RED);
    else if ((module["warning"].toBool()) || ((state != "active") && (state != "inactive")))
        setBackground(*item, QC_LIGHT_YELLOW);
    else if (module["beacon"].toBool())
        setBackground(*item, QC_LIGHT_BLUE);
    else if (state == "inactive")
        setBackground(*item, QC_LIGHT_GRAY);
    else
        setBackground(*item, QC_LIGHT_GREEN);
}

void MainWindow::ui_updateAllModulesFromMModules() {
    for (const QJsonObject& module : this->m_modules)
        if (module.contains("address"))
            this->ui_updateModule(module);
}

unsigned MainWindow::ui_twModulesInsertIndex(unsigned addr) {
    while ((addr < MTBBUS_ADDR_COUNT) && (this->m_tw_lines[addr] == nullptr))
        addr++;
    return (addr == MTBBUS_ADDR_COUNT) ? this->ui.tw_modules->topLevelItemCount() : this->ui.tw_modules->indexOfTopLevelItem(this->m_tw_lines[addr]);
}

void MainWindow::clientReceivedMtbUsb(const QJsonObject& json) {
    this->m_mtbUsbConnected = json["connected"].toBool();
    if (this->m_mtbUsbConnected)
        this->m_mtbUsbStatus.emplace(json);
    else
        this->m_mtbUsbStatus.reset();

    if (this->m_mtbUsbStatus) {
        const MtbUsbStatus& status = this->m_mtbUsbStatus.value();
        this->m_sb_mtbusb.setText(
            tr("MTB-USB connected: type: ")+QString::number(status.type)+", "+
            tr("MtbBus speed: ")+QString::number(status.speed)+" bdps, "+
            tr("FW: v")+status.firmware_version+", "+
            tr("protocol: v")+status.protocol_version
        );
    } else {
        this->m_sb_mtbusb.setText(tr("No MTB-USB connected"));
    }

    this->connectedUpdate();
}

void MainWindow::clientReceivedModule(const QJsonObject& json) {
    if (!json.contains("address")) {
        log("MainWindow::clientReceivedModule json does not contain 'address'", LogLevel::Error);
        return;
    }

    this->moduleReceived(json);
}

void MainWindow::clientReceivedModules(const QJsonObject& modules) {
    this->ui_twModulesClear();
    for (const QString& addrStr : modules.keys())
        this->moduleReceived(QJsonSafe::safeObject(modules[addrStr]));
}

void MainWindow::moduleReceived(const QJsonObject& module) {
    unsigned address = QJsonSafe::safeUInt(module["address"]);
    if (address == 0)
        return;

    this->m_modules[address] = module;
    this->ui_updateModule(module);
    this->checkModuleTypeChanged(module);
    if ((this->m_ioWindows[address]) && (this->m_ioWindows[address]->isVisible()))
        this->m_ioWindows[address]->moduleChanged(module);
}

void MainWindow::ui_AModulesRefreshTriggered() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    this->ui_twModulesClear();

    this->m_client.sendNoExc(
        {{"command", "modules"}},
        [this](const QJsonObject& content) {
            (void)content;
            QApplication::restoreOverrideCursor();
            QMessageBox::information(this, tr("Info"), tr("List of modules updated."));
        },
        [this](unsigned errorCode, QString errorMessage) {
            QApplication::restoreOverrideCursor();
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("modules", errorCode, errorMessage));
        }
    );
}

void MainWindow::ui_AMtbUsbSettingsTriggered() {
    this->m_mtbUsbWindow.open();
}

void MainWindow::ui_twModulesClear() {
    this->ui.tw_modules->clear();
    for (auto& ref : this->m_tw_lines)
        ref = nullptr;
}

void MainWindow::ui_ALogTriggered() {
    this->m_logWindow.show();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    this->m_logWindow.close();
    this->m_mtbUsbWindow.close();
    for (auto& window : this->m_configWindows)
        window.reset();
    for (auto& window : this->m_ioWindows)
        window.reset();
    for (auto& window : this->m_diagWindows)
        window.reset();
    QMainWindow::closeEvent(event);
}

QString daemonSupportedVersionsStr() {
    if (DAEMON_SUPPORTED_VERSIONS.empty())
        return "";
    QString result = "";
    for (unsigned i = 0; i < DAEMON_SUPPORTED_VERSIONS.count()-1; i++)
        result += DAEMON_SUPPORTED_VERSIONS[i] + ", ";
    return result + DAEMON_SUPPORTED_VERSIONS[DAEMON_SUPPORTED_VERSIONS.count()-1];
}

void MainWindow::ui_ADaemonSaveConfigTriggered() {
    QApplication::setOverrideCursor(Qt::WaitCursor);

    this->m_client.sendNoExc(
        {{"command", "save_config"}},
        [this](const QJsonObject& content) {
            (void)content;
            QApplication::restoreOverrideCursor();
            QMessageBox::information(this, tr("OK"), tr("MTB Daemon's configuration saved to the server's configuration file."));
        },
        [this](unsigned errorCode, QString errorMessage) {
            QApplication::restoreOverrideCursor();
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("save_config", errorCode, errorMessage));
        }
    );
}

void MainWindow::ui_twCustomContextMenu(const QPoint& pos) {
    QTreeWidgetItem *nd = this->ui.tw_modules->itemAt(pos);
    this->twModulesContextMenu.setEnabled(nd != nullptr);
    this->twModulesContextMenu.exec(ui.tw_modules->mapToGlobal(pos));
}

void MainWindow::ui_twModulesSelectionChanged() {
    const bool selected = (this->ui.tw_modules->currentItem() != nullptr);
    this->ui.a_module_configure->setEnabled(selected);
    this->ui.a_module_io->setEnabled(selected);
    this->ui.a_module_reboot->setEnabled(selected);
    this->ui.a_module_beacon->setEnabled(selected);
    this->ui.a_module_fw_upgrade->setEnabled(selected);
    this->ui.a_module_delete->setEnabled(selected);
    this->ui.a_module_diagnostics->setEnabled(selected);
}

void MainWindow::ui_twItemDblClicked(QTreeWidgetItem* item, int column) {
    (void)column;
    (void)item;
    if (this->ui.tw_modules->currentItem() != nullptr)
        this->ui_AModuleConfigure();
}

void MainWindow::ui_AModuleConfigure() {
    try {
        const QTreeWidgetItem* currentLine = this->ui.tw_modules->currentItem();
        if (currentLine == nullptr)
            throw QStrException("currentLine == nullptr");
        unsigned addr = currentLine->text(TwModulesColumns::twAddrDec).toInt();
        const unsigned typeCode = QJsonSafe::safeUInt(this->m_modules[addr]["type_code"]);
        const MtbModuleType type = static_cast<MtbModuleType>(typeCode);

        if ((typeCode&0xF0) == 0x10) { // MTB-UNI and variants
            if (!this->m_configWindows[addr])
                this->m_configWindows[addr] = std::make_unique<MtbUniConfigWindow>();
        } else if (type == MtbModuleType::Unis10) {
            if (!this->m_configWindows[addr])
                this->m_configWindows[addr] = std::make_unique<MtbUnisConfigWindow>();
        } else if (type == MtbModuleType::Rc) {
            QMessageBox::warning(this, tr("No config"), tr("This module has no configuration."));
            return;
        } else {
            QMessageBox::warning(this, tr("Unknown module type"), tr("Unknown module type code: ")+QString::number(typeCode)+tr(", no configuration window available!"));
            return;
        }

        // Reopen already active window -> refill up-to-date values
        this->m_configWindows[addr]->editModule(this->m_modules[addr]);
    } catch (const QStrException& e) {
        QMessageBox::critical(this, "Error", "Cannot edit module: "+e.str());
    }
}

void MainWindow::ui_AModuleIO() {
    try {
        const QTreeWidgetItem* currentLine = this->ui.tw_modules->currentItem();
        if (currentLine == nullptr)
            throw QStrException("currentLine == nullptr");
        unsigned addr = currentLine->text(TwModulesColumns::twAddrDec).toInt();
        const unsigned typeCode = QJsonSafe::safeUInt(this->m_modules[addr]["type_code"]);
        const MtbModuleType type = static_cast<MtbModuleType>(typeCode);

        if ((typeCode&0xF0) == 0x10) { // MTB-UNI and variants
            if (!this->m_ioWindows[addr])
                this->m_ioWindows[addr] = std::make_unique<MtbUniIOWindow>();
        } else if (type == MtbModuleType::Unis10) {
            if (!this->m_ioWindows[addr])
                this->m_ioWindows[addr] = std::make_unique<MtbUnisIOWindow>();
        } else if (type == MtbModuleType::Rc) {
            if (!this->m_ioWindows[addr])
                this->m_ioWindows[addr] = std::make_unique<MtbRCIOWindow>();
        } else {
            QMessageBox::warning(this, tr("Unknown module type"), tr("Unknown module type code: ")+QString::number(typeCode)+tr(", no IO window available!"));
            return;
        }

        // Reopen already active window -> refill up-to-date values
        this->m_ioWindows[addr]->openModule(this->m_modules[addr]);
    } catch (const QStrException& e) {
        QMessageBox::critical(this, "Error", "Cannot open IO window: "+e.str());
    }
}

void MainWindow::ui_AModuleReboot() {
    const QTreeWidgetItem* currentLine = this->ui.tw_modules->currentItem();
    if (currentLine == nullptr)
        return;
    unsigned addr = currentLine->text(TwModulesColumns::twAddrDec).toInt();

    QMessageBox::StandardButton reply = QMessageBox::question(this, "?", tr("Really reboot module ")+QString::number(addr)+"?");
    if (reply == QMessageBox::StandardButton::No)
        return;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    this->m_client.sendNoExc(
        {{"command", "module_reboot"}, {"address", static_cast<int>(addr)}},
        [this](const QJsonObject& content) {
            (void)content;
            QApplication::restoreOverrideCursor();
            QMessageBox::information(this, tr("Finished"), tr("Module successfully rebooted"));
        },
        [this](unsigned errorCode, QString errorMessage) {
            QApplication::restoreOverrideCursor();
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("module_reboot", errorCode, errorMessage));
        }
    );
}

void MainWindow::ui_AModuleBeacon() {
    QTreeWidgetItem* currentLine = this->ui.tw_modules->currentItem();
    if (currentLine == nullptr)
        return;
    const unsigned addr = currentLine->text(TwModulesColumns::twAddrDec).toInt();
    const bool beaconOn = (currentLine->text(TwModulesColumns::twBeacon) == TEXT_BEACON_ON);

    this->m_client.sendNoExc(
        {{"command", "module_beacon"}, {"address", static_cast<int>(addr)}, {"beacon", !beaconOn}},
        [this, addr](const QJsonObject& content) {
            this->m_modules[addr]["beacon"] = content["beacon"];
            this->ui_updateModule(this->m_modules[addr]);
        },
        [this](unsigned errorCode, QString errorMessage) {
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("module_beacon", errorCode, errorMessage));
        }
    );
}

void MainWindow::ui_AModuleFwUpgrade() {
    QTreeWidgetItem* currentLine = this->ui.tw_modules->currentItem();
    if (currentLine == nullptr)
        return;
    const unsigned addr = currentLine->text(TwModulesColumns::twAddrDec).toInt();

    QString filename = QFileDialog::getOpenFileName(
        this,
        tr("Choose firmware hex file"),
        s["paths"]["fw"].toString(),
        tr("Hex file (*.hex)")
    );

    if (filename == "")
        return;

    QFileInfo fileInfo(filename);
    s["paths"]["fw"] = fileInfo.dir().absolutePath();
    s.save(CONFIG_FILENAME);

    QJsonObject firmware;
    try {
        firmware = MainWindow::loadFwHex(filename);
    } catch (const QStrException& e) {
        QMessageBox::warning(this, tr("Error"), tr("Unable to load hex file")+"\n"+e.str());
        return;
    } catch (...) {
        QMessageBox::warning(this, tr("Error"), tr("Unable to load hex file - general exception"));
        return;
    }

    this->m_modules[addr]["state"] = "fw_upgrading";
    this->ui_updateModule(this->m_modules[addr]);

    this->m_client.sendNoExc(
        {{"command", "module_upgrade_fw"}, {"address", static_cast<int>(addr)}, {"firmware", firmware}},
        [this](const QJsonObject& content) {
            this->fwUpgraded(content);
        },
        [this, addr](unsigned errorCode, QString errorMessage) {
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("module_upgrade_fw", errorCode, errorMessage));
            this->m_client.sendNoExc( // refresh module state
                {{"command", "module"}, {"address", static_cast<int>(addr)}},
                [](const QJsonObject&) {},
                [](unsigned, QString) {}
            );
        }
    );
}

void MainWindow::fwUpgraded(const QJsonObject& json) {
    uint8_t addr = json["address"].toInt(0);
    if (addr == 0)
        return;

    // firmware upgraded -> ask new module state
    this->m_client.sendNoExc(
        {{"command", "module"}, {"address", addr}},
        [](const QJsonObject& content) {
            (void)content;
        },
        [this](unsigned errorCode, QString errorMessage) {
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("module", errorCode, errorMessage));
        }
    );
}

void MainWindow::ui_AModuleDiagnostics() {
    try {
        const QTreeWidgetItem* currentLine = this->ui.tw_modules->currentItem();
        if (currentLine == nullptr)
            throw QStrException("currentLine == nullptr");
        unsigned addr = currentLine->text(TwModulesColumns::twAddrDec).toInt();

        if (!this->m_diagWindows[addr])
            this->m_diagWindows[addr] = std::make_unique<DiagDialog>();
        this->m_diagWindows[addr]->moduleOpen(this->m_modules[addr]);
    } catch (const QStrException& e) {
        QMessageBox::critical(this, "Error", "Cannot open diag window: "+e.str());
    }
}

QJsonObject MainWindow::loadFwHex(const QString& filename) {
    QJsonObject firmware;
    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text))
        throw EHexParseError(tr("Cannot read file ")+filename);

    bool ok = true;
    unsigned offset = 0;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().simplified();
        if (!line.startsWith(':'))
            throw EHexParseError(tr("Some line in input file does not start with ':'"));

        unsigned type = line.mid(7, 2).toInt(&ok, 16);
        if (!ok)
            throw EHexParseError(tr("Type !ok"));

        unsigned addr = offset + line.mid(3, 4).toInt(&ok, 16);
        if (!ok)
            throw EHexParseError(tr("Addr !ok"));

        if (type == 2) {
            offset = line.mid(9, 4).toInt(&ok, 16)*16;
            if (!ok)
                throw EHexParseError(tr("Offset !ok"));
        } else if (type == 0) {
            firmware[QString::number(addr)] = line.mid(9, line.length()-11);
        }
    }

    return firmware;
}

void MainWindow::checkModuleTypeChanged(const QJsonObject& module) {
    const uint8_t address = QJsonSafe::safeUInt(module["address"]);
    const unsigned typeCode = QJsonSafe::safeUInt(module["type_code"]);
    const MtbModuleType type = static_cast<MtbModuleType>(typeCode);

    bool changed = false;
    if (this->m_configWindows[address]) {
        if (((typeCode&0xF0) == 0x10) && (!is<MtbUniConfigWindow>(*this->m_configWindows[address]))) // MTB-UNI and variants
            changed = true;
        if ((type == MtbModuleType::Unis10) && (!is<MtbUnisConfigWindow>(*this->m_configWindows[address]))) // MTB-UNIS
            changed = true;
    }

    if (changed) {
        bool visible = this->m_configWindows[address]->isVisible() || this->m_ioWindows[address]->isVisible();
        this->m_configWindows[address].reset();
        this->m_ioWindows[address].reset();
        if (visible)
            QMessageBox::warning(this, tr("Warning"), tr("Type of module ")+QString::number(address)+tr(" changed, module window closed."));
    }

    // TODO: this->m_diagWindows[address].reset();
}

void MainWindow::ui_AClearErrorSb() {
    this->m_sb_error.setText("");
    this->ui.a_clear_error_sb->setEnabled(false);
}

void MainWindow::criticalError(const QString& err) {
    this->m_sb_error.setText(QTime::currentTime().toString("hh:mm:ss") + " " + err);
    this->ui.a_clear_error_sb->setEnabled(true);
}

void MainWindow::clientReceivedModuleDeleted(const QJsonObject &json) {
    uint8_t addr = QJsonSafe::safeUInt(json["module"]);
    if (addr != 0)
        this->moduleDeleted(addr);
}

void MainWindow::moduleDeleted(uint8_t addr) {
    this->m_modules[addr] = {};
    if (this->m_tw_lines[addr] != nullptr) {
        int i = this->ui.tw_modules->indexOfTopLevelItem(this->m_tw_lines[addr]);
        if (i >= 0)
            this->ui.tw_modules->takeTopLevelItem(i);
        this->m_tw_lines[addr] = nullptr;
    }

    if (((this->m_configWindows[addr]) && (this->m_configWindows[addr]->isVisible())) ||
        ((this->m_ioWindows[addr]) && (this->m_ioWindows[addr]->isVisible())) ||
        ((this->m_diagWindows[addr]) && (this->m_diagWindows[addr]->isVisible()))) {
        QMessageBox::warning(this, tr("Warning"), tr("Module with open window was deleted on the server!\nModule ")+QString::number(addr));
    }
}

void MainWindow::clientReceivedModuleInputsChanged(const QJsonObject &json) {
    uint8_t addr = QJsonSafe::safeUInt(json["address"]);
    if ((this->m_ioWindows[addr]) && (this->m_ioWindows[addr]->isVisible()))
        this->m_ioWindows[addr]->inputsChanged(json);
}

void MainWindow::clientReceivedModuleOutputsChanged(const QJsonObject &json) {
    uint8_t addr = QJsonSafe::safeUInt(json["address"]);
    if ((this->m_ioWindows[addr]) && (this->m_ioWindows[addr]->isVisible()))
        this->m_ioWindows[addr]->outputsChanged(json);
}

void MainWindow::ui_AModuleAdd() {
    this->m_moduleAddDialog.add();
}

void MainWindow::ui_AModuleDelete() {
    const QTreeWidgetItem* currentLine = this->ui.tw_modules->currentItem();
    if (currentLine == nullptr)
        return;
    unsigned addr = currentLine->text(TwModulesColumns::twAddrDec).toInt();

    QMessageBox::StandardButton reply = QMessageBox::question(this, "?", tr("Really delete module ")+QString::number(addr)+"?");
    if (reply == QMessageBox::StandardButton::No)
        return;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    this->m_client.sendNoExc(
        {{"command", "module_delete"}, {"address", static_cast<int>(addr)}},
        [addr, this](const QJsonObject& content) {
            (void)content;
            QApplication::restoreOverrideCursor();
            this->moduleDeleted(addr);
            QMessageBox::information(this, tr("Finished"), tr("Module successfully deleted."));
        },
        [this](unsigned errorCode, QString errorMessage) {
            QApplication::restoreOverrideCursor();
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("module_delete", errorCode, errorMessage));
        }
    );
}

void MainWindow::ui_AModuleChangeAddr() {
    const QTreeWidgetItem* currentLine = this->ui.tw_modules->currentItem();
    if (currentLine != nullptr) {
        unsigned addr = currentLine->text(TwModulesColumns::twAddrDec).toInt();
        this->m_changeAddressDialog.openFromModule(addr);
    } else {
        this->m_changeAddressDialog.openGeneral();
    }
}

void MainWindow::ui_AModulesSelectAll() {
    for (int i = 0; i < this->ui.tw_modules->topLevelItemCount(); i++)
        this->ui.tw_modules->topLevelItem(i)->setCheckState(twChecked, Qt::CheckState::Checked);
}

void MainWindow::ui_AModulesUnselectAll() {
    for (int i = 0; i < this->ui.tw_modules->topLevelItemCount(); i++)
        this->ui.tw_modules->topLevelItem(i)->setCheckState(twChecked, Qt::CheckState::Unchecked);
}

void MainWindow::ui_AModulesSelectType() {
    QTreeWidgetItem* selected = this->ui.tw_modules->currentItem();
    if (selected == nullptr) {
        QMessageBox::information(this, tr("Information"), tr("No module selected!"), QMessageBox::Ok);
        return;
    }
    const QString selectedType = selected->text(twType);

    for (int i = 0; i < this->ui.tw_modules->topLevelItemCount(); i++) {
        QTreeWidgetItem* current = this->ui.tw_modules->topLevelItem(i);
        Qt::CheckState checked = (current->text(twType) == selectedType) ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
        current->setCheckState(twChecked, checked);
    }
}

void MainWindow::ui_AModulesFwUpgrade() {
    if (this->twModulesFirstChecked() == nullptr) {
        QMessageBox::warning(this, tr("Warning"), tr("No module selected!"), QMessageBox::Ok);
        return;
    }

    QString filename = QFileDialog::getOpenFileName(
        this,
        tr("Choose firmware hex file"),
        s["paths"]["fw"].toString(),
        tr("Hex file (*.hex)")
    );

    if (filename == "")
        return;

    QFileInfo fileInfo(filename);
    s["paths"]["fw"] = fileInfo.dir().absolutePath();
    s.save(CONFIG_FILENAME);

    this->m_bulkFirmware = {};
    try {
        this->m_bulkFirmware = MainWindow::loadFwHex(filename);
    } catch (const QStrException& e) {
        QMessageBox::warning(this, tr("Error"), tr("Unable to load hex file")+"\n"+e.str());
        return;
    } catch (...) {
        QMessageBox::warning(this, tr("Error"), tr("Unable to load hex file - general exception"));
        return;
    }

    this->m_bulkFwUpgrading = true;
    this->ui.a_modules_fw_upgrade->setEnabled(false);
    this->ui.a_modules_fw_upgrade_stop->setEnabled(true);
    this->fwUpgradeNextSelectedModule();
}

void MainWindow::ui_AModulesFwUpgradeStop() {
    this->ui_AModulesUnselectAll();
}

void MainWindow::fwUpgradeNextSelectedModule() {
    QTreeWidgetItem* toUpgrade = this->twModulesFirstChecked();
    if (toUpgrade == nullptr) {
        this->fwUpgradeBulkFinished();
        QMessageBox::information(this, tr("Information"), tr("Bulk upgrading of firmwares finished!"), QMessageBox::Ok);
        return;
    }
    if (!this->m_bulkFwUpgrading) {
        this->fwUpgradeBulkFinished();
        QMessageBox::warning(this, tr("Information"), tr("Bulk upgrading of firmwares interrupted!"), QMessageBox::Ok);
        return;
    }

    toUpgrade->setCheckState(twChecked, Qt::CheckState::Unchecked);
    const unsigned addr = toUpgrade->text(TwModulesColumns::twAddrDec).toInt();

    this->m_modules[addr]["state"] = "fw_upgrading";
    this->ui_updateModule(this->m_modules[addr]);

    this->m_client.sendNoExc(
        {{"command", "module_upgrade_fw"}, {"address", static_cast<int>(addr)}, {"firmware", this->m_bulkFirmware}},
        [addr, this](const QJsonObject& content) {
            this->fwUpgradedBulk(addr, content);
        },
        [this, addr](unsigned errorCode, QString errorMessage) {
            this->fwUpgradeBulkStopError(addr, DaemonClient::standardErrrorMessage("module_upgrade_fw", errorCode, errorMessage));
            this->m_client.sendNoExc( // refresh module state
                {{"command", "module"}, {"address", static_cast<int>(addr)}},
                [](const QJsonObject&) {},
                [](unsigned, QString) {}
            );
        }
    );
}

void MainWindow::fwUpgradedBulk(unsigned addr, const QJsonObject& json) {
    // firmware upgraded -> ask new module state
    (void)json;

    this->m_client.sendNoExc(
        {{"command", "module"}, {"address", static_cast<int>(addr)}},
        [this](const QJsonObject& content) {
            (void)content;
            this->fwUpgradeNextSelectedModule();
        },
        [this, addr](unsigned errorCode, QString errorMessage) {
            this->fwUpgradeBulkStopError(addr, DaemonClient::standardErrrorMessage("module", errorCode, errorMessage));
        }
    );
}

void MainWindow::fwUpgradeBulkFinished() {
    this->m_bulkFwUpgrading = false;
    this->m_bulkFirmware = {};
    this->ui.a_modules_fw_upgrade->setEnabled(true);
    this->ui.a_modules_fw_upgrade_stop->setEnabled(false);
}

void MainWindow::fwUpgradeBulkStopError(unsigned addr, const QString& error) {
    this->fwUpgradeBulkFinished();
    QMessageBox::warning(this, tr("Error"), tr("Firmware upgrade of module ") + QString::number(addr) + ":\n" + error);
}

QTreeWidgetItem* MainWindow::twModulesFirstChecked() const {
    for (int i = 0; i < this->ui.tw_modules->topLevelItemCount(); i++) {
        QTreeWidgetItem* current = this->ui.tw_modules->topLevelItem(i);
        if (current->checkState(twChecked) == Qt::CheckState::Checked)
            return current;
    }
    return nullptr;
}
