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
    QWidget rectState;
    QLabel textState;
};

struct UniIOGuiOutput {
    QLabel name;
    QClickableWidget rectState;
    QComboBox cbState;
    QString outputType;
};

class MtbUniIOWindow : public MtbModuleIODialog {
    Q_OBJECT

public:
    MtbUniIOWindow(QWidget *parent = nullptr);

    void openModule(const QJsonObject& module) override;
    void moduleChanged(const QJsonObject& module) override;
    void inputsChanged(const QJsonObject& module_inputs_changed) override;
    void outputsChanged(const QJsonObject& module_outputs_changed) override;
    void retranslate() override;

private:
    static constexpr unsigned RECT_WIDTH = 50;
    unsigned updateInProgress = 0;

    Ui::MtbUniIOWindow ui;
    std::array<bool, UNI_INPUTS_COUNT> m_inputsState;
    std::array<UniIOGuiInput, UNI_INPUTS_COUNT> m_guiInputs;
    std::array<UniIOGuiOutput, UNI_OUTPUTS_COUNT> m_guiOutputs;

    void update(const QJsonObject& module);
    void createGuiInputs();
    void createGuiOutputs();
    void updateOutputType(unsigned outputi, const QString& type);
    void updateOutput(unsigned outputi, const QJsonObject& output);
    void updateInputs(const QJsonObject& inputs);
    void updateOutputs(const QJsonObject& outputs);
    void disableAll() override;
    void setOutput(unsigned output);

    static int outputCbToValue(const QString& type, unsigned index);

private slots:
    void ui_cbOutputStateCurrentIndexChanged(int);
    void ui_wOutputClicked();

};

#endif // WIN_MTBUNICONFIG_H
