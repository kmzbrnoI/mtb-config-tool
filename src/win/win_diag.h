#ifndef WIN_DIAG_H
#define WIN_DIAG_H

#include <QDialog>

namespace Ui {
class DiagDialog;
}

class DiagDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DiagDialog(QWidget *parent = nullptr);
    ~DiagDialog();

private:
    Ui::DiagDialog *ui;
};

#endif // WIN_DIAG_H
