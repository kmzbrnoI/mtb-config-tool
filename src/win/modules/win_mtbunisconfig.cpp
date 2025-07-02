#include <QJsonArray>
#include <QMessageBox>
#include "win_mtbunisconfig.h"
#include "client.h"
#include "qjsonsafe.h"

MtbUnisConfigWindow::MtbUnisConfigWindow(QWidget *parent) :
    MtbModuleConfigDialog(parent) {
    this->ui.setupUi(this);
    this->setFixedSize(this->width(), this->height());
    this->createGuiInputs();
    this->createGuiOutputs();
    this->createGuiServos();
    this->retranslate();

    QPushButton *b = this->ui.bb_main->button(QDialogButtonBox::Apply);
    if (b)
        b->setDefault(true);

    QObject::connect(ui.b_refresh, SIGNAL(released()), this, SLOT(refresh()));
    QObject::connect(ui.bb_main, SIGNAL(clicked(QAbstractButton*)), this, SLOT(ui_bClicked(QAbstractButton*)));
}

void MtbUnisConfigWindow::createGuiInputs() {
    this->ui.gl_inputs->addWidget(&this->lInDelay, 0, 1);

    for (unsigned i = 0; i < UNIS_INPUTS_COUNT; i++) {
        QLabel& name = this->m_guiInputs[i].name;
        name.setText(QString::number(i));
        name.setStyleSheet("font-weight: bold");
        name.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

        {
            QComboBox& delay = this->m_guiInputs[i].delay;
            delay.addItems({"0 s", "0.1 s", "0.2 s", "0.3 s", "0.4 s", "0.5 s", "0.6 s", "0.7 s",
                            "0.8 s", "0.9 s", "1.0 s", "1.1 s", "1.2 s", "1.3 s", "1.4 s", "1.5 s"});
        }

        this->ui.gl_inputs->addWidget(&this->m_guiInputs[i].name, i+1, 0);
        this->ui.gl_inputs->addWidget(&this->m_guiInputs[i].delay, i+1, 1);
    }
}

void MtbUnisConfigWindow::createGuiOutputs() {
    this->ui.gl_outputs->addWidget(&this->lOutType, 0, 1);
    this->ui.gl_outputs->addWidget(&this->lOutSafeState, 0, 2);

    for (unsigned i = 0; i < UNIS_OUTPUTS_COUNT; i++) {
        QLabel& name = this->m_guiOutputs[i].name;
        name.setText(QString::number(i));
        name.setStyleSheet("font-weight: bold");
        name.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

        {
            QComboBox& type = this->m_guiOutputs[i].type;
            type.addItems({"plain", "s-com", "flicker"});
        }

        {
            QComboBox& safeState = this->m_guiOutputs[i].safeState;
            safeState.addItems({tr("off"), tr("on")});
            safeState.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        }

        this->ui.gl_outputs->addWidget(&this->m_guiOutputs[i].name, i+1, 0);
        this->ui.gl_outputs->addWidget(&this->m_guiOutputs[i].type, i+1, 1);
        this->ui.gl_outputs->addWidget(&this->m_guiOutputs[i].safeState, i+1, 2);

        QObject::connect(&this->m_guiOutputs[i].type, SIGNAL(currentIndexChanged(int)), this, SLOT(ui_cbOutputTypeCurrentIndexChanged(int)));
    }
}

