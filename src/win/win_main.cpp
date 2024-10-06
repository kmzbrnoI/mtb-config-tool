#include <QMessageBox>
#include "win_main.h"
#include "version.h"

MainWindow::MainWindow(Settings& s, QWidget *parent)
    : QMainWindow(parent), s(s), m_settingsWindow(s) {
    ui.setupUi(this);
    this->setWindowTitle(QString(tr("MTB Configuration Tool")+" v%1.%2").\
                         arg(MTB_CONFIG_VERSION_MAJOR).arg(MTB_CONFIG_VERSION_MINOR));

    this->ui.sb_main->addWidget(&this->m_sb_connection);
    this->ui.sb_main->addWidget(&this->m_sb_mtbusb);

    this->connectedUpdateGui();

    QObject::connect(ui.a_about, SIGNAL(triggered(bool)), this, SLOT(ui_MAboutTriggered(bool)));
    QObject::connect(ui.a_options, SIGNAL(triggered(bool)), this, SLOT(ui_AOptionsTriggered(bool)));
    QObject::connect(ui.a_connect, SIGNAL(triggered(bool)), this, SLOT(ui_AConnectTriggered(bool)));
    QObject::connect(ui.a_disconnect, SIGNAL(triggered(bool)), this, SLOT(ui_ADisconnectTriggered(bool)));
    QObject::connect(ui.a_mtbusb_settings, SIGNAL(triggered(bool)), this, SLOT(ui_AMtbUsbSettingsTriggered(bool)));
    QObject::connect(ui.a_daemon_connection_settings, SIGNAL(triggered(bool)), this, SLOT(ui_ADaemonConnectSettingsTriggered(bool)));
    QObject::connect(ui.a_modules_refresh, SIGNAL(triggered(bool)), this, SLOT(ui_AModulesRefreshTriggered(bool)));

    QObject::connect(&m_client, SIGNAL(jsonReceived(const QJsonObject&)), this, SLOT(clientJsonReceived(const QJsonObject&)));
    QObject::connect(&m_client, SIGNAL(onConnected()), this, SLOT(clientConnected()));
    QObject::connect(&m_client, SIGNAL(onDisconnected()), this, SLOT(clientDisconnected()));
    QObject::connect(&m_client, SIGNAL(connectError(const QString&)), this, SLOT(clientConnectError(const QString&)));
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
    this->connectedUpdateGui();

    this->m_client.sendNoExc(
        {{"command", "version"}},
        [this](const QJsonObject& content) {
            this->connectingVersionReceived(content);
        },
        [this](unsigned errorCode, QString errorMessage) {
            QApplication::restoreOverrideCursor();
            this->ui_ADisconnectTriggered(false);
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("version", errorCode, errorMessage)+"\n"+tr("Closing connection..."));
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

    this->m_daemonVersion.emplace(jsonVersion["sw_version_major"].toInt(), jsonVersion["sw_version_minor"].toInt());
    this->m_sb_connection.setText(this->m_sb_connection.text() + " v"+this->m_daemonVersion->str());

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
    this->m_client.sendNoExc(
        {{"command", "modules"}},
        [this](const QJsonObject& content) {
            (void)content;
            this->ui.tw_modules->setEnabled(true);
            QApplication::restoreOverrideCursor();
        },
        [this](unsigned errorCode, QString errorMessage) {
            this->ui_ADisconnectTriggered(false);
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("modules", errorCode, errorMessage)+"\n"+tr("Closing connection..."));
        }
    );
}

void MainWindow::clientConnectError(const QString& msg) {
    QApplication::restoreOverrideCursor();
    QMessageBox::warning(this, tr("Error!"), tr("Unable to connect to ")+this->daemonHostPort()+".\n"+msg);
}

