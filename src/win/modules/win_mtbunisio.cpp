#include <QJsonArray>
#include <QMessageBox>
#include <QTimer>
#include "win_mtbunisio.h"
#include "client.h"
#include "qjsonsafe.h"
#include "countingguard.h"
#include <QJsonDocument>

MtbUnisIOWindow::MtbUnisIOWindow(QWidget *parent) :
    MtbModuleIODialog(parent) {
    this->ui.setupUi(this);
    this->createGuiInputs();
    this->createGuiOutputs();
    this->createGuiServos();
    this->retranslate();

    QObject::connect(ui.b_refresh, SIGNAL(released()), this, SLOT(refresh()));
    QObject::connect(ui.b_manual_start, SIGNAL(released()), this, SLOT(ui_bServoManualStartClicked()));
    QObject::connect(ui.b_manual_end, SIGNAL(released()), this, SLOT(ui_bServoManualEndClicked()));
    QObject::connect(ui.b_manual_set, SIGNAL(released()), this, SLOT(ui_bServoManualSetClicked()));
    QObject::connect(ui.b_manual_plus, SIGNAL(released()), this, SLOT(ui_bServoManualPMClicked()));
    QObject::connect(ui.b_manual_plusplus, SIGNAL(released()), this, SLOT(ui_bServoManualPMClicked()));
    QObject::connect(ui.b_manual_minus, SIGNAL(released()), this, SLOT(ui_bServoManualPMClicked()));
    QObject::connect(ui.b_manual_minusminus, SIGNAL(released()), this, SLOT(ui_bServoManualPMClicked()));
    QObject::connect(ui.b_manual_save_a, SIGNAL(released()), this, SLOT(ui_bServoManualSavePosAClicked()));
    QObject::connect(ui.b_manual_save_b, SIGNAL(released()), this, SLOT(ui_bServoManualSavePosBClicked()));
}

void MtbUnisIOWindow::createGuiInputs() {
    for (unsigned i = 0; i < UNIS_INPUTS_COUNT; i++) {
        {
            QLabel& name = this->m_guiInputs[i].name;
            name.setText(QString::number(i));
            name.setStyleSheet("font-weight: bold");
            name.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        }

        {
            QWidget& rectState = this->m_guiInputs[i].rectState;
            rectState.setFixedWidth(RECT_WIDTH);
            rectState.setStyleSheet("background-color:gray;");
            rectState.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        }

        {
            QLabel& textState = this->m_guiInputs[i].textState;
            textState.setText("?");
            textState.setAlignment(Qt::AlignCenter);
        }

        this->ui.gl_inputs->addWidget(&this->m_guiInputs[i].name, i, 0);
        this->ui.gl_inputs->addWidget(&this->m_guiInputs[i].rectState, i, 1);
        this->ui.gl_inputs->addWidget(&this->m_guiInputs[i].textState, i, 2);
    }
}

void MtbUnisIOWindow::createGuiOutputs() {
    for (unsigned i = 0; i < UNIS_OUTPUTS_COUNT; i++) {
        {
            QLabel& name = this->m_guiOutputs[i].name;
            name.setText(QString::number(i));
            name.setStyleSheet("font-weight: bold");
            name.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        }

        {
            QClickableWidget& rectState = this->m_guiOutputs[i].rectState;
            rectState.setFixedWidth(RECT_WIDTH);
            rectState.setStyleSheet("background-color:gray;");
            rectState.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        }

        {
            QComboBox& cbState = this->m_guiOutputs[i].cbState;
            cbState.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        }

        this->ui.gl_outputs->addWidget(&this->m_guiOutputs[i].name, i, 0);
        this->ui.gl_outputs->addWidget(&this->m_guiOutputs[i].rectState, i, 1);
        this->ui.gl_outputs->addWidget(&this->m_guiOutputs[i].cbState, i, 2);

        QObject::connect(&this->m_guiOutputs[i].cbState, SIGNAL(currentIndexChanged(int)), this, SLOT(ui_cbOutputStateCurrentIndexChanged(int)));
        QObject::connect(&this->m_guiOutputs[i].rectState, SIGNAL(onClicked()), this, SLOT(ui_wOutputClicked()));
    }
}

