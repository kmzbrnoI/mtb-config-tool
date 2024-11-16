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

enum class MtbModuleType {
    Unknown = 0x00,
    Univ2ir = 0x10,
    Univ2noIr = 0x11,
    Univ40 = 0x15,
    Univ42 = 0x16,
    Unis10 = 0x50,
    Rc = 0x30,
};

QString moduleTypeToStr(MtbModuleType type);

void setBacground(QTreeWidgetItem& item, const QBrush& brush);

template <typename Target, typename Source>
bool is(const Source &x) {
    return (dynamic_cast<const Target *>(&x) != nullptr);
}

constexpr unsigned UNI_INPUTS_COUNT = 16;
constexpr unsigned UNIS_INPUTS_COUNT = 16;
constexpr unsigned UNI_OUTPUTS_COUNT = 16;
constexpr unsigned UNIS_OUTPUTS_COUNT = 28;
constexpr unsigned RC_INPUTS_COUNT = 8;

const std::array<QString, 17> SComSignalCodes {
    "Stůj/posun zakázán",
    "Volno",
    "Výstraha",
    "Očekávejte 40 km/h",
    "40 km/h a volno",
    "Svítí vše (rezerva)",
    "40 km/h a výstraha",
    "40 km/h a očekávejte 40 km/h",
    "Přivolávací návěst",
    "Dovolen zajištěný posun",
    "Dovolen nezajištěný posun",
    "Opakování návěsti volno",
    "Opakování návěsti výstraha",
    "Návěstidlo zhaslé",
    "Opak. návěsti očekávejte 40 km/h",
    "Opak. návěsti výstraha a 40 km/h",
    "Opak. návěsti oček. 40 km/h a 40 km/h",
};

const std::array<unsigned, 8> UniFlickerPerMin {60, 120, 128, 240, 300, 600, 33, 66};

#endif // COMMON_H
