#include <QJsonArray>
#include <QMessageBox>
#include "win_mtbuniconfig.h"

MtbUniConfigWindow::MtbUniConfigWindow(QWidget *parent) :
    MtbModuleConfigDialog(parent) {
    this->ui.setupUi(this);
    this->setFixedSize(this->width(), this->height());
    this->setWindowFlags(Qt::Tool);
    this->createGuiInputs();
    this->createGuiOutputs();
}

void MtbUniConfigWindow::createGuiInputs() {
    for (unsigned i = 0; i < UNI_INPUTS_COUNT; i++) {
        QLabel& name = this->m_guiInputs[i].name;
        name.setText(QString::number(i));
        name.setStyleSheet("font-weight: bold");

        this->m_guiInputs[i].typeLabel.setText(tr("Type:"));
        this->m_guiInputs[i].delayLabel.setText(tr("Delay:"));

        {
            QComboBox& type = this->m_guiInputs[i].type;
            type.addItems({"plain", "ir"});
        }

        {
            QComboBox& delay = this->m_guiInputs[i].delay;
            delay.addItems({"0 s", "0.1 s", "0.2 s", "0.3 s", "0.4 s", "0.5 s", "0.6 s", "0.7 s",
                            "0.8 s", "0.9 s", "1.0 s", "1.1 s", "1.2 s", "1.3 s", "1.4 s", "1.5 s"});
        }

        this->ui.gl_inputs->addWidget(&this->m_guiInputs[i].name, i, 0);
        this->ui.gl_inputs->addWidget(&this->m_guiInputs[i].typeLabel, i, 1);
        this->ui.gl_inputs->addWidget(&this->m_guiInputs[i].type, i, 2);
        this->ui.gl_inputs->addWidget(&this->m_guiInputs[i].delayLabel, i, 3);
        this->ui.gl_inputs->addWidget(&this->m_guiInputs[i].delay, i, 4);
    }
}

void MtbUniConfigWindow::createGuiOutputs() {
    for (unsigned i = 0; i < UNI_OUTPUTS_COUNT; i++) {
        QLabel& name = this->m_guiOutputs[i].name;
        name.setText(QString::number(i));
        name.setStyleSheet("font-weight: bold");

        this->m_guiOutputs[i].typeLabel.setText(tr("Type:"));
        this->m_guiOutputs[i].safeStateLabel.setText(tr("Default:"));

        {
            QComboBox& type = this->m_guiOutputs[i].type;
            type.addItems({"plain", "s-com", "flicker"});
        }

        {
            QComboBox& delay = this->m_guiOutputs[i].safeState;
            delay.addItems({tr("off"), tr("on")});
        }

        this->ui.gl_outputs->addWidget(&this->m_guiOutputs[i].name, i, 0);
        this->ui.gl_outputs->addWidget(&this->m_guiOutputs[i].typeLabel, i, 1);
        this->ui.gl_outputs->addWidget(&this->m_guiOutputs[i].type, i, 2);
        this->ui.gl_outputs->addWidget(&this->m_guiOutputs[i].safeStateLabel, i, 3);
        this->ui.gl_outputs->addWidget(&this->m_guiOutputs[i].safeState, i, 4);
    }
}