void MtbUnisIOWindow::createGuiServos() {
    for (unsigned i = 0; i < UNIS_SERVOS_COUNT; i++) {
        {
            QLabel& name = this->m_guiServos[i].name;
            name.setText(QString::number(i+1));
            name.setStyleSheet("font-weight: bold");
            name.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        }

        {
            QPushButton& bPosA = this->m_guiServos[i].bPosA;
            bPosA.setText("A");
            bPosA.setFixedWidth(RECT_WIDTH);
            bPosA.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        }

        {
            QPushButton& bPosB = this->m_guiServos[i].bPosB;
            bPosB.setText("B");
            bPosB.setFixedWidth(RECT_WIDTH);
            bPosB.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        }

        this->ui.gl_servos->addWidget(&this->m_guiServos[i].name, i, 0);
        this->ui.gl_servos->addWidget(&this->m_guiServos[i].bPosA, i, 1);
        this->ui.gl_servos->addWidget(&this->m_guiServos[i].bPosB, i, 2);

        QObject::connect(&this->m_guiServos[i].bPosA, SIGNAL(released()), this, SLOT(ui_bServoPosClicked()));
        QObject::connect(&this->m_guiServos[i].bPosB, SIGNAL(released()), this, SLOT(ui_bServoPosClicked()));
    }
}

void MtbUnisIOWindow::openModule(const QJsonObject& module) {
    this->address = QJsonSafe::safeUInt(module["address"]);
    this->disableAll();
    this->setWindowTitle(tr("IO of module ")+QString::number(this->address)+" – "+module["type"].toString());
    this->updateInProgress = 0;
    this->show();
    this->sendModuleRequest();
}

void MtbUnisIOWindow::moduleChanged(const QJsonObject& module) {
    MtbModuleIODialog::moduleChanged(module);
    this->update(module);
    this->ui.b_refresh->setEnabled(true);
}

void MtbUnisIOWindow::inputsChanged(const QJsonObject& module_inputs_changed) {
    std::array<bool, UNIS_INPUTS_COUNT> inputsLastState = this->m_inputsState;
    this->updateInputs(QJsonSafe::safeObject(module_inputs_changed["inputs"]));
    for (unsigned i = 0; i < UNIS_INPUTS_COUNT; i++)
        if (this->m_inputsState[i] != inputsLastState[i])
            this->m_guiInputs[i].textState.setStyleSheet("background-color:yellow;");
}

void MtbUnisIOWindow::outputsChanged(const QJsonObject& module_outputs_changed) {
    this->updateOutputs(QJsonSafe::safeObject(module_outputs_changed["outputs"]));
}

void MtbUnisIOWindow::update(const QJsonObject& module) {
    const QString& typeStr = QJsonSafe::safeString(module["type"]);
    const QString& stateStr = QJsonSafe::safeString(module["state"]);
    const QJsonObject& unis = QJsonSafe::safeObject(module[typeStr]);

    if (stateStr == "active") {
        if (unis.contains("state")) {
            const QJsonObject& state = QJsonSafe::safeObject(unis["state"]);
            this->updateInputs(QJsonSafe::safeObject(state["inputs"]));
            this->updateOutputs(QJsonSafe::safeObject(state["outputs"]));
            this->servoManualUpdateGUI((stateStr == "active"), this->isManualPositioningActive());
        } // else in case of module update, but state not present (e.g. config update etc.)
    } else {
        this->disableAll();
    }
}

void MtbUnisIOWindow::updateInputs(const QJsonObject& inputs) {
    const QJsonArray& inputsFull = QJsonSafe::safeArrayAtLeastSize(inputs["full"], UNIS_INPUTS_COUNT);

    for (unsigned i = 0; i < UNIS_INPUTS_COUNT; i++) {
        bool state = static_cast<int>(QJsonSafe::safeBool(inputsFull[i].toBool()));
        this->m_inputsState[i] = state;
        this->m_guiInputs[i].textState.setText(QString::number(state));
        QString color = state ? "green" : "red";
        this->m_guiInputs[i].rectState.setStyleSheet("background-color:"+color);
        this->m_guiInputs[i].textState.setStyleSheet("");
    }
}

