#ifndef WIN_MTBUNISCONFIG_H
#define WIN_MTBUNISCONFIG_H

#include <array>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include "win_config.h"
#include "ui_mtbunisconfig.h"
#include "common.h"

struct UnisGuiInput {
    QLabel name;
    QComboBox delay;
};

struct UnisGuiOutput {
    QLabel name;
    QLabel typeLabel;
    QComboBox type;
    QLabel safeStateLabel;
    QComboBox safeState;
};

class MtbUnisConfigWindow : public MtbModuleConfigDialog {
    Q_OBJECT

public:
    MtbUnisConfigWindow(QWidget *parent = nullptr);

    virtual void editModule(const QJsonObject& module) override;
    virtual void newModule(unsigned addr, MtbModuleType) override;
    virtual void update(const QJsonObject& module) override;
    virtual void retranslate() override;

private:
    Ui::MtbUnisConfigWindow ui;
    std::array<UnisGuiInput, UNIS_INPUTS_COUNT> m_guiInputs;
    std::array<UnisGuiOutput, UNIS_OUTPUTS_COUNT> m_guiOutputs;
    bool updateInProgress = false;
    QLabel lInDelay;
    QLabel lOutType;
    QLabel lOutSafeState;

    void createGuiInputs();
    void createGuiOutputs();
    void jsonParseError(const QString& err);
    void apply();

    static void fillOutputSafeState(QComboBox&, unsigned value, const QString& type);
    static int outputCbToValue(const QString& type, unsigned index);

private slots:
    void ui_bClicked(QAbstractButton *button);
    void ui_cbOutputTypeCurrentIndexChanged(int);

};

#endif // WIN_MTBUNISCONFIG_H
