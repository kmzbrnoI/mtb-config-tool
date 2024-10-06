#ifndef COMMON_H
#define COMMON_H

#include <QColor>
#include <QJsonObject>

constexpr QColor QC_LIGHT_RED = QColor(0xFF, 0xAA, 0xAA);
constexpr QColor QC_LIGHT_YELLOW = QColor(0xFF, 0xFF, 0xAA);
constexpr QColor QC_LIGHT_GREEN = QColor(0xAA, 0xFF, 0xAA);
constexpr QColor QC_LIGHT_BLUE = QColor(0xE0, 0xE0, 0xFF);

struct DaemonVersion {
    unsigned major;
    unsigned minor;

    DaemonVersion(unsigned major, unsigned minor) : major(major), minor(minor) {}
    QString str() const { return QString::number(major)+"."+QString::number(minor); }
};

struct MtbUsbStatus {
    unsigned type;
    unsigned speed;
    QString firmware_version;
    QString protocol_version;
    QList<unsigned> activeModules;

    MtbUsbStatus(const QJsonObject& json) { this->update(json); }
    void update(const QJsonObject&);
};

#endif // COMMON_H
