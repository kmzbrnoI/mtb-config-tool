#ifndef COMMON_H
#define COMMON_H

#include <QColor>
#include <QJsonObject>
#include <QTreeWidgetItem>

constexpr QColor QC_LIGHT_RED = QColor(0xFF, 0xAA, 0xAA);
constexpr QColor QC_LIGHT_YELLOW = QColor(0xFF, 0xFF, 0xAA);
constexpr QColor QC_LIGHT_GREEN = QColor(0xCA, 0xFF, 0xCA);
constexpr QColor QC_LIGHT_BLUE = QColor(0xE0, 0xE0, 0xFF);
constexpr QColor QC_LIGHT_GRAY = QColor(0xE0, 0xE0, 0xE0);

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

void setBacground(QTreeWidgetItem& item, const QBrush& brush);

#endif // COMMON_H
