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
    const uint8_t addr = module["address"].toInt();

    this->ui.le_name->setText(module["name"].toString());

    this->setWindowTitle(tr("Configuration of module ")+QString::number(addr)+" – "+module["type"].toString());
    this->ui.le_name->setFocus();
    this->show();
}

void MtbUniConfigWindow::newModule() {
    this->show();
}
