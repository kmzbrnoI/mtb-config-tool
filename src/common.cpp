#include <QJsonArray>
#include "common.h"

void MtbUsbStatus::update(const QJsonObject& json) {
    this->type = json["type"].toInt();
    this->speed = json["speed"].toInt();
    this->firmware_version = json["firmware_version"].toString();
    this->firmware_deprecated = json["firmware_deprecated"].toBool();
    this->protocol_version = json["protocol_version"].toString();

    this->activeModules.clear();
    for (const QJsonValue& addr : json["active_modules"].toArray())
        this->activeModules.append(addr.toInt());
}

void setBackground(QTreeWidgetItem& item, const QBrush& brush) {
    for (int i = 0; i < item.columnCount(); i++)
        item.setBackground(i, brush);
}

QString moduleTypeToStr(MtbModuleType type) {
    switch (type) {
    case MtbModuleType::Univ2ir: return "MTB-UNI v2 IR";
    case MtbModuleType::Univ2noIr: return "MTB-UNI v2";
    case MtbModuleType::Univ40: return "MTB-UNI v4";
    case MtbModuleType::Univ42: return "MTB-UNI v4";
    case MtbModuleType::Unis10: return "MTB-UNIS";
    case MtbModuleType::Rc: return "MTB-RC";
    default: return "Unknown type";
    }
}
