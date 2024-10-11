#ifndef WIN_CHANGE_ADRESS_H
#define WIN_CHANGE_ADRESS_H

#include <QDialog>
#include "ui_changeaddress.h"

class ChangeAddressDialog : public QDialog
{
    Q_OBJECT

public:
    ChangeAddressDialog(QWidget *parent = nullptr);

    void openFromModule(uint8_t module);
    void openGeneral();

private:
    Ui::ChangeAddressDialog ui;

    void moduleSetAddress();
    void setAddress();

private slots:
    void ui_rbToggled();
    void ui_bClicked(QAbstractButton *button);

};

#endif // WIN_CHANGE_ADRESS_H
