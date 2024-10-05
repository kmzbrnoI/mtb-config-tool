#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include "ui_settingsdialog.h"

class SettingsWindow : public QDialog
{
    Q_OBJECT
public:
    SettingsWindow(QWidget *parent = nullptr);

private:
    Ui::SettingsDialog ui;

private slots:

};

#endif // SETTINGSWINDOW_H
