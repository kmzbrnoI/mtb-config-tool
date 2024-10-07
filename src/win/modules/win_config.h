#ifndef WIN_CONFIG_H
#define WIN_CONFIG_H

#include <QDialog>
#include <QJsonObject>

class MtbModuleConfigDialog : public QDialog {
    Q_OBJECT

public:
    MtbModuleConfigDialog(QWidget *parent = nullptr) : QDialog(parent) {}

    virtual void editModule(const QJsonObject& module) = 0;
    virtual void newModule() = 0;

};

#endif // WIN_MTBUNICONFIG_H
