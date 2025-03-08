#ifndef WIN_MTBUNISCONFIG_H
#define WIN_MTBUNISCONFIG_H

#include <array>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include "win_config.h"
#include "ui_mtbunisconfig.h"
#include "common.h"

struct UnisConfigGuiInput {
    QLabel name;
    QComboBox delay;
};

struct UnisConfigGuiOutput {
    QLabel name;
    QComboBox type;
    QComboBox safeState;
};

struct UnisConfigGuiServo {
    QLabel name;
    QCheckBox enabled;
    QSpinBox posPlus;
    QSpinBox posMinus;
    QSpinBox speed;
    QComboBox posSensors;
};

class MtbUnisConfigWindow : public MtbModuleConfigDialog {
    Q_OBJECT

public:
    MtbUnisConfigWindow(QWidget *parent = nullptr);

    void editModule(const QJsonObject& module) override;
    void newModule(unsigned addr, MtbModuleType) override;
    void update(const QJsonObject& module) override;
    void retranslate() override;

private:
    Ui::MtbUnisConfigWindow ui;
    std::array<UnisConfigGuiInput, UNIS_INPUTS_COUNT> m_guiInputs;
    std::array<UnisConfigGuiOutput, UNIS_OUTPUTS_COUNT> m_guiOutputs;
    std::array<UnisConfigGuiServo, UNIS_SERVOS_COUNT> m_guiServos;
    bool updateInProgress = false;
    QLabel lInDelay;
    QLabel lOutType;
    QLabel lOutSafeState;
    QLabel lServoEnabled;
    QLabel lServoPlus;
    QLabel lServoMinus;
    QLabel lServoSpeed;
    QLabel lServoPosSensors;

    void createGuiInputs();
    void createGuiOutputs();
    void createGuiServos();
    void jsonParseError(const QString& err);
    void apply();

    static void fillOutputSafeState(QComboBox&, unsigned value, const QString& type);
    static int outputCbToValue(const QString& type, unsigned index);

private slots:
    void ui_bClicked(QAbstractButton *button);
    void ui_cbOutputTypeCurrentIndexChanged(int);

};

#endif // WIN_MTBUNISCONFIG_H
