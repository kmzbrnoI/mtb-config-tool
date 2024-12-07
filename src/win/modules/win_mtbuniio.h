#ifndef WIN_MTBUNIIO_H
#define WIN_MTBUNIIO_H

#include <array>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include "win_io.h"
#include "ui_mtbuniio.h"
#include "common.h"

struct UniIOGuiInput {
    QLabel name;
    QComboBox type;
    QComboBox delay;
};

struct UniIOGuiOutput {
    QLabel name;
    QComboBox type;
    QComboBox safeState;
};

class MtbUniIOWindow : public MtbModuleIODialog {
    Q_OBJECT

public:
    MtbUniIOWindow(QWidget *parent = nullptr);

    void openModule(const QJsonObject& module) override;
    void moduleChanged(const QJsonObject& module) override;
    void inputsChanged(const QJsonObject& module) override;
    void outputsChanged(const QJsonObject& module) override;
    void retranslate() override;

private:
    Ui::MtbUniIOWindow ui;
    std::array<UniIOGuiInput, UNI_INPUTS_COUNT> m_guiInputs;
    std::array<UniIOGuiOutput, UNI_OUTPUTS_COUNT> m_guiOutputs;
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
