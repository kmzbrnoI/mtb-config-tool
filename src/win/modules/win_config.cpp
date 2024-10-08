#include <QApplication>
#include <QMessageBox>
#include "win_config.h"
#include "client.h"

void MtbModuleConfigDialog::refresh() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    DaemonClient::instance->sendNoExc(
        {{"command", "module"}, {"address", static_cast<int>(this->address)}},
        [this](const QJsonObject& content) {
            QApplication::restoreOverrideCursor();
            if (this->isVisible()) { // is case of different type received
                this->update(content["module"].toObject());
                QMessageBox::information(this, tr("Finished"), tr("Module information successfully updated."));
            }
        },
        [this](unsigned errorCode, QString errorMessage) {
            QApplication::restoreOverrideCursor();
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("module", errorCode, errorMessage));
        }
    );
}
