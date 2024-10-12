#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QDialog>
#include "ui_logdialog.h"
#include "log.h"
#include "common.h"

class LogWindow : public QDialog
{
    Q_OBJECT
    static const int MAX_LOG_ROWS = 300;

    static constexpr QColor LOGC_ERROR = QC_LIGHT_RED;
    static constexpr QColor LOGC_WARN = QC_LIGHT_YELLOW;
    static constexpr QColor LOGC_DONE = QC_LIGHT_GREEN;
    static constexpr QColor LOGC_RECV = QC_LIGHT_BLUE;
    static constexpr QColor LOGC_SEND = QColor(0xE0, 0xFF, 0xE0);

public:
    explicit LogWindow(QWidget *parent = nullptr);
    void log(const QString &message, LogLevel loglevel);
    void retranslate();

    static LogWindow* instance;

private:
    Ui::LogDialog ui;

private slots:
    void ui_bClearHandle();
    void ui_twItemSelectionChanged();

};

#endif // LOGWINDOW_H
