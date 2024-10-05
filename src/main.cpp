#include "win_main.h"
#include "settings.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QTranslator translator;
    bool success = translator.load(":/qm/mtb-config_cz");
    if (!success)
        return 1;
    a.installTranslator(&translator);

    Settings settings;
    settings.load(CONFIG_FILENAME);

    MainWindow w(settings);
    w.show();
    return a.exec();
}
