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
    for (unsigned i = 0; i < UNI_INPUTS_COUNT; i++) {
        {
            QLabel& name = this->m_guiInputs[i].name;
            name.setText(QString::number(i));
            name.setStyleSheet("font-weight: bold");
            name.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        }

        {
            QWidget& rectState = this->m_guiInputs[i].rectState;
            rectState.setGeometry(0, 0, RECT_WIDTH, RECT_HEIGHT);
            rectState.setStyleSheet("background-color:gray;");
        }

        {
            QLabel& textState = this->m_guiInputs[i].textState;
            textState.setText("?");
        }

        this->ui.gl_inputs->addWidget(&this->m_guiInputs[i].name, i, 0);
        this->ui.gl_inputs->addWidget(&this->m_guiInputs[i].rectState, i, 1);
        this->ui.gl_inputs->addWidget(&this->m_guiInputs[i].textState, i, 2);
    }
}

void MtbUniIOWindow::createGuiOutputs() {
    for (unsigned i = 0; i < UNI_OUTPUTS_COUNT; i++) {
        {
            QLabel& name = this->m_guiOutputs[i].name;
            name.setText(QString::number(i));
            name.setStyleSheet("font-weight: bold");
            name.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        }

        {
            QWidget& rectState = this->m_guiOutputs[i].rectState;
            rectState.setGeometry(0, 0, RECT_WIDTH, RECT_HEIGHT);
            rectState.setStyleSheet("background-color:gray;");
        }

        this->ui.gl_outputs->addWidget(&this->m_guiOutputs[i].name, i, 0);
        this->ui.gl_outputs->addWidget(&this->m_guiOutputs[i].rectState, i, 1);
        this->ui.gl_outputs->addWidget(&this->m_guiOutputs[i].cbState, i, 2);

        QObject::connect(&this->m_guiOutputs[i].cbState, SIGNAL(currentIndexChanged(int)), this, SLOT(ui_cbOutputStateCurrentIndexChanged(int)));
    }
}

void MtbUniIOWindow::openModule(const QJsonObject& module) {
    this->address = QJsonSafe::safeUInt(module["address"]);
    this->disableAll();
    this->setWindowTitle(tr("IO of module ")+QString::number(this->address)+" – "+module["type"].toString());
    this->show();
    this->sendModuleRequest();
}

void MtbUniIOWindow::moduleChanged(const QJsonObject& module) {
    this->update(module);
    this->ui.b_refresh->setEnabled(true);
}

void MtbUniIOWindow::inputsChanged(const QJsonObject& module_inputs_changed) {
    this->updateInputs(QJsonSafe::safeObject(module_inputs_changed["inputs"]));
}

void MtbUniIOWindow::outputsChanged(const QJsonObject& module_outputs_changed) {
    this->updateOutputs(QJsonSafe::safeObject(module_outputs_changed["outputs"]));
}

void MtbUniIOWindow::update(const QJsonObject& module) {
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

void MtbUniIOWindow::updateInputs(const QJsonObject& inputs) {
    const QJsonArray& inputsFull = QJsonSafe::safeArray(inputs["full"], UNI_INPUTS_COUNT);

    for (unsigned i = 0; i < UNI_INPUTS_COUNT; i++) {
        bool state = static_cast<int>(QJsonSafe::safeBool(inputsFull[i].toBool()));
        this->m_guiInputs[i].textState.setText(QString::number(state));
        QString color = state ? "green" : "red";
        this->m_guiInputs[i].rectState.setStyleSheet("background-color:"+color);
    }
}

void MtbUniIOWindow::updateOutputs(const QJsonObject& outputs) {
    for (unsigned i = 0; i < UNI_OUTPUTS_COUNT; i++) {
        const QJsonObject& output = QJsonSafe::safeObject(outputs[QString::number(i)]);
        this->updateOutput(i, output);
    }
}

void MtbUniIOWindow::updateOutputType(unsigned outputi, const QString& type) {
    UniIOGuiOutput& guiOutput = this->m_guiOutputs[outputi];
    guiOutput.outputType = type;
    guiOutput.cbState.clear();
    if (type == "plain") {
        guiOutput.cbState.addItem(tr("off"));
        guiOutput.cbState.addItem(tr("on"));
    } else if (type == "s-com") {
        for (size_t i = 0; i < SComSignalCodes.size(); i++)
            guiOutput.cbState.addItem(QString::number(i)+" - "+SComSignalCodes[i]);
    } else if (type == "flicker") {
        for (const FlickerDef& def : UniFlickerPerMin)
            guiOutput.cbState.addItem(def.description);
    }
}

void MtbUniIOWindow::updateOutput(unsigned outputi, const QJsonObject& output) {
    const QString& newTypeStr = QJsonSafe::safeString(output["type"]);
    const unsigned value = QJsonSafe::safeUInt(output["value"]);
    UniIOGuiOutput& guiOutput = this->m_guiOutputs[outputi];
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
        guiOutput.rectState.setStyleSheet("background-color:gray");
    } else if (newTypeStr == "flicker") {
        bool set = false;
        for (size_t i = 0; i < UniFlickerPerMin.size(); i++) {
            if (value == UniFlickerPerMin[i].freq) {
                guiOutput.cbState.setCurrentIndex(i);
                set = true;
            }
        }
        if (!set)
            guiOutput.cbState.setCurrentIndex(-1);
        guiOutput.rectState.setStyleSheet("background-color:gray");
    } else {
        guiOutput.cbState.setCurrentIndex(-1);
        guiOutput.rectState.setStyleSheet("background-color:gray");
    }
}

void MtbUniIOWindow::disableAll() {
    for (UniIOGuiInput& guiInput : this->m_guiInputs) {
        guiInput.textState.setText("?");
        guiInput.rectState.setStyleSheet("background-color:gray");
    }
    for (UniIOGuiOutput& guiOutput : this->m_guiOutputs) {
        guiOutput.cbState.setEnabled(false);
        guiOutput.cbState.setCurrentIndex(-1);
        guiOutput.rectState.setStyleSheet("background-color:gray");
    }
}

void MtbUniIOWindow::jsonParseError(const QString& err) {
    QMessageBox::warning(this, tr("Error"), tr("JSON parse error:")+"\n"+err);
}

void MtbUniIOWindow::ui_cbOutputStateCurrentIndexChanged(int) {
    /*if (updateInProgress)
        return;
    for (unsigned i = 0; i < UNI_OUTPUTS_COUNT; i++) {
        if (sender() == &this->m_guiOutputs[i].type) {
            MtbUniConfigWindow::fillOutputSafeState(this->m_guiOutputs[i].safeState, 0, this->m_guiOutputs[i].type.currentText());
        }
    }*/
}

void MtbUniIOWindow::retranslate() {
    this->ui.retranslateUi(this);
}
