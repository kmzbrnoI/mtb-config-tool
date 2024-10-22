#ifndef MODULEADDDIALOG_H
#define MODULEADDDIALOG_H

#include <QDialog>
#include "ui_moduleadd.h"
#include "common.h"

class ModuleAddDialog : public QDialog
{
    Q_OBJECT

public:
    ModuleAddDialog(QWidget *parent = nullptr);

    void add();
    void retranslate();

private:
    Ui::ModuleAddDialog ui;
    std::vector<std::pair<QRadioButton&, MtbModuleType>> m_types;

    void accept() override;
    std::optional<MtbModuleType> typeSelected() const;

};

#endif // MODULEADDDIALOG_H
