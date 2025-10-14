#include <QJsonArray>
#include <QMessageBox>
#include "win_mtbrcio.h"
#include "qjsonsafe.h"

MtbRCIOWindow::MtbRCIOWindow(QWidget *parent) :
    MtbModuleIODialog(parent) {
    this->ui.setupUi(this);
    this->createGuiInputs();
    this->retranslate();

    QObject::connect(ui.b_refresh, SIGNAL(released()), this, SLOT(refresh()));
}

void MtbRCIOWindow::createGuiInputs() {
    this->lHeadInput.setText(tr("Input"));
    this->ui.gl_inputs->addWidget(&this->lHeadInput, 0, 0);
    this->lHeadAddresses.setText(tr("Detected addresses"));
    this->ui.gl_inputs->addWidget(&this->lHeadAddresses, 0, 1);

    for (unsigned i = 0; i < RC_INPUTS_COUNT; i++) {
        {
            QLabel& name = this->m_guiInputs[i].lName;
            name.setText(QString::number(i));
            name.setStyleSheet("font-weight: bold");
            name.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        }

        {
            QTextEdit& teAddresses = this->m_guiInputs[i].teAddresses;
            teAddresses.setReadOnly(true);
            teAddresses.setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        }

        this->ui.gl_inputs->addWidget(&this->m_guiInputs[i].lName, i+1, 0);
        this->ui.gl_inputs->addWidget(&this->m_guiInputs[i].teAddresses, i+1, 1);
    }
}

void MtbRCIOWindow::openModule(const QJsonObject& module) {
    this->address = QJsonSafe::safeUInt(module["address"]);
    this->disableAll();
    this->setWindowTitle(tr("IO of module ")+QString::number(this->address)+" – "+module["type"].toString());
    this->show();
    this->sendModuleRequest();
}

void MtbRCIOWindow::moduleChanged(const QJsonObject& module) {
    MtbModuleIODialog::moduleChanged(module);
    this->update(module);
    this->ui.b_refresh->setEnabled(true);
}

void MtbRCIOWindow::inputsChanged(const QJsonObject& module_inputs_changed) {
    this->updateInputs(QJsonSafe::safeObject(module_inputs_changed["inputs"]));
}

void MtbRCIOWindow::outputsChanged(const QJsonObject&) {
}

void MtbRCIOWindow::update(const QJsonObject& module) {
    const QString& typeStr = QJsonSafe::safeString(module["type"]);
    const QString& stateStr = QJsonSafe::safeString(module["state"]);
    const QJsonObject& rc = QJsonSafe::safeObject(module[typeStr]);

    if (stateStr == "active") {
        if (rc.contains("state")) {
            const QJsonObject& state = QJsonSafe::safeObject(rc["state"]);
            this->updateInputs(QJsonSafe::safeObject(state["inputs"]));
        } // else in case of module update, but state not present (e.g. config update etc.)
    } else {
        this->disableAll();
    }
}

void MtbRCIOWindow::updateInputs(const QJsonObject& inputs) {
    const QJsonArray& ports = QJsonSafe::safeArray(inputs["ports"], RC_INPUTS_COUNT);

    for (unsigned i = 0; i < RC_INPUTS_COUNT; i++) {
        const QJsonArray& addresses = ports[i].toArray();
        this->m_guiInputs[i].teAddresses.clear();
        QString strAddresses;
        for (const auto& addr : addresses)
            strAddresses += QString::number(QJsonSafe::safeUInt(addr)) + " ";
        this->m_guiInputs[i].teAddresses.setText(strAddresses.left(strAddresses.size()-1));
        this->m_guiInputs[i].teAddresses.setEnabled(true);
    }
}

void MtbRCIOWindow::disableAll() {
    for (RCIOGuiInput& guiInput : this->m_guiInputs)
        guiInput.teAddresses.setEnabled(false);
}

void MtbRCIOWindow::retranslate() {
    this->ui.retranslateUi(this);
}