void MtbUnisConfigWindow::createGuiServos() {
    this->ui.gl_servos->addWidget(&this->lServoEnabled, 0, 1);
    this->ui.gl_servos->addWidget(&this->lServoPosA, 0, 2);
    this->ui.gl_servos->addWidget(&this->lServoPosB, 0, 3);
    this->ui.gl_servos->addWidget(&this->lServoSpeed, 0, 4);
    this->ui.gl_servos->addWidget(&this->lServoPosSensors, 0, 5);

    for (unsigned i = 0; i < UNIS_SERVOS_COUNT; i++) {
        QLabel& name = this->m_guiServos[i].name;
        name.setText(QString::number(i+1));
        name.setStyleSheet("font-weight: bold");
        name.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

        {
            QSpinBox& posA = this->m_guiServos[i].posA;
            posA.setMinimum(0);
            posA.setMaximum(255);
        }

        {
            QSpinBox& posB = this->m_guiServos[i].posB;
            posB.setMinimum(0);
            posB.setMaximum(255);
        }

        {
            QSpinBox& speed = this->m_guiServos[i].speed;
            speed.setMinimum(0);
            speed.setMaximum(255);
        }

        {
            QComboBox& inputs = this->m_guiServos[i].posSensors;
            inputs.addItem(tr("No sensors"));
            for (size_t i = 0; i < UNI_INPUTS_COUNT; i++)
                inputs.addItem(QString::number(i) + "+, " + QString::number(i+1) + "-");
        }

        this->ui.gl_servos->addWidget(&this->m_guiServos[i].name, i+1, 0);
        this->ui.gl_servos->addWidget(&this->m_guiServos[i].enabled, i+1, 1);
        this->ui.gl_servos->addWidget(&this->m_guiServos[i].posA, i+1, 2);
        this->ui.gl_servos->addWidget(&this->m_guiServos[i].posB, i+1, 3);
        this->ui.gl_servos->addWidget(&this->m_guiServos[i].speed, i+1, 4);
        this->ui.gl_servos->addWidget(&this->m_guiServos[i].posSensors, i+1, 5);
    }
}

void MtbUnisConfigWindow::editModule(const QJsonObject& module) {
    this->creatingNewModule = false;
    this->update(module);
    this->show();
}

void MtbUnisConfigWindow::update(const QJsonObject& module) {
    this->address = QJsonSafe::safeUInt(module["address"]);
    this->ui.le_name->setText(QJsonSafe::safeString(module["name"]));
    this->type = static_cast<MtbModuleType>(QJsonSafe::safeUInt(module["type_code"]));

    const QString& typeStr = QJsonSafe::safeString(module["type"]);
    const QJsonObject& unis = QJsonSafe::safeObject(module[typeStr]);
    const QJsonObject& config = QJsonSafe::safeObject(unis["config"]);

    // Inputs
    const QJsonArray& inputsDelay = QJsonSafe::safeArray(config["inputsDelay"], UNIS_INPUTS_COUNT);
    for (unsigned i = 0; i < UNIS_INPUTS_COUNT; i++) {
        unsigned delay = static_cast<int>(QJsonSafe::safeDouble(inputsDelay[i].toDouble()) * 10);
        this->m_guiInputs[i].delay.setCurrentIndex(delay);
    }

    // Outputs
    const QJsonArray& outputs = QJsonSafe::safeArray(config["outputsSafe"], UNIS_OUTPUTS_COUNT+(2*UNIS_SERVOS_COUNT));
    for (unsigned i = 0; i < UNIS_OUTPUTS_COUNT; i++) { // TODO: what to do with outputsSafe[16-27]?
        const QJsonObject& output = QJsonSafe::safeObject(outputs[i]);
        const QString& type = QJsonSafe::safeString(output["type"]);
        const unsigned value = QJsonSafe::safeUInt(output["value"]);

        this->updateInProgress = true;
        this->m_guiOutputs[i].type.setCurrentText(type);
        MtbUnisConfigWindow::fillOutputSafeState(this->m_guiOutputs[i].safeState, value, type);
        this->updateInProgress = false;
    }

    // Servos
    {
        unsigned enabledMask = QJsonSafe::safeUInt(config["servoEnabledMask"]);
        const QJsonArray& positions = QJsonSafe::safeArray(config["servoPosition"], 2*UNIS_SERVOS_COUNT);
        const QJsonArray& speeds = QJsonSafe::safeArray(config["servoSpeed"], UNIS_SERVOS_COUNT);
        const QJsonArray& sensors = config["servoInputMap"].toArray(); // allow servoInputMap to be missing (for older MTB Daemon)

        for (unsigned i = 0; i < UNIS_SERVOS_COUNT; i++) {
            this->m_guiServos[i].enabled.setChecked(static_cast<bool>(enabledMask & (1<<i)));
            this->m_guiServos[i].posA.setValue(QJsonSafe::safeUInt(positions[2*i]));
            this->m_guiServos[i].posB.setValue(QJsonSafe::safeUInt(positions[2*i+1]));
            this->m_guiServos[i].speed.setValue(QJsonSafe::safeUInt(speeds[i]));

            this->m_guiServos[i].posSensors.setEnabled(i < sensors.size());
            this->m_guiServos[i].posSensors.setCurrentIndex(i < sensors.size() ? QJsonSafe::safeUInt(sensors[i]) : (2*i)+1); // (2*i)+1 is default in MTB Daemon <v1.8
        }
    }

    this->ui.b_refresh->setEnabled(true);
    this->setWindowTitle(tr("Configuration of module ")+QString::number(this->address)+" – "+module["type"].toString());
    this->ui.le_name->setFocus();
}

