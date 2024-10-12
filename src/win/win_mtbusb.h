#ifndef MTBUSBWINDOW_H
#define MTBUSBWINDOW_H

#include <QMainWindow>
#include "ui_mtbusbdialog.h"
#include "common.h"

class MtbUsbWindow : public QDialog
{
    Q_OBJECT
    const std::optional<MtbUsbStatus>& mtbUsbStatus;

public:
    explicit MtbUsbWindow(const std::optional<MtbUsbStatus>&, QWidget *parent = nullptr);
    void open() override;
    void retranslate();

private:
    Ui::MtbUsbDialog ui;

    void update();
    unsigned selectedSpeed() const;

private slots:
    void ui_bUpdateHandle();
    void ui_bClicked(QAbstractButton *button);

};

#endif // MTBUSBWINDOW_H
