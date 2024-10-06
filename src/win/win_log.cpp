#include <QTime>
#include "win_log.h"

LogWindow* LogWindow::instance = nullptr;

LogWindow::LogWindow(QWidget *parent)
    : QDialog{parent}
{
    ui.setupUi(this);
    LogWindow::instance = this;

    QObject::connect(ui.b_clear, SIGNAL(released()), this, SLOT(ui_bClearHandle()));
    QObject::connect(ui.tw_log, SIGNAL(itemSelectionChanged()), this, SLOT(ui_twItemSelectionChanged()));

    this->ui.cb_loglevel->setFocus();
}

void LogWindow::log(const QString &message, LogLevel loglevel) {
    constexpr size_t COLUMN_COUNT = 3;

    if (ui.tw_log->topLevelItemCount() > MAX_LOG_ROWS)
        ui.tw_log->clear();

    if (((message == "RECV: {}") || (message == "SEND: {}")) && (loglevel == LogLevel::Messages) && (!this->ui.chb_log_keep_alive->isChecked()))
        return; // Do not log keep-alive

    auto *item = new QTreeWidgetItem(ui.tw_log);
    item->setText(0, QTime::currentTime().toString("hh:mm:ss.zzz"));

    if (message.startsWith("RECV:"))
        for (size_t i = 0; i < COLUMN_COUNT; i++)
            item->setBackground(i, LOGC_RECV);
    if (message.startsWith("SEND:"))
        for (size_t i = 0; i < COLUMN_COUNT; i++)
            item->setBackground(i, LOGC_SEND);

    if (loglevel == LogLevel::None)
        item->setText(1, "None");
    else if (loglevel == LogLevel::Error) {
        item->setText(1, "Error");
        for (size_t i = 0; i < COLUMN_COUNT; i++)
            item->setBackground(i, LOGC_ERROR);
    } else if (loglevel == LogLevel::Warning) {
        item->setText(1, "Warning");
        for (size_t i = 0; i < COLUMN_COUNT; i++)
            item->setBackground(i, LOGC_WARN);
    } else if (loglevel == LogLevel::Info)
        item->setText(1, "Info");
    else if (loglevel == LogLevel::Messages)
        item->setText(1, "Messages");
    else if (loglevel == LogLevel::Debug)
        item->setText(1, "Debug");

    item->setText(2, message);
    ui.tw_log->addTopLevelItem(item);
}

void LogWindow::ui_bClearHandle() {
    this->ui.tw_log->clear();
    this->ui.te_message->clear();
}

void LogWindow::ui_twItemSelectionChanged() {
    if (this->ui.tw_log->selectedItems().empty())
        this->ui.te_message->clear();
    else
        this->ui.te_message->setText(this->ui.tw_log->selectedItems()[0]->text(2));
}
