#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QTranslator translator;
    bool success = translator.load(":/qm/mtb-config_cz");
    if (!success)
        return 1;
    a.installTranslator(&translator);
    MainWindow w;
    w.show();
    return a.exec();
}
