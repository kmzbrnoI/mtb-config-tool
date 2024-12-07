#include <QJsonArray>
#include <QMessageBox>
#include "win_mtbuniio.h"
#include "client.h"
#include "qjsonsafe.h"

MtbUniIOWindow::MtbUniIOWindow(QWidget *parent) :
    MtbModuleIODialog(parent) {
    this->ui.setupUi(this);
    this->setFixedSize(this->width(), this->height());
    this->createGuiInputs();
    this->createGuiOutputs();
    this->retranslate();

    /*QPushButton *b = this->ui.bb_main->button(QDialogButtonBox::Apply);
    if (b)
        b->setDefault(true);

    QObject::connect(ui.b_refresh, SIGNAL(released()), this, SLOT(refresh()));
    QObject::connect(ui.bb_main, SIGNAL(clicked(QAbstractButton*)), this, SLOT(ui_bClicked(QAbstractButton*)));*/
}

void MtbUniIOWindow::createGuiInputs() {
    /*this->ui.gl_inputs->addWidget(&this->lInType, 0, 1);
    this->ui.gl_inputs->addWidget(&this->lInDelay, 0, 2);

    for (unsigned i = 0; i < UNI_INPUTS_COUNT; i++) {
        QLabel& name = this->m_guiInputs[i].name;
        name.setText(QString::number(i));
        name.setStyleSheet("font-weight: bold");
        name.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

        {
            QComboBox& type = this->m_guiInputs[i].type;
            type.addItems({"plain", "ir"});
        }

        {
            QComboBox& delay = this->m_guiInputs[i].delay;
            delay.addItems({"0 s", "0.1 s", "0.2 s", "0.3 s", "0.4 s", "0.5 s", "0.6 s", "0.7 s",
                            "0.8 s", "0.9 s", "1.0 s", "1.1 s", "1.2 s", "1.3 s", "1.4 s", "1.5 s"});
        }

        this->ui.gl_inputs->addWidget(&this->m_guiInputs[i].name, i+1, 0);
        this->ui.gl_inputs->addWidget(&this->m_guiInputs[i].type, i+1, 1);
        this->ui.gl_inputs->addWidget(&this->m_guiInputs[i].delay, i+1, 2);
    }*/
}

void MtbUniIOWindow::createGuiOutputs() {
    /*this->ui.gl_outputs->addWidget(&this->lOutType, 0, 1);
    this->ui.gl_outputs->addWidget(&this->lOutSafeState, 0, 2);

    for (unsigned i = 0; i < UNI_OUTPUTS_COUNT; i++) {
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
    }*/
}

void MtbUniIOWindow::openModule(const QJsonObject& module) {
    /*this->creatingNewModule = false;
    this->update(module);
    this->show();*/
}

void MtbUniIOWindow::moduleChanged(const QJsonObject& module) {

}

void MtbUniIOWindow::inputsChanged(const QJsonObject& module) {

}

void MtbUniIOWindow::outputsChanged(const QJsonObject& module) {

}

/*void MtbUniIOWindow::update(const QJsonObject& module) {
    this->address = QJsonSafe::safeUInt(module["address"]);
    this->ui.le_name->setText(QJsonSafe::safeString(module["name"]));
    this->type = static_cast<MtbModuleType>(QJsonSafe::safeUInt(module["type_code"]));
    this->updateUiType(this->type);

    const QString& typeStr = QJsonSafe::safeString(module["type"]);
    const QJsonObject& uni = QJsonSafe::safeObject(module[typeStr]);
    const QJsonObject& config = QJsonSafe::safeObject(uni["config"]);

    // Inputs

    if (this->type == MtbModuleType::Univ2ir) {
        const QJsonArray& irs = QJsonSafe::safeArray(config["irs"], UNI_INPUTS_COUNT);
        for (unsigned i = 0; i < UNI_INPUTS_COUNT; i++)
            this->m_guiInputs[i].type.setCurrentIndex(QJsonSafe::safeBool(irs[i]));
    }

    const QJsonArray& inputsDelay = QJsonSafe::safeArray(config["inputsDelay"], UNI_INPUTS_COUNT);
    for (unsigned i = 0; i < UNI_INPUTS_COUNT; i++) {
        unsigned delay = static_cast<int>(QJsonSafe::safeDouble(inputsDelay[i].toDouble()) * 10);
        this->m_guiInputs[i].delay.setCurrentIndex(delay);
    }


    // Outputs

    const QJsonArray& outputs = QJsonSafe::safeArray(config["outputsSafe"], UNI_OUTPUTS_COUNT);
    for (unsigned i = 0; i < UNI_OUTPUTS_COUNT; i++) {
        const QJsonObject& output = QJsonSafe::safeObject(outputs[i]);
        const QString& type = QJsonSafe::safeString(output["type"]);
        const unsigned value = QJsonSafe::safeUInt(output["value"]);

        this->updateInProgress = true;
        this->m_guiOutputs[i].type.setCurrentText(type);
        MtbUniConfigWindow::fillOutputSafeState(this->m_guiOutputs[i].safeState, value, type);
        this->updateInProgress = false;
    }

    this->ui.b_refresh->setEnabled(true);
    this->setWindowTitle(tr("Configuration of module ")+QString::number(this->address)+" – "+module["type"].toString());
    this->ui.le_name->setFocus();
}*/