void MtbUnisIOWindow::updateOutputs(const QJsonObject& outputs) {
    for (unsigned i = 0; i < UNIS_OUTPUTS_COUNT; i++) {
        const QJsonObject& output = QJsonSafe::safeObject(outputs[QString::number(i)]);
        this->updateOutput(i, output);
    }
    for (unsigned i = 0; i < UNIS_SERVOS_COUNT; i++) {
        if (outputs.contains(QString::number(i))) {
            this->m_guiServos[i].bPosA.setEnabled(true);
            this->m_guiServos[i].bPosB.setEnabled(true);
        }
    }
}

void MtbUnisIOWindow::updateOutputType(unsigned outputi, const QString& type) {
    CountingGuard updateGuard(this->updateInProgress);
    UnisIOGuiOutput& guiOutput = this->m_guiOutputs[outputi];
    guiOutput.outputType = type;
    guiOutput.cbState.clear();
    if ((type == "plain") || (type == "flicker")) {
        guiOutput.cbState.addItem(tr("off"));
        guiOutput.cbState.addItem(tr("on"));
        for (const FlickerDef& def : UniFlickerPerMin)
            guiOutput.cbState.addItem(tr("flick ")+def.description);
    } else if (type == "s-com") {
        for (size_t i = 0; i < SComSignalCodes.size(); i++)
            guiOutput.cbState.addItem(QString::number(i)+" - "+SComSignalCodes[i]);
    }
}

void MtbUnisIOWindow::updateOutput(unsigned outputi, const QJsonObject& output) {
    CountingGuard updateGuard(this->updateInProgress);
    const QString& newTypeStr = QJsonSafe::safeString(output["type"]);
    const unsigned value = QJsonSafe::safeUInt(output["value"]);
    UnisIOGuiOutput& guiOutput = this->m_guiOutputs[outputi];
    guiOutput.cbState.setEnabled(true);

    if (newTypeStr != guiOutput.outputType)
        this->updateOutputType(outputi, newTypeStr);

    // now we're sure that cbState contains items for new type

    if (newTypeStr == "plain") {
        guiOutput.cbState.setCurrentIndex(value);
        const QString color = (value > 0) ? "green" : "red";
        guiOutput.rectState.setStyleSheet("background-color:"+color);
    } else if (newTypeStr == "s-com") {
        guiOutput.cbState.setCurrentIndex(value);
        guiOutput.rectState.setStyleSheet("background-color:blue");
    } else if (newTypeStr == "flicker") {
        bool set = false;
        for (size_t i = 0; i < UniFlickerPerMin.size(); i++) {
            if (value == UniFlickerPerMin[i].freq) {
                guiOutput.cbState.setCurrentIndex(i+2); // +2 for "off","on"
                set = true;
            }
        }
        if (!set)
            guiOutput.cbState.setCurrentIndex(-1);
        guiOutput.rectState.setStyleSheet("background-color:blue");
    } else {
        guiOutput.cbState.setCurrentIndex(-1);
        guiOutput.rectState.setStyleSheet("background-color:blue");
    }
}

void MtbUnisIOWindow::disableAll() {
    for (UnisIOGuiInput& guiInput : this->m_guiInputs) {
        guiInput.textState.setText("?");
        guiInput.rectState.setStyleSheet("background-color:gray");
    }
    for (UnisIOGuiOutput& guiOutput : this->m_guiOutputs) {
        CountingGuard updateGuard(this->updateInProgress);
        guiOutput.cbState.setEnabled(false);
        guiOutput.cbState.setCurrentIndex(-1);
        guiOutput.rectState.setStyleSheet("background-color:gray");
    }
    for (UnisIOGuiServo& guiServo : this->m_guiServos) {
        guiServo.bPosA.setEnabled(false);
        guiServo.bPosB.setEnabled(false);
    }
    this->servoManualUpdateGUI(false, false);
}

