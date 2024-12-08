#include <QApplication>
#include <QMessageBox>
#include "win_io.h"
#include "client.h"

void MtbModuleIODialog::sendModuleRequest() {
    DaemonClient::instance->sendNoExc(
        {
            {"command", "module"},
            {"address", this->address},
            {"state", true},
        },
        [](const QJsonObject& content) {
            (void)content; // reaction will be done in MtbUniIOWindow::moduleChanged
        },
        [this](unsigned errorCode, QString errorMessage) {
            QApplication::restoreOverrideCursor();
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("moduleUpdate", errorCode, errorMessage));
        }
    );
}
