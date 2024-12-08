#ifndef WIN_IO_H
#define WIN_IO_H

#include <QDialog>
#include <QJsonObject>

class MtbModuleIODialog : public QDialog {
    Q_OBJECT

public:
    MtbModuleIODialog(QWidget *parent = nullptr) : QDialog(parent) {}

    virtual void openModule(const QJsonObject& module) = 0;
    virtual void moduleChanged(const QJsonObject& module) = 0;
    virtual void inputsChanged(const QJsonObject& module) = 0;
    virtual void outputsChanged(const QJsonObject& module) = 0;
    virtual void retranslate() = 0;

protected:
    uint8_t address;

    void sendModuleRequest();

};

#endif // WIN_MTBUNICONFIG_H
