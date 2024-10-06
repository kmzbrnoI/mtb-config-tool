#include "settings.h"

Settings::Settings() { this->loadDefaults(); }

void Settings::loadDefaults() {
    for (const auto [defgrpname, defgrpcontent] : DEFAULTS.asKeyValueRange())
        for (const auto [option, value] : defgrpcontent.asKeyValueRange())
            if (!data[defgrpname].contains(option))
                data[defgrpname][option] = value;
}

void Settings::load(const QString &filename, bool loadNonDefaults) {
    QSettings s(filename, QSettings::IniFormat);
    data.clear();

    for (const auto &g : s.childGroups()) {
        if ((!loadNonDefaults) && (!DEFAULTS.contains(g)))
            continue;

        s.beginGroup(g);
        for (const auto &k : s.childKeys()) {
            if ((!loadNonDefaults) && (!DEFAULTS[g].contains(k)))
                continue;
            data[g][k] = s.value(k, "");
        }
        s.endGroup();
    }

    this->loadDefaults();
}

void Settings::save(const QString &filename) {
    QSettings s(filename, QSettings::IniFormat);

    for (const auto [groupname, groupcontent] : data.asKeyValueRange()) {
        s.beginGroup(groupname);
        for (const auto [option, value] : groupcontent.asKeyValueRange())
            s.setValue(option, value);
        s.endGroup();
    }
}

QMap<QString, QVariant> &Settings::at(const QString &g) { return data[g]; }
QMap<QString, QVariant> &Settings::operator[](const QString &g) { return at(g); }
