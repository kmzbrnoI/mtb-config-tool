#ifndef WIN_MTBUNISIO_H
#define WIN_MTBUNISIO_H

#include <array>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include "win_io.h"
#include "ui_mtbunisio.h"
#include "common.h"

struct UnisIOGuiInput {
    QLabel name;
    QWidget rectState;
    QLabel textState;
};

struct UnisIOGuiOutput {
    QLabel name;
    QClickableWidget rectState;
    QComboBox cbState;
    QString outputType;
};

struct UnisIOGuiServo {
    QLabel name;
    QPushButton bPlus;
    QPushButton bMinus;
    QLabel lManual;
    QSpinBox sbManual;
    QPushButton bManual;
};

enum class ServoPos {
    sPlus = 0,
    sMinus = 1,
};

class MtbUnisIOWindow : public MtbModuleIODialog {
    Q_OBJECT

public:
    MtbUnisIOWindow(QWidget *parent = nullptr);

    void openModule(const QJsonObject& module) override;
    void moduleChanged(const QJsonObject& module) override;
    void inputsChanged(const QJsonObject& module_inputs_changed) override;
    void outputsChanged(const QJsonObject& module_outputs_changed) override;
    void retranslate() override;

private:
    static constexpr unsigned RECT_WIDTH = 50;
    bool updateInProgress = false;

    Ui::MtbUnisIOWindow ui;
    std::array<UnisIOGuiInput, UNIS_INPUTS_COUNT> m_guiInputs;
    std::array<UnisIOGuiOutput, UNIS_OUTPUTS_COUNT> m_guiOutputs;
    std::array<UnisIOGuiServo, UNIS_SERVOS_COUNT> m_guiServos;

    void update(const QJsonObject& module);
    void createGuiInputs();
    void createGuiOutputs();
    void createGuiServos();
    void updateOutputType(unsigned outputi, const QString& type);
    void updateOutput(unsigned outputi, const QJsonObject& output);
    void updateInputs(const QJsonObject& inputs);
    void updateOutputs(const QJsonObject& outputs);
    void jsonParseError(const QString& err);
    void disableAll() override;
    void setOutput(unsigned output);
    void servoMove(unsigned servo, ServoPos);
    void servoOutputActivated(unsigned servo, ServoPos);
    void servoManual(uint8_t servo, uint8_t position);

    static int outputCbToValue(const QString& type, unsigned index);

private slots:
    void ui_cbOutputStateCurrentIndexChanged(int);
    void ui_wOutputClicked();
    void ui_bServoPosClicked();
    void ui_bServoManualClicked();
    void ui_bServoEndManualClicked();

};

#endif // WIN_MTBUNICONFIG_H
