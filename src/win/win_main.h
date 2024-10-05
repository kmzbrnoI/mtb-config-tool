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
    SettingsWindow settings_window;

private slots:
    void ui_m_about_triggered(bool);
    void ui_a_options_triggered(bool);

}; // class MainWindow

#endif // MAINWINDOW_H
