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

void MainWindow::retranslate() {
    this->ui.retranslateUi(this);
    this->m_settingsWindow.retranslate();
}

void MainWindow::clientJsonReceived(const QJsonObject& json) {
    if (json["command"] == "mtbusb")
        this->clientReceivedMtbUsb(json["mtbusb"].toObject());
}

void MainWindow::clientConnected() {
    this->connectedUpdateGui();

    this->m_client.sendNoExc(
        {{"command", "mtbusb"}},
        [](const QJsonObject& content) {
            (void)content;
            QApplication::restoreOverrideCursor();
        },
        [this](unsigned errorCode, QString errorMessage) {
            (void)errorCode;
            (void)errorMessage;
            this->ui_ADisconnectTriggered(false);
            QMessageBox::warning(this, tr("No response for 'mtbusb' command from MTB Daemon server. Closing connection..."), tr("Error"));
        }
    );
}

void MainWindow::clientConnectError(const QString& msg) {
    QApplication::restoreOverrideCursor();
    QMessageBox::warning(this, tr("Error!"), tr("Unable to connect to ")+this->daemonHostPort()+".\n"+msg);
}

void MainWindow::clientDisconnected() {
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
        QMessageBox::critical(this, e.str(), tr("Error!"));
        this->connectedUpdateGui();
    } catch (...) {
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
        QMessageBox::critical(this, e.str(), tr("Error!"));
        this->connectedUpdateGui();
    } catch (...) {
        QMessageBox::critical(this, tr("Unknown exception!"), tr("Error!"));
        this->connectedUpdateGui();
    }
}

void MainWindow::connectedUpdateGui() {
    this->ui.a_connect->setEnabled(!this->m_client.connected());
    this->ui.a_disconnect->setEnabled(this->m_client.connected());

    this->m_sb_connection.setText((this->m_client.connected()) ? tr("Connected to MTB Daemon ")+this->daemonHostPort(): tr("Disconnected from MTB Daemon"));

    if (!this->m_client.connected()) {
        this->m_sb_mtbusb.setText("---");
    }
}

QString MainWindow::daemonHostPort() const {
    return s["mtb-daemon"]["host"].toString() + ":" + s["mtb-daemon"]["port"].toString();
}

void MainWindow::clientReceivedMtbUsb(const QJsonObject& json) {
    const bool connected = json["connected"].toBool();

    if (connected) {
        qDebug() << json;
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
