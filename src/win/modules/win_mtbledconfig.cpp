#include <QJsonArray>
#include <QMessageBox>
#include "win_mtbledconfig.h"
#include "client.h"
#include "qjsonsafe.h"

MtbLedConfigWindow::MtbLedConfigWindow(QWidget *parent) :
    MtbModuleConfigDialog(parent) {
    this->ui.setupUi(this);
    this->createGuiOutputs();
    this->retranslate();

    QPushButton *b = this->ui.bb_main->button(QDialogButtonBox::Apply);
    if (b)
        b->setDefault(true);

    QObject::connect(ui.b_refresh, SIGNAL(released()), this, SLOT(refresh()));
    QObject::connect(ui.bb_main, SIGNAL(clicked(QAbstractButton*)), this, SLOT(ui_bClicked(QAbstractButton*)));
}

void MtbLedConfigWindow::createGuiOutputs() {
    this->ui.gl_outputs->addWidget(&this->lOutSafeState[0], 0, 1);
    this->ui.gl_outputs->addWidget(&this->lOutBrightness[0], 0, 2);
    this->ui.gl_outputs->addWidget(&this->lOutSafeState[1], 0, 4);
    this->ui.gl_outputs->addWidget(&this->lOutBrightness[1], 0, 5);

    for (unsigned i = 0; i < LED_IO_COUNT; i++) {
        QLabel& name = this->m_guiOutputs[i].name;
        name.setText(QString::number(i));
        name.setStyleSheet("font-weight: bold");
        name.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

        {
            QComboBox& safeState = this->m_guiOutputs[i].safeState;
            safeState.addItems({tr("off"), tr("on")});
            safeState.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        }

        {
            QSpinBox& brightness = this->m_guiOutputs[i].brightness;
            brightness.setMinimum(0);
            brightness.setMaximum(255);
            brightness.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        }

        const int row = i % (LED_IO_COUNT/2) + 1;
        const int column = i / (LED_IO_COUNT/2);

        this->ui.gl_outputs->addWidget(&this->m_guiOutputs[i].name, row, (3*column));
        this->ui.gl_outputs->addWidget(&this->m_guiOutputs[i].safeState, row, (3*column)+1);
        this->ui.gl_outputs->addWidget(&this->m_guiOutputs[i].brightness, row, (3*column)+2);
    }
}

void MtbLedConfigWindow::editModule(const QJsonObject& module) {
    this->creatingNewModule = false;
    this->update(module);
    this->show();
}

void MtbLedConfigWindow::update(const QJsonObject& module) {
    this->address = QJsonSafe::safeUInt(module["address"]);
    this->ui.le_name->setText(QJsonSafe::safeString(module["name"]));
    this->type = static_cast<MtbModuleType>(QJsonSafe::safeUInt(module["type_code"]));

    const QString& typeStr = QJsonSafe::safeString(module["type"]);
    const QJsonObject& led = QJsonSafe::safeObject(module[typeStr]);
    const QJsonObject& config = QJsonSafe::safeObject(led["config"]);

    // Outputs
    const QJsonArray& outputs = QJsonSafe::safeArray(config["outputsSafe"], LED_IO_COUNT);
    const QJsonArray& brightness = QJsonSafe::safeArray(config["brightness"], LED_IO_COUNT);
    for (unsigned i = 0; i < LED_IO_COUNT; i++) {
        this->m_guiOutputs[i].safeState.setCurrentIndex(static_cast<int>(QJsonSafe::safeBool(outputs[i])));
        this->m_guiOutputs[i].brightness.setValue(QJsonSafe::safeUInt(brightness[i]));
    }

    this->ui.b_refresh->setEnabled(true);
    this->setWindowTitle(tr("Configuration of module ")+QString::number(this->address)+" – "+module["type"].toString());
    this->ui.le_name->setFocus();
}

void MtbLedConfigWindow::newModule(unsigned addr, MtbModuleType type) {
    this->creatingNewModule = true;
    this->type = type;
    this->address = addr;

    this->ui.le_name->setText("");

    for (unsigned i = 0; i < LED_IO_COUNT; i++) {
        this->m_guiOutputs[i].safeState.setCurrentText(0);
        this->m_guiOutputs[i].brightness.setValue(100);
    }

    this->ui.b_refresh->setEnabled(false);
    this->setWindowTitle(tr("New module ")+QString::number(addr)+" – "+moduleTypeToStr(type));
    this->ui.le_name->setFocus();
    this->exec();
}

void MtbLedConfigWindow::ui_bClicked(QAbstractButton *button) {
    if (button == this->ui.bb_main->button(QDialogButtonBox::StandardButton::Apply)) {
        this->apply();
    }
}

void MtbLedConfigWindow::apply() {
    if (this->ui.le_name->text() == "") {
        QMessageBox::warning(this, tr("Error"), tr("Fill in module name!"));
        return;
    }

    for (unsigned i = 0; i < LED_IO_COUNT; i++) {
        if (this->m_guiOutputs[i].safeState.currentIndex() < 0) {
            QMessageBox::warning(this, tr("Error"), tr("Fill in all output safe states!"));
            return;
        }
    }

    QJsonArray outputsSafe;
    for (unsigned i = 0; i < LED_IO_COUNT; i++)
        outputsSafe.append(this->m_guiOutputs[i].safeState.currentIndex() == 1);

    QJsonArray brightness;
    for (unsigned i = 0; i < LED_IO_COUNT; i++)
        brightness.append(this->m_guiOutputs[i].brightness.value());

    QJsonObject config{
        {"outputsSafe", outputsSafe},
        {"brightness", brightness},
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

void MtbLedConfigWindow::retranslate() {
    this->ui.retranslateUi(this);
    this->lOutSafeState[0].setText(tr("Default:"));
    this->lOutSafeState[1].setText(tr("Default:"));
    this->lOutBrightness[0].setText(tr("Brightness [0-255]:"));
    this->lOutBrightness[1].setText(tr("Brightness [0-255]:"));
}
