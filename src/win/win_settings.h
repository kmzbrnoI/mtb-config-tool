#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include "ui_settingsdialog.h"
#include "settings.h"

class SettingsWindow : public QDialog
{
    Q_OBJECT
public:
    SettingsWindow(Settings& s, QWidget *parent = nullptr);
    void open() override;
    void retranslate();

private:
    Ui::SettingsDialog ui;
    Settings& s;
    void accept() override;

private slots:

};

#endif // SETTINGSWINDOW_H