void MtbUniIOWindow::updateUiType(MtbModuleType type) {
    for (UniIOGuiInput& in : this->m_guiInputs) {
        in.type.setEnabled(type == MtbModuleType::Univ2ir);
        if (type != MtbModuleType::Univ2ir)
            in.type.setCurrentIndex(0);
    }
}

void MtbUniIOWindow::jsonParseError(const QString& err) {
    QMessageBox::warning(this, tr("Error"), tr("JSON parse error:")+"\n"+err);
}

void MtbUniIOWindow::fillOutputSafeState(QComboBox& cb, unsigned value, const QString& type) {
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
        cb.addItem("1 Hz");
        cb.addItem("2 Hz");
        cb.addItem("3 Hz");
        cb.addItem("4 Hz");
        cb.addItem("5 Hz");
        cb.addItem("10 Hz");
        cb.addItem("33/min");
        cb.addItem("66/min");
        cb.setCurrentIndex(-1);
        for (unsigned i = 0; i < UniFlickerPerMin.size(); i++)
            if (value == UniFlickerPerMin[i])
                cb.setCurrentIndex(i);
    } else {
        cb.setCurrentIndex(-1);
    }
}

int MtbUniIOWindow::outputCbToValue(const QString& type, unsigned index) {
    if (type == "plain")
        return index;
    if (type == "s-com")
        return index;
    if (type == "flicker")
        return index < UniFlickerPerMin.size() ? UniFlickerPerMin[index] : 0;
    return 0;
}

void MtbUniIOWindow::ui_cbOutputTypeCurrentIndexChanged(int) {
    /*if (updateInProgress)
        return;
    for (unsigned i = 0; i < UNI_OUTPUTS_COUNT; i++) {
        if (sender() == &this->m_guiOutputs[i].type) {
            MtbUniConfigWindow::fillOutputSafeState(this->m_guiOutputs[i].safeState, 0, this->m_guiOutputs[i].type.currentText());
        }
    }*/
}

void MtbUniIOWindow::ui_bClicked(QAbstractButton *button) {
    /*if (button == this->ui.bb_main->button(QDialogButtonBox::StandardButton::Apply)) {
        this->apply();
    }*/
}

void MtbUniIOWindow::apply() {
    /*if (this->ui.le_name->text() == "") {
        QMessageBox::warning(this, tr("Error"), tr("Fill in module name!"));
        return;
    }

    for (unsigned i = 0; i < UNI_INPUTS_COUNT; i++) {
        if (this->m_guiInputs[i].type.currentIndex() < 0) {
            QMessageBox::warning(this, tr("Error"), tr("Fill in all input types!"));
            return;
        }

        if (this->m_guiInputs[i].delay.currentIndex() < 0) {
            QMessageBox::warning(this, tr("Error"), tr("Fill in all input delays!"));
            return;
        }
    }

    for (unsigned i = 0; i < UNI_OUTPUTS_COUNT; i++) {
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
    for (unsigned i = 0; i < UNI_INPUTS_COUNT; i++)
        inputsDelay.append(static_cast<double>(this->m_guiInputs[i].delay.currentIndex())/10);

    QJsonArray outputsSafe;
    for (unsigned i = 0; i < UNI_OUTPUTS_COUNT; i++) {
        QJsonObject output;
        const QString& outputType = this->m_guiOutputs[i].type.currentText();
        output["type"] = outputType;
        output["value"] = MtbUniConfigWindow::outputCbToValue(outputType, this->m_guiOutputs[i].safeState.currentIndex());
        outputsSafe.append(output);
    }

    QJsonObject config{
        {"inputsDelay", inputsDelay},
        {"outputsSafe", outputsSafe},
    };

    if (this->type == MtbModuleType::Univ2ir) {
        QJsonArray irs;
        for (unsigned i = 0; i < UNI_INPUTS_COUNT; i++)
            irs.append(this->m_guiInputs[i].type.currentIndex() == 1);
        config["irs"] = irs;
    }

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
    );*/
}

void MtbUniIOWindow::retranslate() {
    this->ui.retranslateUi(this);
    this->lInType.setText(tr("Type:"));
    this->lInDelay.setText(tr("Delay:"));
    this->lOutType.setText(tr("Type:"));
    this->lOutSafeState.setText(tr("Default:"));
}
