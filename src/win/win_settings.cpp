#include <QMessageBox>
#include "win_settings.h"
#include "main.h"

SettingsWindow::SettingsWindow(Settings& s, QWidget *parent)
    : QDialog{parent}, s(s) {
    ui.setupUi(this);
    this->setFixedSize(this->width(), this->height());
    this->setWindowFlags(Qt::Tool);
}

void SettingsWindow::open() {
    this->ui.cb_language->setCurrentIndex((this->s["general"]["language"].toString() == "cz") ? 1 : 0);
    this->ui.le_mtb_daemon_host->setText(this->s["mtb-daemon"]["host"].toString());
    this->ui.se_mtb_daemon_port->setValue(this->s["mtb-daemon"]["port"].toInt());

    this->show();
}

void SettingsWindow::accept() {
    if (this->ui.cb_language->currentIndex() < 0) {
        QMessageBox::warning(this, tr("Cannot save!"), tr("Select a language!"));
        return;
    }
    if (this->ui.le_mtb_daemon_host->text() == "") {
        QMessageBox::warning(this, tr("Cannot save!"), tr("Fill a mtb-daemon hostname!"));
        return;
    }

    switch (this->ui.cb_language->currentIndex()) {
    default:
    case 0:
        this->s["general"]["language"] = "en";
        translate_app_en();
        break;
    case 1:
        this->s["general"]["language"] = "cz";
        translate_app_cz();
        break;
    }

    this->s["mtb-daemon"]["host"] = this->ui.le_mtb_daemon_host->text();
    this->s["mtb-daemon"]["port"] = this->ui.se_mtb_daemon_port->value();

    try {
        s.save(CONFIG_FILENAME);
    } catch (...) {
        QMessageBox::critical(this, tr("Cannot save!"), tr("Unable to save file ")+CONFIG_FILENAME);
    }

    this->close();
    this->accepted();
}

void SettingsWindow::retranslate() {
    this->ui.retranslateUi(this);
}
