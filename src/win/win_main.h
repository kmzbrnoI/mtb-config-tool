#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include "ui_mainwindow.h"
#include "settings.h"
#include "win_settings.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Settings& s, QWidget *parent = nullptr);
    void retranslate();

private:
    Ui::MainWindow ui;
    Settings& s;
    SettingsWindow m_settingsWindow;

private slots:
    void ui_MAboutTriggered(bool);
    void ui_AOptionsTriggered(bool);

}; // class MainWindow

#endif // MAINWINDOW_H
