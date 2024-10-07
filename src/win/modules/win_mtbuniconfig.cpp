#include "win_mtbuniconfig.h"

MtbUniConfigWindow::MtbUniConfigWindow(QWidget *parent) :
    MtbModuleConfigDialog(parent) {
    this->ui.setupUi(this);
}

void MtbUniConfigWindow::editModule(const QJsonObject& module) {
    const uint8_t addr = module["address"].toInt();

    this->setWindowTitle(tr("Configuration of module ")+QString::number(addr)+" – "+module["type"].toString());
    this->show();
}

void MtbUniConfigWindow::newModule() {
    this->show();
}