void MtbUnisIOWindow::ui_cbOutputStateCurrentIndexChanged(int) {
    if (this->updateInProgress > 0)
        return;

    int output = -1;
    for (unsigned i = 0; i < UNIS_OUTPUTS_COUNT; i++)
        if (sender() == &this->m_guiOutputs[i].cbState)
            output = i;
    if (output == -1)
        return;

    this->setOutput(output);
}

void MtbUnisIOWindow::ui_wOutputClicked() {
    if (this->updateInProgress > 0)
        return;

    int output = -1;
    for (unsigned i = 0; i < UNIS_OUTPUTS_COUNT; i++)
        if (sender() == &this->m_guiOutputs[i].rectState)
            output = i;
    if (output == -1)
        return;

    if (output >= static_cast<int>(this->m_guiOutputs.size()))
        return;
    UnisIOGuiOutput& guiOutput = this->m_guiOutputs[output];
    if (guiOutput.outputType == "plain")
        guiOutput.cbState.setCurrentIndex((guiOutput.cbState.currentIndex() == 0) ? 1 : 0);
    else if (guiOutput.outputType == "flicker")
        guiOutput.cbState.setCurrentIndex(0);
}

void MtbUnisIOWindow::setOutput(unsigned output) {
    if (output >= this->m_guiOutputs.size())
        return;
    UnisIOGuiOutput& guiOutput = this->m_guiOutputs[output];
    const int cbIndex = guiOutput.cbState.currentIndex();
    if (cbIndex < 0)
        return;

    QJsonObject outputJson;
    if ((guiOutput.outputType == "plain") || (guiOutput.outputType == "flicker")) {
        if (cbIndex < 2) {
            outputJson["type"] = "plain";
            outputJson["value"] = cbIndex;
        } else {
            const unsigned flickerIndex = cbIndex-2;
            outputJson["type"] = "flicker";
            if (flickerIndex >= UniFlickerPerMin.size())
                return;
            outputJson["value"] = static_cast<int>(UniFlickerPerMin[flickerIndex].freq);
        }
    } else if (guiOutput.outputType == "s-com") {
        outputJson["type"] = "s-com";
        outputJson["value"] = guiOutput.cbState.currentIndex();
    } else {
        return;
    }

    guiOutput.rectState.setStyleSheet("background-color:yellow");

    DaemonClient::instance->sendNoExc(
        {
            {"command", "module_set_outputs"},
            {"address", this->address},
            {"outputs", QJsonObject({{QString::number(output), outputJson}})},
        },
        [this](const QJsonObject& content) {
            this->outputsChanged(content);
        },
        [this](unsigned errorCode, QString errorMessage) {
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("setOutput", errorCode, errorMessage));
        }
    );
}

void MtbUnisIOWindow::retranslate() {
    this->ui.retranslateUi(this);
}

void MtbUnisIOWindow::ui_bServoPosClicked() {
    int servo = -1;
    ServoPos pos;
    for (unsigned i = 0; i < UNIS_OUTPUTS_COUNT; i++) {
        if (sender() == &this->m_guiServos[i].bPosA) {
            servo = i;
            pos = ServoPos::A;
        }
        if (sender() == &this->m_guiServos[i].bPosB) {
            servo = i;
            pos = ServoPos::B;
        }
    }
    if (servo != -1) {
        this->servoMove(servo, pos);
        if (this->ui.cb_servo_manual->isEnabled()) {
            this->ui.cb_servo_manual->setCurrentIndex(servo);
            this->ui.sb_manual_pos->setValue(this->servoPos(servo, pos));
        }
    }
}

