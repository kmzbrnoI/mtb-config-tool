#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QTranslator translator;
    translator.load(":/qm/mtb-config_cz");
    a.installTranslator(&translator);
    QObject::tr("Hello John!");
    MainWindow w;
    w.show();
    return a.exec();
}
