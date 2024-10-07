#ifndef WIN_MTBUNICONFIG_H
#define WIN_MTBUNICONFIG_H

#include <array>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include "win_config.h"
#include "ui_mtbuniconfig.h"
#include "common.h"

struct UniGuiInput {
    QLabel name;
    QLabel typeLabel;
    QComboBox type;
    QLabel delayLabel;
    QComboBox delay;
};

struct UniGuiOutput {
    QLabel name;
    QLabel typeLabel;
    QComboBox type;
    QLabel safeStateLabel;
    QComboBox safeState;
};

class MtbUniConfigWindow : public MtbModuleConfigDialog {
    Q_OBJECT

public:
    MtbUniConfigWindow(QWidget *parent = nullptr);

    virtual void editModule(const QJsonObject& module) override;
    virtual void newModule() override;

private:
    Ui::MtbUniConfigWindow ui;
    std::array<UniGuiInput, UNI_INPUTS_COUNT> m_guiInputs;
    std::array<UniGuiOutput, UNI_INPUTS_COUNT> m_guiOutputs;

    void createGuiInputs();
    void createGuiOutputs();
};

#endif // WIN_MTBUNICONFIG_H
