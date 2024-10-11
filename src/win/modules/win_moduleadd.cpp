#include <QMessageBox>
#include "win_moduleadd.h"
#include "win_config.h"
#include "win_mtbuniconfig.h"

ModuleAddDialog::ModuleAddDialog(QWidget *parent) :
    QDialog(parent)
{
    this->ui.setupUi(this);
    this->setFixedSize(this->width(), this->height());
    this->setWindowFlags(Qt::Tool);

    m_types = {
        {*this->ui.rb_univ2, MtbModuleType::Univ2noIr},
        {*this->ui.rb_univ2ir, MtbModuleType::Univ2ir},
        {*this->ui.rb_univ4, MtbModuleType::Univ42},
        {*this->ui.rb_unis, MtbModuleType::Unis10},
        {*this->ui.rb_rc, MtbModuleType::Rc},
    };
}

void ModuleAddDialog::add() {
    this->ui.sb_address->setFocus();
    this->show();
}

std::optional<MtbModuleType> ModuleAddDialog::typeSelected() const {
    for (const auto& pair : this->m_types)
        if (pair.first.isChecked())
            return {pair.second};
    return {};
}

void ModuleAddDialog::accept() {
    if (!this->typeSelected().has_value()) {
        QMessageBox::warning(this, tr("Cannot add module!"), tr("Select a module type!"));
        return;
    }

    MtbModuleType type = this->typeSelected().value();
    uint8_t addr = this->ui.sb_address->value();

    std::unique_ptr<MtbModuleConfigDialog> configDialog;

    switch (type) {
    case MtbModuleType::Univ2noIr:
    case MtbModuleType::Univ2ir:
    case MtbModuleType::Univ40:
    case MtbModuleType::Univ42:
        configDialog = std::make_unique<MtbUniConfigWindow>();
        break;
    default:
        QMessageBox::warning(this, tr("Cannot add module!"), tr("Module configuration dialog is not available!"));
        return;
    }

    if (configDialog == nullptr) {
        QMessageBox::warning(this, tr("Cannot add module!"), tr("Internal error: missing configDialog pointer!"));
        return;
    }

    this->close();
    this->accepted();
    configDialog->newModule(addr, type);
}
