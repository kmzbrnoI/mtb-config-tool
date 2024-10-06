#include "log.h"
#include "win_log.h"

void log(const QString& msg, LogLevel level) {
    if (LogWindow::instance != nullptr)
        LogWindow::instance->log(msg, level);
}
