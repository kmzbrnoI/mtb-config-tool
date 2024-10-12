#include <QMessageBox>
#include <QPushButton>
#include "win_change_adress.h"
#include "client.h"

ChangeAddressDialog::ChangeAddressDialog(QWidget *parent) :
    QDialog(parent)
{
    this->ui.setupUi(this);
    this->setFixedSize(this->width(), this->height());
    this->setWindowFlags(Qt::Tool);

    QObject::connect(this->ui.rb_readdress_mode, SIGNAL(toggled(bool)), this, SLOT(ui_rbToggled()));
    QObject::connect(this->ui.rb_specific, SIGNAL(toggled(bool)), this, SLOT(ui_rbToggled()));
    QObject::connect(ui.bb_main, SIGNAL(clicked(QAbstractButton*)), this, SLOT(ui_bClicked(QAbstractButton*)));
}

void ChangeAddressDialog::openFromModule(uint8_t module) {
    this->ui.rb_specific->setChecked(true);
    this->ui.sb_origin_addr->setEnabled(true);
    this->ui.sb_origin_addr->setValue(module);
    this->show();
}

void ChangeAddressDialog::openGeneral() {
    this->ui.rb_readdress_mode->setChecked(true);
    this->ui.sb_origin_addr->setEnabled(false);
    this->show();
}

void ChangeAddressDialog::ui_rbToggled() {
    this->ui.sb_origin_addr->setEnabled(this->ui.rb_specific->isChecked());
}

void ChangeAddressDialog::ui_bClicked(QAbstractButton* btn) {
    if (btn != this->ui.bb_main->button(QDialogButtonBox::StandardButton::Apply))
        return;

    QJsonObject request;
    if (this->ui.rb_specific->isChecked()) {
        request = {
            {"command", "module_set_address"},
            {"address", this->ui.sb_origin_addr->value()},
            {"new_address", this->ui.sb_new_address->value()},
        };
    } else {
        request = {
            {"command", "set_address"},
            {"new_address", this->ui.sb_new_address->value()},
        };
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    DaemonClient::instance->sendNoExc(
        request,
        [this](const QJsonObject& content) {
            (void)content;
            QApplication::restoreOverrideCursor();
            QMessageBox::information(this, tr("Ok"), tr("Command successfully executed."));
        },
        [request, this](unsigned errorCode, QString errorMessage) {
            QApplication::restoreOverrideCursor();
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage(request["command"].toString(), errorCode, errorMessage));
        }
    );
}

void ChangeAddressDialog::retranslate() {
    this->ui.retranslateUi(this);
}