void MtbUniConfigWindow::editModule(const QJsonObject& module) {
    if (module["address"].toInt(-1) == -1)
        return this->jsonParseError("'address' does not exist or is invalid type.");
    const uint8_t addr = module["address"].toInt();

    if (!module["name"].isString())
        return this->jsonParseError("'name' does not exist or is invalid type.");
    this->ui.le_name->setText(module["name"].toString());
    MtbModuleType moduleType = static_cast<MtbModuleType>(module["type_code"].toInt());
    this->updateUiType(moduleType);

    if (!module["type"].isString())
        return this->jsonParseError("'type' "+tr("does not exist or is invalid type."));
    const QString& typeStr = module["type"].toString();
    if (!module[typeStr].isObject())
        return this->jsonParseError("'"+typeStr+tr("' does not exist or is invalid type."));
    const QJsonObject& uni = module[typeStr].toObject();
    if (!uni["config"].isObject())
        return this->jsonParseError("'config' "+tr("does not exist or is invalid type."));
    const QJsonObject& config = uni["config"].toObject();


    // Inputs

    if (moduleType == MtbModuleType::Univ2ir) {
        if (!config["irs"].isArray())
            return this->jsonParseError("'irs' "+tr("does not exist or is invalid type."));
        const QJsonArray& irs = config["irs"].toArray();
        if (irs.size() != UNI_INPUTS_COUNT)
            return this->jsonParseError(tr("Invalid irs size!"));

        for (unsigned i = 0; i < UNI_INPUTS_COUNT; i++) {
            if (!irs[i].isBool())
                return this->jsonParseError(tr("'irs[i]' is not a bool!"));
            this->m_guiInputs[i].type.setCurrentIndex(irs[i].toBool());
        }
    }

    if ((!config["inputsDelay"].isArray()) || (config["inputsDelay"].toArray().size() != UNI_INPUTS_COUNT))
        return this->jsonParseError("'inputsDelay' "+tr("does not exist or is invalid type or has invalid length."));
    const QJsonArray& inputsDelay = config["inputsDelay"].toArray();
    for (unsigned i = 0; i < UNI_INPUTS_COUNT; i++) {
        if (!inputsDelay[i].isDouble())
            return this->jsonParseError(tr("'inputsDelay[i]' is not a double!"));
        unsigned delay = static_cast<int>(inputsDelay[i].toDouble() * 10);
        this->m_guiInputs[i].delay.setCurrentIndex(delay);
    }


    // Outputs

    if ((!config["outputsSafe"].isArray()) || (config["outputsSafe"].toArray().size() != UNI_INPUTS_COUNT))
        return this->jsonParseError("'outputsSafe' "+tr("does not exist or is invalid type or has invalid length."));
    const QJsonArray& outputs = config["outputsSafe"].toArray();
    for (unsigned i = 0; i < UNI_OUTPUTS_COUNT; i++) {
        if (!outputs[i].isObject())
            return this->jsonParseError("'outputs[i]' "+tr("is not an object."));
        const QJsonObject& output = outputs[i].toObject();

        if (!output["type"].isString())
            return this->jsonParseError("'type' "+tr("does not exist or is invalid type."));
        if (output["value"].toInt(-1) == -1)
            return this->jsonParseError("'type' "+tr("does not exist or is invalid type."));

        const QString& type = output["type"].toString();
        const unsigned value = output["value"].toInt();

        this->m_guiOutputs[i].type.setCurrentText(type);
        MtbUniConfigWindow::fillOutputSafeState(this->m_guiOutputs[i].safeState, value, type);
    }

    this->setWindowTitle(tr("Configuration of module ")+QString::number(addr)+" – "+module["type"].toString());
    this->ui.le_name->setFocus();
    this->show();
}

void MtbUniConfigWindow::newModule(unsigned addr, MtbModuleType type) {
    this->updateUiType(type);
    this->setWindowTitle(tr("New module ")+QString::number(addr)+" – "+moduleTypeToStr(type));
    this->ui.le_name->setFocus();
    this->show();
}

void MtbUniConfigWindow::updateUiType(MtbModuleType type) {
    for (UniGuiInput& in : this->m_guiInputs) {
        in.type.setEnabled(type == MtbModuleType::Univ2ir);
        if (type != MtbModuleType::Univ2ir)
            in.type.setCurrentIndex(0);
    }
}

void MtbUniConfigWindow::jsonParseError(const QString& err) {
    QMessageBox::warning(this, tr("Error"), tr("JSON parse error:")+"\n"+err);
}

void MtbUniConfigWindow::fillOutputSafeState(QComboBox& cb, unsigned value, const QString& type) {
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
