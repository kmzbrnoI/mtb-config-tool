#include <QJsonArray>
#include <QMessageBox>
#include <QTimer>
#include "win_mtbunisio.h"
#include "client.h"
#include "qjsonsafe.h"
#include "countingguard.h"

MtbUnisIOWindow::MtbUnisIOWindow(QWidget *parent) :
    MtbModuleIODialog(parent) {
    this->ui.setupUi(this);
    this->setFixedSize(this->width(), this->height());
    this->createGuiInputs();
    this->createGuiOutputs();
    this->createGuiServos();
    this->retranslate();

    QObject::connect(ui.b_refresh, SIGNAL(released()), this, SLOT(refresh()));
    QObject::connect(ui.b_end_manual, SIGNAL(released()), this, SLOT(ui_bServoEndManualClicked()));
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
            QPushButton& bPlus = this->m_guiServos[i].bPlus;
            bPlus.setText("+");
            bPlus.setFixedWidth(RECT_WIDTH);
            bPlus.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        }

        {
            QPushButton& bMinus = this->m_guiServos[i].bMinus;
            bMinus.setText("-");
            bMinus.setFixedWidth(RECT_WIDTH);
            bMinus.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        }

        this->m_guiServos[i].lManual.setText(tr("Manual position:"));

        {
            QSpinBox& sbManual = this->m_guiServos[i].sbManual;
            sbManual.setMinimum(0);
            sbManual.setMaximum(255);
        }

        this->m_guiServos[i].bManual.setText(tr("Set manual position"));

        this->ui.gl_servos->addWidget(&this->m_guiServos[i].name, i, 0);
        this->ui.gl_servos->addWidget(&this->m_guiServos[i].bPlus, i, 1);
        this->ui.gl_servos->addWidget(&this->m_guiServos[i].bMinus, i, 2);
        this->ui.gl_servos->addWidget(&this->m_guiServos[i].lManual, i, 3);
        this->ui.gl_servos->addWidget(&this->m_guiServos[i].sbManual, i, 4);
        this->ui.gl_servos->addWidget(&this->m_guiServos[i].bManual, i, 5);

        QObject::connect(&this->m_guiServos[i].bPlus, SIGNAL(released()), this, SLOT(ui_bServoPosClicked()));
        QObject::connect(&this->m_guiServos[i].bMinus, SIGNAL(released()), this, SLOT(ui_bServoPosClicked()));
        QObject::connect(&this->m_guiServos[i].bManual, SIGNAL(released()), this, SLOT(ui_bServoManualClicked()));
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
    const QJsonObject& uni = QJsonSafe::safeObject(module[typeStr]);
    if (uni.contains("state")) {
        const QJsonObject& state = QJsonSafe::safeObject(uni["state"]);
        this->updateInputs(QJsonSafe::safeObject(state["inputs"]));
        this->updateOutputs(QJsonSafe::safeObject(state["outputs"]));
    } else {
        this->disableAll();
    }
}

void MtbUnisIOWindow::updateInputs(const QJsonObject& inputs) {
    const QJsonArray& inputsFull = QJsonSafe::safeArray(inputs["full"], UNIS_INPUTS_COUNT);

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
            this->m_guiServos[i].bPlus.setEnabled(true);
            this->m_guiServos[i].bMinus.setEnabled(true);
            this->m_guiServos[i].sbManual.setEnabled(true);
            this->m_guiServos[i].bManual.setEnabled(true);
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
        guiServo.bPlus.setEnabled(false);
        guiServo.bMinus.setEnabled(false);
        guiServo.sbManual.setEnabled(false);
        guiServo.bManual.setEnabled(false);
    }
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
        if (sender() == &this->m_guiServos[i].bPlus) {
            servo = i;
            pos = ServoPos::sPlus;
        }
        if (sender() == &this->m_guiServos[i].bMinus) {
            servo = i;
            pos = ServoPos::sMinus;
        }
    }
    if (servo != -1)
        this->servoMove(servo, pos);
}

void MtbUnisIOWindow::servoMove(unsigned servo, ServoPos pos) {
    const unsigned output = UNIS_OUTPUTS_COUNT + (2*servo) + (pos == ServoPos::sMinus ? 1 : 0);

    this->m_guiServos[servo].bPlus.setEnabled(false);
    this->m_guiServos[servo].bMinus.setEnabled(false);

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
            this->m_guiServos[servo].bPlus.setEnabled(true);
            this->m_guiServos[servo].bMinus.setEnabled(true);
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("setOutput", errorCode, errorMessage));
        }
    );
}

void MtbUnisIOWindow::servoOutputActivated(unsigned servo, ServoPos pos) {
    // Output activated -> deactivate
    const unsigned output = UNIS_OUTPUTS_COUNT + (2*servo) + (pos == ServoPos::sMinus ? 1 : 0);

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
            this->m_guiServos[servo].bPlus.setEnabled(true);
            this->m_guiServos[servo].bMinus.setEnabled(true);
        },
        [this, servo](unsigned errorCode, QString errorMessage) {
            this->m_guiServos[servo].bPlus.setEnabled(true);
            this->m_guiServos[servo].bMinus.setEnabled(true);
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("setOutput", errorCode, errorMessage));
        }
    );
}

void MtbUnisIOWindow::ui_bServoManualClicked() {
    int servo = -1;
    for (unsigned i = 0; i < UNIS_OUTPUTS_COUNT; i++) {
        if (sender() == &this->m_guiServos[i].bManual)
            servo = i;
    }
    if (servo != -1)
        this->servoManual(servo, this->m_guiServos[servo].sbManual.value());
}

void MtbUnisIOWindow::servoManual(uint8_t servo, uint8_t position) {
    QJsonArray data{3, (servo+1) << 1, position};

    DaemonClient::instance->sendNoExc(
        {
            {"command", "module_specific_command"},
            {"address", this->address},
            {"data", data},
        },
        [this](const QJsonObject&) {
            QMessageBox::information(this, tr("Information"), tr("Ok."));
        },
        [this](unsigned errorCode, QString errorMessage) {
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("module_specific_command", errorCode, errorMessage));
        }
    );
}

void MtbUnisIOWindow::ui_bServoEndManualClicked() {
    QJsonArray data{3, 0};

    DaemonClient::instance->sendNoExc(
        {
            {"command", "module_specific_command"},
            {"address", this->address},
            {"data", data},
        },
        [this](const QJsonObject&) {
            QMessageBox::information(this, tr("Information"), tr("Manual positioning successfully ended."));
        },
        [this](unsigned errorCode, QString errorMessage) {
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("module_specific_command", errorCode, errorMessage));
        }
    );
}
