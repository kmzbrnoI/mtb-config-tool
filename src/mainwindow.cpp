#include <QMessageBox>
#include "mainwindow.h"
#include "version.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    this->setWindowTitle(QString("MTB konfigurační nástroj v%1.%2").arg(MTB_CONFIG_VERSION_MAJOR).arg(MTB_CONFIG_VERSION_MINOR));

    QObject::connect(ui.a_about, SIGNAL(triggered(bool)), this, SLOT(ui_m_about_triggered(bool)));
}

MainWindow::~MainWindow()
{

}

void MainWindow::ui_m_about_triggered(bool)
{
    QMessageBox::information(
        this,
        "MTB konfigurační nástroj",
        QString("MTB konfigurační nástroj\nv%1.%2\nVytvořil Jan Horáček 2024").arg(MTB_CONFIG_VERSION_MAJOR).arg(MTB_CONFIG_VERSION_MINOR),
        QMessageBox::Ok
    );
}