void MtbUnisConfigWindow::newModule(unsigned addr, MtbModuleType type) {
    this->updateInProgress = true;
    this->creatingNewModule = true;
    this->type = type;
    this->address = addr;

    this->ui.le_name->setText("");
    for (unsigned i = 0; i < UNIS_INPUTS_COUNT; i++)
        this->m_guiInputs[i].delay.setCurrentIndex(0);

    for (unsigned i = 0; i < UNIS_OUTPUTS_COUNT; i++) {
        this->m_guiOutputs[i].type.setCurrentText(0);
        MtbUnisConfigWindow::fillOutputSafeState(this->m_guiOutputs[i].safeState, 0, "plain");
    }

    for (unsigned i = 0; i < UNIS_SERVOS_COUNT; i++) {
        this->m_guiServos[i].posA.setValue(100);
        this->m_guiServos[i].posB.setValue(200);
        this->m_guiServos[i].posSensors.setCurrentIndex((2*i)+1);
        this->m_guiServos[i].speed.setValue(100);
    }

    this->ui.b_refresh->setEnabled(false);
    this->setWindowTitle(tr("New module ")+QString::number(addr)+" – "+moduleTypeToStr(type));
    this->ui.le_name->setFocus();
    this->updateInProgress = false;
    this->exec();
}

void MtbUnisConfigWindow::jsonParseError(const QString& err) {
    QMessageBox::warning(this, tr("Error"), tr("JSON parse error:")+"\n"+err);
}

void MtbUnisConfigWindow::fillOutputSafeState(QComboBox& cb, unsigned value, const QString& type) {
    cb.clear();
    if (type == "plain") {
        cb.addItem(tr("off"));
        cb.addItem(tr("on"));
        cb.setCurrentIndex(value);
    } else if (type == "s-com") {
        for (unsigned i = 0; i < SComSignalCodes.size(); i++)
            cb.addItem(QString::number(i)+" - "+SComSignalCodes[i]);
        cb.setCurrentIndex(value);
    } else if (type == "flicker") {
        for (unsigned i = 0; i < UniFlickerPerMin.size(); i++)
            cb.addItem(UniFlickerPerMin[i].description);
        cb.setCurrentIndex(-1);
        for (unsigned i = 0; i < UniFlickerPerMin.size(); i++)
            if (value == UniFlickerPerMin[i].freq)
                cb.setCurrentIndex(i);
    } else {
        cb.setCurrentIndex(-1);
    }
}

int MtbUnisConfigWindow::outputCbToValue(const QString& type, unsigned index) {
    if (type == "plain")
        return index;
    if (type == "s-com")
        return index;
    if (type == "flicker")
        return index < UniFlickerPerMin.size() ? UniFlickerPerMin[index].freq : 0;
    return 0;
}

void MtbUnisConfigWindow::ui_cbOutputTypeCurrentIndexChanged(int) {
    if (updateInProgress)
        return;
    for (unsigned i = 0; i < UNIS_OUTPUTS_COUNT; i++) {
        if (sender() == &this->m_guiOutputs[i].type) {
            MtbUnisConfigWindow::fillOutputSafeState(this->m_guiOutputs[i].safeState, 0, this->m_guiOutputs[i].type.currentText());
        }
    }
}

void MtbUnisConfigWindow::ui_bClicked(QAbstractButton *button) {
    if (button == this->ui.bb_main->button(QDialogButtonBox::StandardButton::Apply)) {
        this->apply();
    }
}

