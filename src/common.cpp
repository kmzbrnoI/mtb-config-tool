#include <QJsonArray>
#include "common.h"

void MtbUsbStatus::update(const QJsonObject& json) {
    this->type = json["type"].toInt();
    this->speed = json["speed"].toInt();
    this->firmware_version = json["firmware_version"].toString();
    this->protocol_version = json["protocol_version"].toString();

    this->activeModules.clear();
    for (const QJsonValue& addr : json["active_modules"].toArray())
        this->activeModules.append(addr.toInt());
}

void setBacground(QTreeWidgetItem& item, const QBrush& brush) {
    for (int i = 0; i < item.columnCount(); i++)
        item.setBackground(i, brush);
}