void MtbUnisIOWindow::servoMove(unsigned servo, ServoPos pos) {
    const unsigned output = UNIS_OUTPUTS_COUNT + (2*servo) + (pos == ServoPos::B ? 1 : 0);

    this->m_guiServos[servo].bPosA.setEnabled(false);
    this->m_guiServos[servo].bPosB.setEnabled(false);

    QJsonObject outputJson{
        {"type", "plain"},
        {"value", 1},
    };
    DaemonClient::instance->sendNoExc(
        {
            {"command", "module_set_outputs"},
            {"address", this->address},
            {"outputs", QJsonObject({{QString::number(output), outputJson}})},
        },
        [this, servo, pos](const QJsonObject&) {
            QTimer::singleShot(100, [this, servo, pos]() { this->servoOutputActivated(servo, pos); });
        },
        [this, servo](unsigned errorCode, QString errorMessage) {
            this->m_guiServos[servo].bPosA.setEnabled(true);
            this->m_guiServos[servo].bPosB.setEnabled(true);
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("setOutput", errorCode, errorMessage));
        }
    );
}

void MtbUnisIOWindow::servoOutputActivated(unsigned servo, ServoPos pos) {
    // Output activated -> deactivate
    const unsigned output = UNIS_OUTPUTS_COUNT + (2*servo) + (pos == ServoPos::B ? 1 : 0);

    QJsonObject outputJson{
        {"type", "plain"},
        {"value", 0},
    };
    DaemonClient::instance->sendNoExc(
        {
            {"command", "module_set_outputs"},
            {"address", this->address},
            {"outputs", QJsonObject({{QString::number(output), outputJson}})},
        },
        [this, servo](const QJsonObject&) {
            this->m_guiServos[servo].bPosA.setEnabled(true);
            this->m_guiServos[servo].bPosB.setEnabled(true);
        },
        [this, servo](unsigned errorCode, QString errorMessage) {
            this->m_guiServos[servo].bPosA.setEnabled(true);
            this->m_guiServos[servo].bPosB.setEnabled(true);
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("setOutput", errorCode, errorMessage));
        }
    );
}

bool MtbUnisIOWindow::isManualPositioningActive() const {
    return this->ui.b_manual_end->isEnabled();
}

uint8_t MtbUnisIOWindow::currentManualServo() const {
    return this->ui.cb_servo_manual->currentIndex();
}

void MtbUnisIOWindow::ui_bServoManualStartClicked() {
    this->servoManualUpdateGUI(true, true);
    this->servoManualSendPos();
}

void MtbUnisIOWindow::ui_bServoManualEndClicked() {
    QJsonArray data{3, 0};
    this->servoManualUpdateGUI(true, false);
    this->ui.b_manual_start->setEnabled(false);

    DaemonClient::instance->sendNoExc(
        {
            {"command", "module_specific_command"},
            {"address", this->address},
            {"data", data},
        },
        [this](const QJsonObject&) {
            this->ui.b_manual_start->setEnabled(true);
        },
        [this](unsigned errorCode, QString errorMessage) {
            this->ui.b_manual_start->setEnabled(true);
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("module_specific_command", errorCode, errorMessage));
        }
    );
}

void MtbUnisIOWindow::ui_bServoManualSetClicked() {
    this->servoManualSendPos();
}

void MtbUnisIOWindow::ui_bServoManualPMClicked() {
    int increment = 0;
    if (QObject::sender() == this->ui.b_manual_plus)
        increment = SERVO_MANUAL_PLUS_VALUE;
    else if (QObject::sender() == this->ui.b_manual_plusplus)
        increment = SERVO_MANUAL_PLUSPLUS_VALUE;
    else if (QObject::sender() == this->ui.b_manual_minus)
        increment = SERVO_MANUAL_MINUS_VALUE;
    else if (QObject::sender() == this->ui.b_manual_minusminus)
        increment = SERVO_MANUAL_MINUSMINUS_VALUE;

    this->ui.sb_manual_pos->setValue(this->ui.sb_manual_pos->value() + increment);
    this->servoManualSendPos();
}

void MtbUnisIOWindow::servoManualSendPos() {
    const uint8_t position = this->ui.sb_manual_pos->value();
    QJsonArray data{3, (this->currentManualServo()+1) << 1, position};
    this->ui.b_manual_set->setEnabled(false);

    DaemonClient::instance->sendNoExc(
        {
            {"command", "module_specific_command"},
            {"address", this->address},
            {"data", data},
        },
        [this](const QJsonObject&) {
            this->ui.b_manual_set->setEnabled(true);
        },
        [this](unsigned errorCode, QString errorMessage) {
            this->ui.b_manual_set->setEnabled(true);
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("module_specific_command", errorCode, errorMessage));
        }
    );
}

