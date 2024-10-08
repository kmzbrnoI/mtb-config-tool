#include <QMessageBox>
#include <QJsonArray>
#include <QFileDialog>
#include <QFileInfo>
#include "win_main.h"
#include "version.h"
#include "win_mtbuniconfig.h"

MainWindow::MainWindow(Settings& s, QWidget *parent)
    : QMainWindow(parent), s(s), m_settingsWindow(s), m_mtbUsbWindow(this->m_mtbUsbStatus) {
    ui.setupUi(this);
    this->setWindowTitle(QString(tr("MTB Configuration Tool")+" v%1.%2").\
                         arg(MTB_CONFIG_VERSION_MAJOR).arg(MTB_CONFIG_VERSION_MINOR));

    this->ui.sb_main->addWidget(&this->m_sb_connection);
    this->ui.sb_main->addWidget(&this->m_sb_mtbusb);

    this->connectedUpdate();
    this->ui_setupModulesContextMenu();

    QObject::connect(ui.tw_modules, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ui_twCustomContextMenu(const QPoint&)));

    QObject::connect(ui.a_about, SIGNAL(triggered(bool)), this, SLOT(ui_MAboutTriggered(bool)));
    QObject::connect(ui.a_options, SIGNAL(triggered(bool)), this, SLOT(ui_AOptionsTriggered(bool)));
    QObject::connect(ui.a_connect, SIGNAL(triggered(bool)), this, SLOT(ui_AConnectTriggered(bool)));
    QObject::connect(ui.a_disconnect, SIGNAL(triggered(bool)), this, SLOT(ui_ADisconnectTriggered(bool)));
    QObject::connect(ui.a_mtbusb_settings, SIGNAL(triggered(bool)), this, SLOT(ui_AMtbUsbSettingsTriggered(bool)));
    QObject::connect(ui.a_daemon_connection_settings, SIGNAL(triggered(bool)), this, SLOT(ui_ADaemonConnectSettingsTriggered(bool)));
    QObject::connect(ui.a_modules_refresh, SIGNAL(triggered(bool)), this, SLOT(ui_AModulesRefreshTriggered(bool)));
    QObject::connect(ui.a_log, SIGNAL(triggered(bool)), this, SLOT(ui_ALogTriggered(bool)));
    QObject::connect(ui.a_mtb_daemon_save, SIGNAL(triggered(bool)), this, SLOT(ui_ADaemonSaveConfigTriggered(bool)));

    QObject::connect(ui.tw_modules, SIGNAL(itemSelectionChanged()), this, SLOT(ui_twModulesSelectionChanged()));
    QObject::connect(ui.a_module_configure, SIGNAL(triggered(bool)), this, SLOT(ui_AModuleConfigure()));
    QObject::connect(ui.a_module_reboot, SIGNAL(triggered(bool)), this, SLOT(ui_AModuleReboot()));
    QObject::connect(ui.a_module_fw_upgrade, SIGNAL(triggered(bool)), this, SLOT(ui_AModuleFwUpgrade()));
    QObject::connect(ui.a_module_beacon, SIGNAL(triggered(bool)), this, SLOT(ui_AModuleBeacon()));
    QObject::connect(ui.a_module_diagnostics, SIGNAL(triggered(bool)), this, SLOT(ui_AModuleDiagnostics()));

    QObject::connect(&m_client, SIGNAL(jsonReceived(const QJsonObject&)), this, SLOT(clientJsonReceived(const QJsonObject&)));
    QObject::connect(&m_client, SIGNAL(onConnected()), this, SLOT(clientConnected()));
    QObject::connect(&m_client, SIGNAL(onDisconnected()), this, SLOT(clientDisconnected()));
    QObject::connect(&m_client, SIGNAL(connectError(const QString&)), this, SLOT(clientConnectError(const QString&)));
}

void MainWindow::ui_setupModulesContextMenu() {
    QAction *aConfigure = new QAction(tr("Configure"), this);
    connect(aConfigure, SIGNAL(triggered()), this, SLOT(ui_AModuleConfigure()));
    this->twModulesContextMenu.addAction(aConfigure);

    QAction *aReboot = new QAction(tr("Reboot"), this);
    connect(aReboot, SIGNAL(triggered()), this, SLOT(ui_AModuleReboot()));
    this->twModulesContextMenu.addAction(aReboot);

    QAction *aBeacon = new QAction(tr("Beacon of/off"), this);
    connect(aBeacon, SIGNAL(triggered()), this, SLOT(ui_AModuleBeacon()));
    this->twModulesContextMenu.addAction(aBeacon);

    QAction *aFwUpgrade = new QAction(tr("Firmware upgrade"), this);
    connect(aFwUpgrade, SIGNAL(triggered()), this, SLOT(ui_AModuleFwUpgrade()));
    this->twModulesContextMenu.addAction(aFwUpgrade);

    QAction *aDisgnostics = new QAction(tr("Diagnostics"), this);
    connect(aDisgnostics, SIGNAL(triggered()), this, SLOT(ui_AModuleDiagnostics()));
    this->twModulesContextMenu.addAction(aDisgnostics);
}

