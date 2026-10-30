// ------------------------------------------------------
// BLACKMATTER TEAN PROJECT 2026
// ------------------------------------------------------

// ------------------------------------------------------
// BLACKMATTER TEAN PROJECT 2026
// ------------------------------------------------------

#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <ctime>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace dm {
namespace common {

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    AUDIT
};

enum class WeatherType {
    CLEAR,
    RAIN,
    THUNDER,
    SNOW,
    FOG,
    SANDSTORM
};

enum class WeatherSeverity {
    LIGHT,
    MEDIUM,
    HEAVY
};

std::string WeatherTypeToString(WeatherType type);
std::string WeatherSeverityToString(WeatherSeverity severity);

class Logger {
public:
    static Logger& Instance();

    void Init(const std::string& logFilePath, LogLevel minLevel = LogLevel::DEBUG, bool console = true, bool fileOutput = true);
    void Log(LogLevel level, const std::string& message);
    void Error(const std::string& message);
    void Warn(const std::string& message);
    void Info(const std::string& message);
    void Debug(const std::string& message);
    void Audit(const std::string& message);

    LogLevel GetMinLevel() const { return m_minLevel; }
    bool IsConsoleEnabled() const { return m_console; }
    bool IsFileOutputEnabled() const { return m_fileOutput; }
    static LogLevel StringToLevel(const std::string& str);

private:
    Logger() = default;
    ~Logger();

    std::string LevelToString(LogLevel level);
    std::string GetTimestamp();
    void WriteToFile(const std::string& line);

    std::ofstream m_logFile;
    std::mutex m_mutex;
    bool m_initialized = false;
    std::string m_logFilePath;
    LogLevel m_minLevel = LogLevel::DEBUG;
    bool m_console = true;
    bool m_fileOutput = true;
};

} // namespace common
} // namespace dm
