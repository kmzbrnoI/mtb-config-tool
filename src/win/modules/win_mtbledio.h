#ifndef WIN_MTBLEDIO_H
#define WIN_MTBLEDIO_H

#include <array>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include "win_io.h"
#include "ui_mtbledio.h"
#include "common.h"

struct LedIOGuiInput {
    QLabel name;
    QWidget rectState;
    QLabel textState;
};

struct LedIOGuiOutput {
    QLabel name;
    QClickableWidget rectState;
    QLabel textState;
};

class MtbLedIOWindow : public MtbModuleIODialog {
    Q_OBJECT

public:
    MtbLedIOWindow(QWidget *parent = nullptr);

    void openModule(const QJsonObject& module) override;
    void moduleChanged(const QJsonObject& module) override;
    void inputsChanged(const QJsonObject& module_inputs_changed) override;
    void outputsChanged(const QJsonObject& module_outputs_changed) override;
    void retranslate() override;

private:
    static constexpr unsigned RECT_WIDTH = 50;
    unsigned updateInProgress = 0;

    Ui::MtbLedIOWindow ui;
    std::array<bool, LED_IO_COUNT> m_inputsState;
    std::array<LedIOGuiInput, LED_IO_COUNT> m_guiInputs;
    std::array<LedIOGuiOutput, LED_IO_COUNT> m_guiOutputs;

    void update(const QJsonObject& module);
    void createGuiInputs();
    void createGuiOutputs();
    void updateInputs(const QJsonObject& inputs);
    void updateOutputs(const QJsonObject& outputs);
    void disableAll() override;

    static int outputCbToValue(const QString& type, unsigned index);

private slots:
    void ui_wOutputClicked();

};

#endif // WIN_MTBUNICONFIG_H
