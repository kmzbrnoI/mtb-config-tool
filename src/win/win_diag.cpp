#include "win_diag.h"

DiagDialog::DiagDialog(QWidget *parent) :
    QDialog(parent)
{
    this->ui.setupUi(this);
}

void DiagDialog::retranslate() {
    this->ui.retranslateUi(this);
}

void DiagDialog::moduleOpen(const QJsonObject& module) {
    this->show();
}
