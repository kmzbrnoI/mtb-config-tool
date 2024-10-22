#include "win_main.h"
#include "settings.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <memory>
#include <optional>
#include <array>
#include "main.h"

std::vector<std::unique_ptr<QTranslator>> cz_translators;
std::unique_ptr<MainWindow> main_window;

std::unique_ptr<QTranslator> load_translation(const QString& filename, const QString& directory = QString());

///////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    cz_translators.push_back(load_translation(":/i18n/mtb-config_cz"));
    cz_translators.push_back(load_translation("qt_cs", QLibraryInfo::path(QLibraryInfo::TranslationsPath))); // messageBox buttons etc. translations

    for (const auto& ptr : cz_translators)
        if (!ptr)
            return 1;

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
    cz_translators.clear();

    return result;
}

///////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<QTranslator> load_translation(const QString& filename, const QString& directory) {
    std::unique_ptr<QTranslator> trans = std::make_unique<QTranslator>();
    bool success = trans->load(filename, directory);
    if (!success) {
        qCritical() << "Unable to load translation " << directory << filename << "!" << Qt::endl;
        return {};
    }
    return trans;
}

void translate_app_cz() {
    for (std::unique_ptr<QTranslator>& trans : cz_translators) {
        qApp->removeTranslator(trans.get());
        if (!qApp->installTranslator(trans.get()))
            qCritical() << "Unable to install translator " << trans->filePath() << "!" << Qt::endl;
    }
    main_window->retranslate();
}

void translate_app_en() {
    for (std::unique_ptr<QTranslator>& trans : cz_translators)
        qApp->removeTranslator(trans.get());
    main_window->retranslate();
}
