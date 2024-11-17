#include <QMessageBox>
#include "win_mtbusb.h"
#include "client.h"

MtbUsbWindow::MtbUsbWindow(const std::optional<MtbUsbStatus> &mtbusb, QWidget *parent)
    : QDialog{parent}, mtbUsbStatus(mtbusb)
{
    ui.setupUi(this);
    this->setFixedSize(this->width(), this->height());

    QObject::connect(ui.b_update, SIGNAL(released()), this, SLOT(ui_bUpdateHandle()));
    QObject::connect(ui.bb_main, SIGNAL(clicked(QAbstractButton*)), this, SLOT(ui_bClicked(QAbstractButton*)));
}

void MtbUsbWindow::open() {
    this->update();
    this->exec();
}

void MtbUsbWindow::ui_bUpdateHandle() {
    this->update();
    QMessageBox::information(this, tr("Updated"), tr("Updated from internal variables"));
}

void MtbUsbWindow::update() {
    if (this->mtbUsbStatus) {
        const MtbUsbStatus& status = this->mtbUsbStatus.value();
        this->ui.l_type->setText(QString::number(status.type));
        this->ui.l_fw_version->setText(status.firmware_version);
        this->ui.l_protocol_version->setText(status.protocol_version);

        switch (status.speed) {
        case 38400: this->ui.cb_speed->setCurrentIndex(0); break;
        case 57600: this->ui.cb_speed->setCurrentIndex(1); break;
        case 115200: this->ui.cb_speed->setCurrentIndex(2); break;
        case 230400: this->ui.cb_speed->setCurrentIndex(3); break;
        default:
            this->ui.cb_speed->setCurrentIndex(-1);
        }
    } else {
        this->ui.l_type->setText("N/A");
        this->ui.l_fw_version->setText("N/A");
        this->ui.l_protocol_version->setText("N/A");
        this->ui.cb_speed->setCurrentIndex(-1);
    }
}

void MtbUsbWindow::ui_bClicked(QAbstractButton *button) {
    if (button == this->ui.bb_main->button(QDialogButtonBox::StandardButton::Apply)) {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        unsigned newSpeed = this->selectedSpeed();
        if (newSpeed == 0) {
            QMessageBox::warning(this, tr("Error"), tr("Select a valid MTBbus speed!"));
            return;
        }

        DaemonClient::instance->sendNoExc(
            {{"command", "mtbusb"}, {"mtbusb", QJsonObject({{"speed", static_cast<int>(newSpeed)}})}},
            [this](const QJsonObject& content) {
                (void)content;
                QApplication::restoreOverrideCursor();
                QMessageBox::information(this, tr("Ok"), tr("Speed successfully set."));
            },
            [this](unsigned errorCode, QString errorMessage) {
                QApplication::restoreOverrideCursor();
                QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("mtbusb", errorCode, errorMessage));
            }
        );

    }
}

unsigned MtbUsbWindow::selectedSpeed() const {
    switch (this->ui.cb_speed->currentIndex()) {
    case 0: return 38400;
    case 1: return 57600;
    case 2: return 115200;
    case 3: return 230400;
    default: return 0;
    }
}

void MtbUsbWindow::retranslate() {
    this->ui.retranslateUi(this);
}
