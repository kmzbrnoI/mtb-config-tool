#ifndef LOG_H
#define LOG_H

#include <QString>

enum class LogLevel {
    None = 0,
    Error = 1,
    Warning = 2,
    Info = 3,
    Commands = 4,
    RawData = 5,
    Debug = 6,
};

void log(const QString& msg, LogLevel level);

#endif // LOG_H