void MainWindow::clientDisconnected() {
    this->m_daemonVersion.reset();
    this->connectedUpdateGui();
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
        this->connectedUpdateGui();
    } catch (...) {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, tr("Unknown exception!"), tr("Error!"));
        this->connectedUpdateGui();
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
        this->connectedUpdateGui();
    } catch (...) {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, tr("Unknown exception!"), tr("Error!"));
        this->connectedUpdateGui();
    }
}

void MainWindow::connectedUpdateGui() {
    this->ui.a_connect->setEnabled(!this->m_client.connected());
    this->ui.a_disconnect->setEnabled(this->m_client.connected());
    this->ui.a_modules_refresh->setEnabled(this->m_client.connected());
    this->ui.a_mtbusb_settings->setEnabled(this->m_client.connected());

    this->m_sb_connection.setText((this->m_client.connected()) ? tr("Connected to MTB Daemon ")+this->daemonHostPort(): tr("Disconnected from MTB Daemon"));

    if (!this->m_client.connected()) {
        this->m_sb_mtbusb.setText("---");
        this->ui.tw_modules->clear();
        this->ui.tw_modules->setEnabled(false);
    }
}

QString MainWindow::daemonHostPort() const {
    return s["mtb-daemon"]["host"].toString() + ":" + s["mtb-daemon"]["port"].toString();
}

void MainWindow::ui_updateModule(const QJsonObject& module) {
    if (!module.contains("address")) {
        qDebug() << "MainWindow::ui_updateModule json does not contains 'address'";
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

    item->setText(0, QString::number(address));
    item->setText(1, "0x"+(QString::number(address, 16).rightJustified(2, '0')));
    item->setText(2, "0b"+(QString::number(address, 2).rightJustified(8, '0')));
    item->setText(3, module["name"].toString());
    item->setText(4, module["type"].toString());
    item->setText(5, module["state"].toString());
    const QString deprecated = module["fw_deprecated"].toBool() ? tr(" (deprecated)") : "";
    item->setText(6, module.contains("firmware_version") ? module["firmware_version"].toString()+deprecated : "N/A");
    item->setText(7, module.contains("bootloader_version") ? module["bootloader_version"].toString() : "N/A");
    item->setText(8, module.contains("error") ? (module["error"].toBool() ? tr("ERROR") : "-") : "N/A");
    item->setText(9, module.contains("warning") ? (module["warning"].toBool() ? tr("WARN") : "-") : "N/A");
    item->setText(10, module.contains("beacon") ? (module["beacon"].toBool() ? tr("YES") : "-") : "N/A");
}

unsigned MainWindow::ui_twModulesInsertIndex(unsigned addr) {
    while ((addr < MTBBUS_ADDR_COUNT) && (this->m_tw_lines[addr] == nullptr))
        addr++;
    return (addr == MTBBUS_ADDR_COUNT) ? this->ui.tw_modules->topLevelItemCount() : this->ui.tw_modules->indexOfTopLevelItem(this->m_tw_lines[addr]);
}

void MainWindow::clientReceivedMtbUsb(const QJsonObject& json) {
    const bool connected = json["connected"].toBool();

    if (connected) {
        this->m_sb_mtbusb.setText(
            tr("MTB-USB connected: type: ")+QString::number(json["type"].toInt())+", "+
            tr("MtbBus speed: ")+QString::number(json["speed"].toInt())+" bdps, "+
            tr("FW: v")+json["firmware_version"].toString()+", "+
            tr("protocol: v")+json["protocol_version"].toString()
        );
    } else {
        this->m_sb_mtbusb.setText(tr("No MTB-USB connected"));
    }
}

void MainWindow::clientReceivedModule(const QJsonObject& json) {
    this->ui_updateModule(json);
}

void MainWindow::clientReceivedModules(const QJsonObject& modules) {
    this->ui_twModulesClear();
    for (const QString& addr : modules.keys())
         this->ui_updateModule(modules[addr].toObject());
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

}

void MainWindow::ui_twModulesClear() {
    this->ui.tw_modules->clear();
    for (auto& ref : this->m_tw_lines)
        ref = nullptr;
}
