#include <QMessageBox>
#include "win_main.h"
#include "version.h"

MainWindow::MainWindow(Settings& s, QWidget *parent)
    : QMainWindow(parent), s(s), settings_window(s) {
    ui.setupUi(this);
    this->setWindowTitle(QString(tr("MTB Configuration Tool")+" v%1.%2").\
                         arg(MTB_CONFIG_VERSION_MAJOR).arg(MTB_CONFIG_VERSION_MINOR));

    QObject::connect(ui.a_about, SIGNAL(triggered(bool)), this, SLOT(ui_m_about_triggered(bool)));
    QObject::connect(ui.a_options, SIGNAL(triggered(bool)), this, SLOT(ui_a_options_triggered(bool)));
}

void MainWindow::ui_m_about_triggered(bool) {
    QMessageBox::information(
        this,
        tr("MTB Configuration Tool"),
        QString(tr("MTB Configuration Tool")+
                "\nv%1.%2\n"+
                tr("Created by Jan Horáček 2024")).arg(MTB_CONFIG_VERSION_MAJOR).arg(MTB_CONFIG_VERSION_MINOR),
        QMessageBox::Ok
    );
}

void MainWindow::ui_a_options_triggered(bool) {
    this->settings_window.open();
}

void MainWindow::retranslate() {
    this->ui.retranslateUi(this);
    this->settings_window.retranslate();
}
