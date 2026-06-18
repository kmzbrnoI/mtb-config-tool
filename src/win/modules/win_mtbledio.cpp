#include <QJsonArray>
#include <QMessageBox>
#include "win_mtbledio.h"
#include "client.h"
#include "qjsonsafe.h"
#include "countingguard.h"

MtbLedIOWindow::MtbLedIOWindow(QWidget *parent) :
    MtbModuleIODialog(parent) {
    this->ui.setupUi(this);
    this->createGuiInputs();
    this->createGuiOutputs();
    this->retranslate();

    QObject::connect(ui.b_refresh, SIGNAL(released()), this, SLOT(refresh()));
}

void MtbLedIOWindow::createGuiInputs() {
    for (unsigned i = 0; i < LED_IO_COUNT; i++) {
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

void MtbLedIOWindow::createGuiOutputs() {
    for (unsigned i = 0; i < LED_IO_COUNT; i++) {
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
            QLabel& textState = this->m_guiOutputs[i].textState;
            textState.setText("?");
            textState.setAlignment(Qt::AlignCenter);
        }

        this->ui.gl_outputs->addWidget(&this->m_guiOutputs[i].name, i, 0);
        this->ui.gl_outputs->addWidget(&this->m_guiOutputs[i].rectState, i, 1);
        this->ui.gl_outputs->addWidget(&this->m_guiOutputs[i].textState, i, 2);

        QObject::connect(&this->m_guiOutputs[i].rectState, SIGNAL(onClicked()), this, SLOT(ui_wOutputClicked()));
    }
}

void MtbLedIOWindow::openModule(const QJsonObject& module) {
    MtbModuleIODialog::moduleChanged(module);
    this->address = QJsonSafe::safeUInt(module["address"]);
    this->disableAll();
    this->setWindowTitle(tr("IO of module ")+QString::number(this->address)+" – "+module["type"].toString());
    this->updateInProgress = 0;
    this->show();
    this->sendModuleRequest();
}

void MtbLedIOWindow::moduleChanged(const QJsonObject& module) {
    this->update(module);
    this->ui.b_refresh->setEnabled(true);
}

void MtbLedIOWindow::inputsChanged(const QJsonObject& module_inputs_changed) {
    std::array<bool, LED_IO_COUNT> inputsLastState = this->m_inputsState;
    this->updateInputs(QJsonSafe::safeObject(module_inputs_changed["inputs"]));
    for (unsigned i = 0; i < LED_IO_COUNT; i++)
        if (this->m_inputsState[i] != inputsLastState[i])
            this->m_guiInputs[i].textState.setStyleSheet("background-color:yellow;");
}

void MtbLedIOWindow::outputsChanged(const QJsonObject& module_outputs_changed) {
    this->updateOutputs(QJsonSafe::safeObject(module_outputs_changed["outputs"]));
}

void MtbLedIOWindow::update(const QJsonObject& module) {
    const QString& typeStr = QJsonSafe::safeString(module["type"]);
    const QString& stateStr = QJsonSafe::safeString(module["state"]);
    const QJsonObject& uni = QJsonSafe::safeObject(module[typeStr]);

    if (stateStr == "active") {
        if (uni.contains("state")) {
            const QJsonObject& state = QJsonSafe::safeObject(uni["state"]);
            this->updateInputs(QJsonSafe::safeObject(state["inputs"]));
            this->updateOutputs(QJsonSafe::safeObject(state["outputs"]));
        } // else in case of module update, but state not present (e.g. config update etc.)
    } else {
        this->disableAll();
    }
}

void MtbLedIOWindow::updateInputs(const QJsonObject& inputs) {
    const QJsonArray& inputsFull = QJsonSafe::safeArray(inputs["full"], LED_IO_COUNT);

    for (unsigned i = 0; i < LED_IO_COUNT; i++) {
        const bool state = QJsonSafe::safeBool(inputsFull[i]);
        this->m_inputsState[i] = state;
        this->m_guiInputs[i].textState.setText(QString::number(state));
        const QString color = state ? "green" : "red";
        this->m_guiInputs[i].rectState.setStyleSheet("background-color:"+color);
        this->m_guiInputs[i].textState.setStyleSheet("");
    }
}

void MtbLedIOWindow::updateOutputs(const QJsonObject& outputs) {
    const QJsonArray& outputsFull = QJsonSafe::safeArray(outputs["full"], LED_IO_COUNT);
    for (unsigned i = 0; i < LED_IO_COUNT; i++) {
        const bool state = QJsonSafe::safeBool(outputsFull[i]);
        this->m_guiOutputs[i].textState.setText(QString::number(state));
        const QString color = (state) ? "green" : "red";
        this->m_guiOutputs[i].rectState.setStyleSheet("background-color:"+color);

    }
}

void MtbLedIOWindow::disableAll() {
    for (LedIOGuiInput& guiInput : this->m_guiInputs) {
        guiInput.textState.setText("?");
        guiInput.rectState.setStyleSheet("background-color:gray");
    }
    for (LedIOGuiOutput& guiOutput : this->m_guiOutputs) {
        CountingGuard updateGuard(this->updateInProgress);
        guiOutput.textState.setText("?");
        guiOutput.rectState.setStyleSheet("background-color:gray");
    }
}

void MtbLedIOWindow::ui_wOutputClicked() {
    if (this->updateInProgress > 0)
        return;

    int output = -1;
    for (unsigned i = 0; i < LED_IO_COUNT; i++)
        if (sender() == &this->m_guiOutputs[i].rectState)
            output = i;
    if (output == -1)
        return;

    if (output >= static_cast<int>(this->m_guiOutputs.size()))
        return;
    LedIOGuiOutput& guiOutput = this->m_guiOutputs[output];

    const bool newState = (guiOutput.textState.text() == "0");

    guiOutput.rectState.setStyleSheet("background-color:yellow");

    DaemonClient::instance->sendNoExc(
        {
         {"command", "module_set_outputs"},
         {"address", this->address},
         {"outputs", QJsonObject({{QString::number(output), newState}})},
         },
        [this](const QJsonObject& content) {
            this->outputsChanged(content);
        },
        [this](unsigned errorCode, QString errorMessage) {
            QMessageBox::warning(this, tr("Error"), DaemonClient::standardErrrorMessage("setOutput", errorCode, errorMessage));
        }
        );
}

void MtbLedIOWindow::retranslate() {
    this->ui.retranslateUi(this);
}
