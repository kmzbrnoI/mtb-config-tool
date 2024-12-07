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
    this->update(module);
    this->setWindowTitle(tr("IO of module ")+QString::number(this->address)+" – "+module["type"].toString());
    this->show();
}

void MtbUniIOWindow::moduleChanged(const QJsonObject& module) {
    this->update(module);
    this->ui.b_refresh->setEnabled(true);
}

void MtbUniIOWindow::inputsChanged(const QJsonObject& module_inputs_changed) {

}

void MtbUniIOWindow::outputsChanged(const QJsonObject& module_outputs_changed) {

}

void MtbUniIOWindow::update(const QJsonObject& module) {
    const QString& typeStr = QJsonSafe::safeString(module["type"]);
    const QJsonObject& uni = QJsonSafe::safeObject(module[typeStr]);
    const QJsonObject& state = QJsonSafe::safeObject(uni["state"]);

    // Inputs
    const QJsonObject& inputs = QJsonSafe::safeObject(state["inputs"]);
    const QJsonArray& inputsFull = QJsonSafe::safeArray(inputs["full"], UNI_INPUTS_COUNT);

    for (unsigned i = 0; i < UNI_INPUTS_COUNT; i++) {
        bool state = static_cast<int>(QJsonSafe::safeBool(inputsFull[i].toBool()));
        this->m_guiInputs[i].textState.setText(QString::number(state));
        QString color = state ? "green" : "red";
        this->m_guiInputs[i].rectState.setStyleSheet("background-color:"+color);
    }

    // Outputs
    const QJsonObject& outputs = QJsonSafe::safeObject(state["outputs"]);

    for (unsigned i = 0; i < UNI_OUTPUTS_COUNT; i++) {
        const QJsonObject& output = QJsonSafe::safeObject(outputs[QString::number(i)]);
        const QString& type = QJsonSafe::safeString(output["type"]);
        const unsigned value = QJsonSafe::safeUInt(output["value"]);

        if (type == "plain") {
            this->m_guiOutputs[i].cbState.setCurrentIndex(value);
            const QString color = (value > 0) ? "green" : "red";
            this->m_guiOutputs[i].rectState.setStyleSheet("background-color:"+color);
        } else if (type == "s-com") {
            this->m_guiOutputs[i].cbState.setCurrentIndex(value);
            this->m_guiOutputs[i].rectState.setStyleSheet("background-color:gray");
        } else if (type == "flicker") {

        } else {
            this->m_guiOutputs[i].cbState.setCurrentIndex(-1);
            this->m_guiOutputs[i].rectState.setStyleSheet("background-color:gray");
        }
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
