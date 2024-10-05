#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QString>
#include <QMap>

/* This file defines global settings of the application */

const QString CONFIG_FILENAME = "config.ini";

using Config = QMap<QString, QMap<QString, QVariant>>;

const Config DEFAULTS {
    {"common", {
        {"language", "cz"},
    }},
    {"mtb-daemon", {
        {"host", "127.0.0.1"},
        {"port", "3841"},
    }},
};

class Settings {
public:
    Settings();
    Config data;

    void load(const QString &filename, bool loadNonDefaults = true);
    void save(const QString &filename);

    QMap<QString, QVariant> &at(const QString &g);
    QMap<QString, QVariant> &operator[](const QString &g);

private:
    void loadDefaults();
};

#endif
