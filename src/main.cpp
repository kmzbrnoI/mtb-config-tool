#include "win_main.h"
#include "settings.h"
#include <QApplication>
#include <QTranslator>
#include <memory>
#include "main.h"

std::unique_ptr<QTranslator> cz_translator;
std::unique_ptr<MainWindow> main_window;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    {
        // Load CZ translator
        const QString cz_trans_path = ":/qm/mtb-config_cz";
        cz_translator = std::make_unique<QTranslator>();
        bool success = cz_translator->load(cz_trans_path);
        if (!success) {
            qCritical() << "Unable to load translation " << cz_trans_path << "!" << Qt::endl;
            return 1;
        }
    }

    Settings settings;
    settings.load(CONFIG_FILENAME);

    main_window = std::make_unique<MainWindow>(settings);
    main_window->show();

    if (settings["common"]["language"] == "cz")
        translate_app_cz();
    else
        translate_app_en();

    int result = a.exec();

    main_window.release();
    cz_translator.release();

    return result;
}

void translate_app_cz() {
    qApp->removeTranslator(cz_translator.get());
    if (!qApp->installTranslator(cz_translator.get()))
        qCritical() << "Unable to install translator cz_translator!" << Qt::endl;
    main_window->retranslate();
}

void translate_app_en() {
    qApp->removeTranslator(cz_translator.get());
    main_window->retranslate();
}
