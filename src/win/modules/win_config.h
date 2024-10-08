#ifndef WIN_CONFIG_H
#define WIN_CONFIG_H

#include <QDialog>
#include <QJsonObject>
#include "common.h"

class MtbModuleConfigDialog : public QDialog {
    Q_OBJECT

public:
    MtbModuleConfigDialog(QWidget *parent = nullptr) : QDialog(parent) {}

    virtual void editModule(const QJsonObject& module) = 0;
    virtual void newModule(unsigned addr, MtbModuleType) = 0;
    virtual void update(const QJsonObject& module) = 0;

protected:
    uint8_t address;

protected slots:
    void refresh();

};

#endif // WIN_MTBUNICONFIG_H
