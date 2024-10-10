#include "log.h"
#include "win_log.h"
#include "win_main.h"

void log(const QString& msg, LogLevel level) {
    if (LogWindow::instance != nullptr)
        LogWindow::instance->log(msg, level);
    if ((level == LogLevel::Error) && (MainWindow::instance != nullptr))
        MainWindow::instance->criticalError(msg);
}
