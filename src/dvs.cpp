#include "dvs.h"
#include <QJsonDocument>

DVs* DVs::instance;

const QVector<DVDef> DVS_COMMON {
    {0, "diagnostic version", DVs::reprSingleValue},
    {1, "state", DVs::reprId},
    {2, "uptime", DVs::reprTime},
    {10, "errors", DVs::reprId},
    {11, "warnings", DVs::reprId},
    {12, "mcu voltage", DVs::reprMcuVoltage},
    {13, "mcu temperature", DVs::reprId},
    {16, "mtbbus received messages", DVs::reprSingleValue},
    {17, "mtbbus received messages with invalid crc", DVs::reprSingleValue},
    {18, "mtbbus sent messages", DVs::reprSingleValue},
    {19, "mtbbus unsent messages", DVs::reprSingleValue},
};

const QVector<DVDef> DVS_RC_COMMON {
    {32, "cutouts started", DVs::reprSingleValue},
    {33, "cutouts finished", DVs::reprSingleValue},
    {34, "cutouts timeouts", DVs::reprSingleValue},
    {35, "cutouts with data in channel 1", DVs::reprSingleValue},
    {36, "cutouts with data in channel 2", DVs::reprSingleValue},
    {37, "cutouts without ready_to_parse", DVs::reprSingleValue},
    {40, "RailCom channel 1 control-sum invalid reads", DVs::reprSingleValue},
    {41, "RailCom channel 2 control-sum invalid reads", DVs::reprSingleValue},
    {42, "addr1_received_count resets", DVs::reprSingleValue},
    {43, "addr2_received_count resets", DVs::reprSingleValue},
    {44, "APP_ID_ADR_LOW received", DVs::reprSingleValue},
    {45, "APP_ID_ADR_HIGH received", DVs::reprSingleValue},
    {46, "ch1 address added/refreshed", DVs::reprSingleValue},
    {47, "ch2 address added/refreshed", DVs::reprSingleValue},
    {130, "DCC received packets", DVs::reprSingleValue},
    {131, "DCC received bad xor", DVs::reprSingleValue},
    {132, "logical 0 after <10 preamble bits", DVs::reprSingleValue},
    {133, "number of mobile decoders read addresses", DVs::reprSingleValue},
};

const QVector<DVDef> DVS_RC_INPUT {
    {50, "T#: RailCom channel 1 control-sum invalid reads", DVs::reprSingleValue},
    {51, "T#: RailCom channel 2 control-sum invalid reads", DVs::reprSingleValue},
    {52, "T#: addr1_received_count resets", DVs::reprSingleValue},
    {53, "T#: addr2_received_count resets", DVs::reprSingleValue},
    {54, "T#: APP_ID_ADR_LOW received", DVs::reprSingleValue},
    {55, "T#: APP_ID_ADR_HIGH received", DVs::reprSingleValue},
    {56, "T#: ch1 address added/refreshed", DVs::reprSingleValue},
    {57, "T#: ch2 address added/refreshed", DVs::reprSingleValue},
};

/////////////////////////////////////////////////////////////////////////////////////

QString DVs::reprId(const QJsonObject &json) {
    return QString(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

QString DVs::reprSingleValue(const QJsonObject &json) {
    const QStringList& keys = json.keys();
    return (keys.count() == 1) ? json[keys[0]].toVariant().toString() : reprId(json);
}

QString DVs::reprMcuVoltage(const QJsonObject &json) {
    double value = json["mcu_voltage"].toDouble();
    double min = json["mcu_voltage_min"].toDouble();
    double max = json["mcu_voltage_max"].toDouble();
    int raw = json["mcu_voltage_raw"].toInt();

    return QString::number(min, 'g', 3) + "V – " + QString::number(value, 'g', 3) + "V – " + QString::number(max, 'g', 3) +
            "V (" + QString::number(raw) + ")";
}

QString DVs::reprTime(const QJsonObject &json) {
    const QStringList& keys = json.keys();
    if (keys.count() == 1) {
        unsigned seconds = json[keys[0]].toInt();
        return QTime(0,0,0).addSecs(seconds).toString("hh:mm:ss") + " ("+QString::number(seconds)+")";
    } else {
        return reprId(json);
    }
}

/////////////////////////////////////////////////////////////////////////////////////

DVs::DVs() {
    this->instance = this;
    this->dvsCommon = DVS_COMMON;

    this->dvsRC = DVS_COMMON;
    this->dvsRC += DVS_RC_COMMON;
    for (unsigned i = 0; i < RC_INPUTS_COUNT; i++) {
        for (const DVDef& def : DVS_RC_INPUT) {
            QString newName = def.dvName;
            newName.replace("#", QString::number(i));
            this->dvsRC.push_back({static_cast<uint8_t>(def.dvi+(10*i)), newName, def.repr});
        }
    }
}

const QVector<DVDef>& DVs::dvs(MtbModuleType moduleType) const {
    switch (moduleType) {
    case MtbModuleType::Rc:
        return this->dvsRC;
    default:
        return this->dvsCommon;
    }
}