void MtbUnisConfigWindow::apply() {
    if (this->ui.le_name->text() == "") {
        QMessageBox::warning(this, tr("Error"), tr("Fill in module name!"));
        return;
    }

    for (unsigned i = 0; i < UNIS_INPUTS_COUNT; i++) {
        if (this->m_guiInputs[i].delay.currentIndex() < 0) {
            QMessageBox::warning(this, tr("Error"), tr("Fill in all input delays!"));
            return;
        }
    }

    for (unsigned i = 0; i < UNIS_OUTPUTS_COUNT; i++) {
        if (this->m_guiOutputs[i].type.currentIndex() < 0) {
            QMessageBox::warning(this, tr("Error"), tr("Fill in all output types!"));
            return;
        }

        if (this->m_guiOutputs[i].safeState.currentIndex() < 0) {
            QMessageBox::warning(this, tr("Error"), tr("Fill in all output safe states!"));
            return;
        }
    }


    QJsonArray inputsDelay;
    for (unsigned i = 0; i < UNIS_INPUTS_COUNT; i++)
        inputsDelay.append(static_cast<double>(this->m_guiInputs[i].delay.currentIndex())/10);

    QJsonArray outputsSafe;
    for (unsigned i = 0; i < UNIS_OUTPUTS_COUNT; i++) {
        QJsonObject output;
        const QString& outputType = this->m_guiOutputs[i].type.currentText();
        output["type"] = outputType;
        output["value"] = MtbUnisConfigWindow::outputCbToValue(outputType, this->m_guiOutputs[i].safeState.currentIndex());
        outputsSafe.append(output);
    }
    for (unsigned i = 0; i < UNIS_SERVOS_COUNT; i++) { // TODO temporary
        QJsonObject _default = {{"type", "plain"}, {"value", 0}};
        outputsSafe.append(_default);
        outputsSafe.append(_default);
    }

    unsigned servoEnabledMask = 0;
    QJsonArray servoPosition;
    QJsonArray servoSpeed;
    QJsonArray servoInputMap;
    for (unsigned i = 0; i < UNIS_SERVOS_COUNT; i++) {
        if (this->m_guiServos[i].enabled.isChecked())
            servoEnabledMask |= (1 << i);
        servoPosition.append(this->m_guiServos[i].posA.value());
        servoPosition.append(this->m_guiServos[i].posB.value());
        servoSpeed.append(this->m_guiServos[i].speed.value());
        servoInputMap.append(this->m_guiServos[i].posSensors.currentIndex());
    }

    QJsonObject config{
        {"inputsDelay", inputsDelay},
        {"outputsSafe", outputsSafe},
        {"servoEnabledMask", static_cast<int>(servoEnabledMask)},
        {"servoPosition", servoPosition},
        {"servoSpeed", servoSpeed},
        {"servoInputMap", servoInputMap},
    };

    QJsonObject newModule{
        {"command", "module_set_config"},
        {"address", this->address},
        {"type_code", static_cast<int>(this->type)},
        {"name", this->ui.le_name->text()},
        {"config", {config}}
    };

    QApplication::setOverrideCursor(Qt::WaitCursor);
    DaemonClient::instance->sendNoExc(
        newModule,
        [this](const QJsonObject& content) {
            (void)content;
            QApplication::restoreOverrideCursor();
            QMessageBox::information(this, tr("Ok"), (this->creatingNewModule) ? tr("Module successfully created.") : tr("Configuration successfully set."));
            this->updateModuleFromMtbDaemon();
        },
        [this](unsigned errorCode, QString errorMessage) {
            QApplication::restoreOverrideCursor();
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("module_set_config", errorCode, errorMessage));
        }
    );
}

void MtbUnisConfigWindow::retranslate() {
    this->ui.retranslateUi(this);
    this->lInDelay.setText(tr("Delay:"));
    this->lOutType.setText(tr("Type:"));
    this->lOutSafeState.setText(tr("Default:"));
    this->lServoEnabled.setText(tr("Enabled:"));
    this->lServoPosA.setText(tr("Pos. A [0-255]:"));
    this->lServoPosB.setText(tr("Pos. B [0-255]:"));
    this->lServoSpeed.setText(tr("Speed [0-255]:"));
    this->lServoPosSensors.setText(tr("Sensors:"));
}
