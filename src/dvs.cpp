#include "dvs.h"
#include <QJsonDocument>

QString reprId(const QJsonObject&);
QString reprSingleValue(const QJsonObject&);
QString reprMcuVoltage(const QJsonObject&);
QString reprTime(const QJsonObject&);

QVector<DVDef> dvsCommon {
    {0, "diagnostic version", reprSingleValue},
    {1, "state", reprId},
    {2, "uptime", reprTime},
    {10, "errors", reprId},
    {11, "warnings", reprId},
    {12, "mcu voltage", reprMcuVoltage},
    {13, "mcu temperature", reprId},
    {16, "mtbbus received messages", reprSingleValue},
    {17, "mtbbus received messages with invalid crc", reprSingleValue},
    {18, "mtbbus sent messages", reprSingleValue},
    {19, "mtbbus unsent messages", reprSingleValue},
};

QVector<DVDef> dvsRC {
    {32, "cutouts started", reprSingleValue},
    {33, "cutouts finished", reprSingleValue},
    {34, "cutouts timeouts", reprSingleValue},
    {35, "cutouts with data in channel 1", reprSingleValue},
    {36, "cutouts with data in channel 2", reprSingleValue},
    {37, "cutouts without ready_to_parse", reprSingleValue},
    {40, "RailCom channel 1 control-sum invalid reads", reprSingleValue},
    {41, "RailCom channel 2 control-sum invalid reads", reprSingleValue},
    {42, "addr1_received_count resets", reprSingleValue},
    {43, "addr2_received_count resets", reprSingleValue},
    {44, "APP_ID_ADR_LOW received", reprSingleValue},
    {45, "APP_ID_ADR_HIGH received", reprSingleValue},
    {46, "ch1 address added/refreshed", reprSingleValue},
    {47, "ch2 address added/refreshed", reprSingleValue},

    {50, "T0: RailCom channel 1 control-sum invalid reads", reprSingleValue},
    {51, "T0: RailCom channel 2 control-sum invalid reads", reprSingleValue},
    {52, "T0: addr1_received_count resets", reprSingleValue},
    {53, "T0: addr2_received_count resets", reprSingleValue},
    {54, "T0: APP_ID_ADR_LOW received", reprSingleValue},
    {55, "T0: APP_ID_ADR_HIGH received", reprSingleValue},
    {56, "T0: ch1 address added/refreshed", reprSingleValue},
    {57, "T0: ch2 address added/refreshed", reprSingleValue},

    {60, "T1: RailCom channel 1 control-sum invalid reads", reprSingleValue},
    {61, "T1: RailCom channel 2 control-sum invalid reads", reprSingleValue},
    {62, "T1: addr1_received_count resets", reprSingleValue},
    {63, "T1: addr2_received_count resets", reprSingleValue},
    {64, "T1: APP_ID_ADR_LOW received", reprSingleValue},
    {65, "T1: APP_ID_ADR_HIGH received", reprSingleValue},
    {66, "T1: ch1 address added/refreshed", reprSingleValue},
    {67, "T1: ch2 address added/refreshed", reprSingleValue},

    {70, "T2: RailCom channel 1 control-sum invalid reads", reprSingleValue},
    {71, "T2: RailCom channel 2 control-sum invalid reads", reprSingleValue},
    {72, "T2: addr1_received_count resets", reprSingleValue},
    {73, "T2: addr2_received_count resets", reprSingleValue},
    {74, "T2: APP_ID_ADR_LOW received", reprSingleValue},
    {75, "T2: APP_ID_ADR_HIGH received", reprSingleValue},
    {76, "T2: ch1 address added/refreshed", reprSingleValue},
    {77, "T2: ch2 address added/refreshed", reprSingleValue},

    {80, "T3: RailCom channel 1 control-sum invalid reads", reprSingleValue},
    {81, "T3: RailCom channel 2 control-sum invalid reads", reprSingleValue},
    {82, "T3: addr1_received_count resets", reprSingleValue},
    {83, "T3: addr2_received_count resets", reprSingleValue},
    {84, "T3: APP_ID_ADR_LOW received", reprSingleValue},
    {85, "T3: APP_ID_ADR_HIGH received", reprSingleValue},
    {86, "T3: ch1 address added/refreshed", reprSingleValue},
    {87, "T3: ch2 address added/refreshed", reprSingleValue},

    {90, "T4: RailCom channel 1 control-sum invalid reads", reprSingleValue},
    {91, "T4: RailCom channel 2 control-sum invalid reads", reprSingleValue},
    {92, "T4: addr1_received_count resets", reprSingleValue},
    {93, "T4: addr2_received_count resets", reprSingleValue},
    {94, "T4: APP_ID_ADR_LOW received", reprSingleValue},
    {95, "T4: APP_ID_ADR_HIGH received", reprSingleValue},
    {96, "T4: ch1 address added/refreshed", reprSingleValue},
    {97, "T4: ch2 address added/refreshed", reprSingleValue},

    {100, "T5: RailCom channel 1 control-sum invalid reads", reprSingleValue},
    {101, "T5: RailCom channel 2 control-sum invalid reads", reprSingleValue},
    {102, "T5: addr1_received_count resets", reprSingleValue},
    {103, "T5: addr2_received_count resets", reprSingleValue},
    {104, "T5: APP_ID_ADR_LOW received", reprSingleValue},
    {105, "T5: APP_ID_ADR_HIGH received", reprSingleValue},
    {106, "T5: ch1 address added/refreshed", reprSingleValue},
    {107, "T5: ch2 address added/refreshed", reprSingleValue},

    {110, "T6: RailCom channel 1 control-sum invalid reads", reprSingleValue},
    {111, "T6: RailCom channel 2 control-sum invalid reads", reprSingleValue},
    {112, "T6: addr1_received_count resets", reprSingleValue},
    {113, "T6: addr2_received_count resets", reprSingleValue},
    {114, "T6: APP_ID_ADR_LOW received", reprSingleValue},
    {115, "T6: APP_ID_ADR_HIGH received", reprSingleValue},
    {116, "T6: ch1 address added/refreshed", reprSingleValue},
    {117, "T6: ch2 address added/refreshed", reprSingleValue},

    {120, "T7: RailCom channel 1 control-sum invalid reads", reprSingleValue},
    {121, "T7: RailCom channel 2 control-sum invalid reads", reprSingleValue},
    {122, "T7: addr1_received_count resets", reprSingleValue},
    {123, "T7: addr2_received_count resets", reprSingleValue},
    {124, "T7: APP_ID_ADR_LOW received", reprSingleValue},
    {125, "T7: APP_ID_ADR_HIGH received", reprSingleValue},
    {126, "T7: ch1 address added/refreshed", reprSingleValue},
    {127, "T7: ch2 address added/refreshed", reprSingleValue},

    {130, "DCC received packets", reprSingleValue},
    {131, "DCC received bad xor", reprSingleValue},
    {132, "logical 0 after <10 preamble bits", reprSingleValue},
    {133, "number of mobile decoders read addresses", reprSingleValue},
};

/////////////////////////////////////////////////////////////////////////////////////

QString reprId(const QJsonObject &json) {
    return QString(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

QString reprSingleValue(const QJsonObject &json) {
    const QStringList& keys = json.keys();
    return (keys.count() == 1) ? json[keys[0]].toVariant().toString() : reprId(json);
}

QString reprMcuVoltage(const QJsonObject &json) {
    double value = json["mcu_voltage"].toDouble();
    double min = json["mcu_voltage_min"].toDouble();
    double max = json["mcu_voltage_max"].toDouble();
    int raw = json["mcu_voltage_raw"].toInt();

    return QString::number(min, 'g', 3) + "V – " + QString::number(value, 'g', 3) + "V – " + QString::number(max, 'g', 3) +
            "V (" + QString::number(raw) + ")";
}

QString reprTime(const QJsonObject &json) {
    const QStringList& keys = json.keys();
    if (keys.count() == 1) {
        unsigned seconds = json[keys[0]].toInt();
        return QTime(0,0,0).addSecs(seconds).toString("hh:mm:ss") + " ("+QString::number(seconds)+")";
    } else {
        return reprId(json);
    }
}
