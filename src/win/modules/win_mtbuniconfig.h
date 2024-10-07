#ifndef WIN_MTBUNICONFIG_H
#define WIN_MTBUNICONFIG_H

#include "win_config.h"
#include "ui_mtbuniconfig.h"

class MtbUniConfigWindow : public MtbModuleConfigDialog {
    Q_OBJECT

public:
    MtbUniConfigWindow(QWidget *parent = nullptr);

    virtual void editModule(const QJsonObject& module) override;
    virtual void newModule() override;

private:
    Ui::MtbUniConfigWindow ui;
};

#endif // WIN_MTBUNICONFIG_H