void MtbUnisIOWindow::servoManualUpdateGUI(bool enabled, bool manual) {
    this->ui.cb_servo_manual->setEnabled(enabled && !manual);
    this->ui.b_manual_start->setEnabled(enabled && !manual);
    this->ui.b_manual_end->setEnabled(enabled && manual);
    this->ui.b_manual_set->setEnabled(enabled && manual);
    this->ui.sb_manual_pos->setEnabled(enabled && manual);
    this->ui.b_manual_plus->setEnabled(enabled && manual);
    this->ui.b_manual_plusplus->setEnabled(enabled && manual);
    this->ui.b_manual_minus->setEnabled(enabled && manual);
    this->ui.b_manual_minusminus->setEnabled(enabled && manual);
    this->ui.b_manual_save_a->setEnabled(enabled && manual);
    this->ui.b_manual_save_b->setEnabled(enabled && manual);

    if ((!enabled) || (!manual)) {
        this->ui.sb_manual_pos->setValue(0);
    }
    if (!enabled) {
        this->ui.cb_servo_manual->setCurrentIndex(0);
    }
}

uint8_t MtbUnisIOWindow::servoPos(unsigned servo, ServoPos pos) const {
    try {
        const QJsonArray& positions = QJsonSafe::safeArray(this->m_config["servoPosition"], 2*UNIS_SERVOS_COUNT);
        return QJsonSafe::safeUInt(positions[2*servo + static_cast<int>(pos)]);
    } catch (QJsonSafe::JsonParseError& e) {
        return 0;
    }
}

void MtbUnisIOWindow::ui_bServoManualSavePosAClicked() {
    this->savePosToConfig(2*this->currentManualServo(), this->ui.sb_manual_pos->value());
}

void MtbUnisIOWindow::ui_bServoManualSavePosBClicked() {
    this->savePosToConfig(2*this->currentManualServo()+1, this->ui.sb_manual_pos->value());
}

void MtbUnisIOWindow::savePosToConfig(uint8_t posi, uint8_t position) {
    // First retrieve module config
    DaemonClient::instance->sendNoExc(
        {{"command", "module"}, {"address", this->address}},
        [this, posi, position](const QJsonObject& content) {
            try {
                const QJsonObject& module = QJsonSafe::safeObject(content["module"]);
                const QJsonObject& moduleSpec = QJsonSafe::safeObject(module[QJsonSafe::safeString(module["type"])]);
                this->savePosToConfigRetrieved(QJsonSafe::safeObject(moduleSpec["config"]), posi, position);
            } catch (const QStrException& e) {
                QMessageBox::critical(this, "Error", "Module receive config failed: "+e.str());
            }
        },
        [this](unsigned errorCode, QString errorMessage) {
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("module", errorCode, errorMessage));
        }
    );
}

void MtbUnisIOWindow::savePosToConfigRetrieved(QJsonObject config, uint8_t posi, uint8_t position)
{
    try {
        QJsonArray positions = QJsonSafe::safeArray(config["servoPosition"], 2*UNIS_SERVOS_COUNT);
        positions[posi] = position;
        config["servoPosition"] = positions;

        DaemonClient::instance->sendNoExc(
            {
                {"command", "module_set_config"},
                {"address", this->address},
                {"config", config}
            },
            [this](const QJsonObject& content) {
                (void)content;
                QMessageBox::information(this, tr("Ok"), tr("Configuration successfully sent to module."));
            },
            [this](unsigned errorCode, QString errorMessage) {
                QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("module_set_config", errorCode, errorMessage));
            }
        );
    } catch (const QStrException& e) {
        QMessageBox::critical(this, "Error", "savePosToConfigRetrieved failed: "+e.str());
    }
}
