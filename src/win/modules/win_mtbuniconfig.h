#ifndef WIN_MTBUNICONFIG_H
#define WIN_MTBUNICONFIG_H

#include <array>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include "win_config.h"
#include "ui_mtbuniconfig.h"
#include "common.h"

struct UniConfigGuiInput {
    QLabel name;
    QComboBox type;
    QComboBox delay;
};

struct UniConfigGuiOutput {
    QLabel name;
    QComboBox type;
    QComboBox safeState;
};

class MtbUniConfigWindow : public MtbModuleConfigDialog {
    Q_OBJECT

public:
    MtbUniConfigWindow(QWidget *parent = nullptr);

    void editModule(const QJsonObject& module) override;
    void newModule(unsigned addr, MtbModuleType) override;
    void update(const QJsonObject& module) override;
    void retranslate() override;

private:
    Ui::MtbUniConfigWindow ui;
    std::array<UniConfigGuiInput, UNI_INPUTS_COUNT> m_guiInputs;
    std::array<UniConfigGuiOutput, UNI_OUTPUTS_COUNT> m_guiOutputs;
    bool updateInProgress = false;
    QLabel lInType;
    QLabel lInDelay;
    QLabel lOutType;
    QLabel lOutSafeState;

    void createGuiInputs();
    void createGuiOutputs();
    void updateUiType(MtbModuleType);
    void jsonParseError(const QString& err);
    void apply();

    static void fillOutputSafeState(QComboBox&, unsigned value, const QString& type);
    static int outputCbToValue(const QString& type, unsigned index);

private slots:
    void ui_bClicked(QAbstractButton *button);
    void ui_cbOutputTypeCurrentIndexChanged(int);

};

#endif // WIN_MTBUNICONFIG_H