void MainWindow::ui_MAboutTriggered(bool) {
    QMessageBox::information(
        this,
        tr("MTB Configuration Tool"),
        QString(tr("MTB Configuration Tool")+
                "\nv%1.%2\n"+
                tr("Created by Jan Horáček 2024")).arg(MTB_CONFIG_VERSION_MAJOR).arg(MTB_CONFIG_VERSION_MINOR),
        QMessageBox::Ok
    );
}

void MainWindow::ui_AOptionsTriggered(bool) {
    this->m_settingsWindow.open();
}

void MainWindow::ui_ADaemonConnectSettingsTriggered(bool) {
    this->m_settingsWindow.open();
}

void MainWindow::retranslate() {
    this->ui.retranslateUi(this);
    this->m_settingsWindow.retranslate();
}

void MainWindow::clientJsonReceived(const QJsonObject& json) {
    if (json["command"] == "mtbusb")
        this->clientReceivedMtbUsb(json["mtbusb"].toObject());
    else if (json["command"] == "module")
        this->clientReceivedModule(json["module"].toObject());
    else if (json["command"] == "modules")
        this->clientReceivedModules(json["modules"].toObject());
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
            this->ui_ADisconnectTriggered(false);
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("version", errorCode, errorMessage)+"\n"+
                                 tr("Are you using MTB Daemon < v1.5? Upgrade!")+"\n"+tr("Closing connection..."));
        }
    );
}

