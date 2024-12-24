#include <QApplication>
#include <QMessageBox>
#include "win_io.h"
#include "client.h"

void MtbModuleIODialog::sendModuleRequest(ResponseOkEvent&& onOk) {
    DaemonClient::instance->sendNoExc(
        {
            {"command", "module"},
            {"address", this->address},
            {"state", true},
        },
        std::move(onOk),
        [this](unsigned errorCode, QString errorMessage) {
            QApplication::restoreOverrideCursor();
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("moduleUpdate", errorCode, errorMessage));
        }
    );
}

void MtbModuleIODialog::refresh() {
    this->disableAll();
    this->sendModuleRequest(
        [this](const QJsonObject& content) {
            (void)content; // reaction will be done in MtbUniIOWindow::moduleChanged
            QMessageBox::information(this, tr("Info"), tr("Module information successfully updated."));
        }
    );
}
