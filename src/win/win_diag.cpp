#include "win_diag.h"
#include "ui_diag.h"

DiagDialog::DiagDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DiagDialog)
{
    ui->setupUi(this);
}

DiagDialog::~DiagDialog()
{
    delete ui;
}