void MainWindow::connectingVersionReceived(const QJsonObject& json) {
    const QJsonObject& jsonVersion = json["version"].toObject();
    if ((!jsonVersion.contains("sw_version_major")) || (!jsonVersion.contains("sw_version_minor")) ||
        (jsonVersion["sw_version_major"].toInt(-1) == -1) || (jsonVersion["sw_version_minor"].toInt(-1) == -1)) {
        QApplication::restoreOverrideCursor();
        this->ui_ADisconnectTriggered(false);
        QMessageBox::warning(this, tr("Error"), tr("Invalid received MTB Daemon version!")+"\n"+tr("Closing connection..."));
        return;
    }

    const QString versionStr = jsonVersion["sw_version"].toString();
    this->m_daemonVersion.emplace(jsonVersion["sw_version_major"].toInt(), jsonVersion["sw_version_minor"].toInt());
    this->m_sb_connection.setText(this->m_sb_connection.text() + " v"+this->m_daemonVersion->str());

    if (!DAEMON_SUPPORTED_VERSIONS.contains(versionStr)) {
        QApplication::restoreOverrideCursor();
        QMessageBox::StandardButton reply = QMessageBox::question(this, "?", tr("Unsupported MTB Daemon received version: ")+versionStr+"\n"+
            tr("Supported versions: ")+daemonSupportedVersionsStr()+"\n"+tr("Continue?"));
        if (reply == QMessageBox::No) {
            this->ui_ADisconnectTriggered(false);
            return;
        }
    }

    // version ok -> ask mtbusb status
    this->m_client.sendNoExc(
        {{"command", "mtbusb"}},
        [this](const QJsonObject& content) {
            this->connectingMtbUsbReceived(content);
        },
        [this](unsigned errorCode, QString errorMessage) {
            QApplication::restoreOverrideCursor();
            this->ui_ADisconnectTriggered(false);
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
            this->ui_ADisconnectTriggered(false);
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
            this->ui_ADisconnectTriggered(false);
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

void MainWindow::ui_AConnectTriggered(bool) {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    this->ui.a_connect->setEnabled(false);
    this->ui.a_disconnect->setEnabled(false);
    this->m_sb_connection.setText(tr("Connecting to MTB Daemon ")+this->daemonHostPort()+" ...");

    try {
        this->m_client.connect(QHostAddress(s["mtb-daemon"]["host"].toString()), s["mtb-daemon"]["port"].toInt());
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

void MainWindow::ui_ADisconnectTriggered(bool) {
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

    this->m_sb_connection.setText((this->m_client.connected()) ? tr("Connected to MTB Daemon ")+this->daemonHostPort(): tr("Disconnected from MTB Daemon"));

    if (!this->m_client.connected()) {
        this->m_sb_mtbusb.setText("---");
        this->ui.tw_modules->clear();
        this->ui.tw_modules->setEnabled(false);
        this->m_mtbUsbWindow.close();

        this->m_daemonVersion.reset();
        this->m_mtbUsbStatus.reset();
        this->m_mtbUsbConnected = false;

        for (auto& module : this->m_modules)
            module = {}; // clear everything

        for (auto& window : this->m_configWindows)
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

    const uint8_t address = module["address"].toInt();
    if (address == 0)
        return;

    if (m_tw_lines[address] == nullptr) {
        auto newItem = new QTreeWidgetItem(this->ui.tw_modules);
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
        setBacground(*item, QC_LIGHT_RED);
    else if ((module["warning"].toBool()) || ((state != "active") && (state != "inactive")))
        setBacground(*item, QC_LIGHT_YELLOW);
    else if (module["beacon"].toBool())
        setBacground(*item, QC_LIGHT_BLUE);
    else if (state == "inactive")
        setBacground(*item, QC_LIGHT_GRAY);
    else
        setBacground(*item, QC_LIGHT_GREEN);
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

    const uint8_t address = json["address"].toInt();
    this->m_modules[address] = json;

    this->ui_updateModule(json);
    this->checkModuleTypeChanged(json);
}

void MainWindow::clientReceivedModules(const QJsonObject& modules) {
    this->ui_twModulesClear();
    for (const QString& addrStr : modules.keys()) {
        const uint8_t addr = addrStr.toInt(0);
        if (addr == 0)
            continue;
        const QJsonObject& module = modules[addrStr].toObject();
        this->m_modules[addr] = module;
        this->ui_updateModule(module);
        this->checkModuleTypeChanged(module);
    }
}

void MainWindow::ui_AModulesRefreshTriggered(bool) {
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

void MainWindow::ui_AMtbUsbSettingsTriggered(bool) {
    this->m_mtbUsbWindow.open();
}

void MainWindow::ui_twModulesClear() {
    this->ui.tw_modules->clear();
    for (auto& ref : this->m_tw_lines)
        ref = nullptr;
}

void MainWindow::ui_ALogTriggered(bool) {
    this->m_logWindow.show();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    this->m_logWindow.close();
    this->m_mtbUsbWindow.close();
    for (auto& window : this->m_configWindows)
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

void MainWindow::ui_ADaemonSaveConfigTriggered(bool) {
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
    this->ui.a_module_reboot->setEnabled(selected);
    this->ui.a_module_beacon->setEnabled(selected);
    this->ui.a_module_fw_upgrade->setEnabled(selected);
    this->ui.a_module_diagnostics->setEnabled(selected);
}

void MainWindow::ui_AModuleConfigure() {
    const QTreeWidgetItem* currentLine = this->ui.tw_modules->currentItem();
    if (currentLine == nullptr)
        return;
    unsigned addr = currentLine->text(TwModulesColumns::twAddrDec).toInt();

    const unsigned typeCode = this->m_modules[addr]["type_code"].toInt();

    if ((typeCode&0xF0) == 0x10) { // MTB-UNI and variants
        if (!this->m_configWindows[addr])
            this->m_configWindows[addr] = std::make_unique<MtbUniConfigWindow>();
    } else {
        QMessageBox::warning(this, tr("Unknown module type"), tr("Unknown module type code ")+QString::number(typeCode)+tr(", no configuration window available!"));
        return;
    }

    // Reopen already active window -> refill up-to-date values
    this->m_configWindows[addr]->editModule(this->m_modules[addr]);
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
        [this](unsigned errorCode, QString errorMessage) {
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("module_upgrade_fw", errorCode, errorMessage));
        }
    );
}

void MainWindow::fwUpgraded(const QJsonObject& json) {
    uint8_t addr = json["address"].toInt();

    // firmware upgraded -> ask new module state
    this->m_client.sendNoExc(
        {{"command", "module"}, {"address", addr}},
        [this, addr](const QJsonObject& content) {
            (void)content;
            QMessageBox::information(this, tr("Finished"), tr("Firmware of module ")+QString::number(addr)+tr(" successfully upgraded."));
        },
        [this](unsigned errorCode, QString errorMessage) {
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("module", errorCode, errorMessage));
        }
    );
}

void MainWindow::ui_AModuleDiagnostics() {

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
    const uint8_t address = module["address"].toInt();
    const unsigned typeCode = module["typeCode"].toInt();

    bool changed = false;
    if (this->m_configWindows[address]) {
        if (((typeCode&0xF0) == 0x10) && (!is<MtbUniConfigWindow>(*this->m_configWindows[address]))) // MTB-UNI and variants
            changed = true;
    }

    if (changed) {
        bool visible = this->m_configWindows[address]->isVisible();
        this->m_configWindows[address].reset();
        if (visible)
            QMessageBox::warning(this, tr("Warning"), tr("Type of module ")+QString::number(address)+tr(" changed, configuration window closed."));
    }
}
