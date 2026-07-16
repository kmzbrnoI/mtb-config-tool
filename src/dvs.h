#ifndef DVS_H
#define DVS_H

#include <functional>
#include <cstdint>
#include <QString>
#include <QJsonObject>
#include "common.h"

using DVDescriptor = std::function<QString(const QJsonObject&)>;

struct DVDef {
    uint8_t dvi;
    QString dvName;
    DVDescriptor repr;
};

class DVs {
private:
    QVector<DVDef> dvsCommon;
    QVector<DVDef> dvsRC;

public:
    static DVs* instance;

    DVs();
    const QVector<DVDef>& dvs(MtbModuleType) const;

    static QString reprId(const QJsonObject&);
    static QString reprSingleValue(const QJsonObject&);
    static QString reprMcuVoltage(const QJsonObject&);
    static QString reprTime(const QJsonObject&);
};

#endif // DVS_H
