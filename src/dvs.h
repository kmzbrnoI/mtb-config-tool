#ifndef DVS_H
#define DVS_H

#include <functional>
#include <cstdint>
#include <QString>
#include <QJsonObject>

using DVDescriptor = std::function<QString(const QJsonObject&)>;

struct DVDef {
    uint8_t dvi;
    QString dvName;
    DVDescriptor repr;
};

extern QVector<DVDef> dvsCommon;
extern QVector<DVDef> dvsRC;

#endif // DVS_H
