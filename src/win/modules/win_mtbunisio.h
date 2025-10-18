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
    QPushButton bPosA;
    QPushButton bPosB;
    QClickableWidget rectInA;
    QClickableWidget rectInB;
};

enum class ServoPos {
    A = 0,
    B = 1,
};

class MtbUnisIOWindow : public MtbModuleIODialog {
    Q_OBJECT

    static constexpr int SERVO_MANUAL_PLUS_VALUE = 2;
    static constexpr int SERVO_MANUAL_PLUSPLUS_VALUE = 10;
    static constexpr int SERVO_MANUAL_MINUS_VALUE = -2;
    static constexpr int SERVO_MANUAL_MINUSMINUS_VALUE = -10;

public:
    MtbUnisIOWindow(QWidget *parent = nullptr);

    void openModule(const QJsonObject& module) override;
    void moduleChanged(const QJsonObject& module) override;
    void inputsChanged(const QJsonObject& module_inputs_changed) override;
    void outputsChanged(const QJsonObject& module_outputs_changed) override;
    void retranslate() override;

private:
    static constexpr unsigned RECT_WIDTH = 50;
    static constexpr unsigned SERVO_RECT_WIDTH = 30;
    static constexpr unsigned SERVO_BTN_WIDTH = 30;
    unsigned updateInProgress = 0;

    Ui::MtbUnisIOWindow ui;
    std::array<bool, UNIS_ALL_INPUTS_COUNT> m_inputsState;
    std::array<UnisIOGuiInput, UNIS_HW_INPUTS_COUNT> m_guiInputs;
    std::array<UnisIOGuiOutput, UNIS_HW_OUTPUTS_COUNT> m_guiOutputs;
    std::array<UnisIOGuiServo, UNIS_SERVOS_COUNT> m_guiServos;

    void update(const QJsonObject& module);
    void createGuiInputs();
    void createGuiOutputs();
    void createGuiServos();
    void updateOutputType(unsigned outputi, const QString& type);
    void updateOutput(unsigned outputi, const QJsonObject& output);
    void updateInputs(const QJsonObject& inputs);
    void updateOutputs(const QJsonObject& outputs);
    void disableAll() override;
    void setOutput(unsigned output);
    void servoMove(unsigned servo, ServoPos);
    void servoOutputActivated(unsigned servo, ServoPos);
    void servoManualSendPos();
    void servoManualUpdateGUI(bool enabled, bool manual);
    bool isManualPositioningActive() const;
    uint8_t currentManualServo() const;
    uint8_t servoPos(unsigned servo, ServoPos pos) const;
    void savePosToConfig(uint8_t posi, uint8_t position);
    void savePosToConfigRetrieved(QJsonObject config, uint8_t posi, uint8_t position);

    static int outputCbToValue(const QString& type, unsigned index);

private slots:
    void ui_cbOutputStateCurrentIndexChanged(int);
    void ui_wOutputClicked();
    void ui_bServoPosClicked();
    void ui_bServoManualStartClicked();
    void ui_bServoManualEndClicked();
    void ui_bServoManualPMClicked();
    void ui_bServoManualSetClicked();
    void ui_bServoManualSavePosAClicked();
    void ui_bServoManualSavePosBClicked();

};

#endif // WIN_MTBUNICONFIG_H
