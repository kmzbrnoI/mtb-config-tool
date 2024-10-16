#ifndef WIN_DIAG_H
#define WIN_DIAG_H

#include <QDialog>
#include "ui_diag.h"

class DiagDialog : public QDialog
{
    Q_OBJECT

public:
    DiagDialog(QWidget *parent = nullptr);

    void retranslate();
    void moduleOpen(const QJsonObject& module);

private:
    Ui::DiagDialog ui;

};

#endif // WIN_DIAG_H
