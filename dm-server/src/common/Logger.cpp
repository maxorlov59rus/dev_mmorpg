#include "Logger.h"
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
// Undefine Windows macros that conflict with enum values
#ifdef ERROR
#undef ERROR
#endif
#ifdef INFO
#undef INFO
#endif
#ifdef DEBUG
#undef DEBUG
#endif
#ifdef WARN
#undef WARN
#endif
#endif

namespace dm {
namespace common {

Logger& Logger::Instance() {
    static Logger instance;
    return instance;
}

Logger::~Logger() {
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
}

void Logger::Init(const std::string& logFilePath, LogLevel minLevel, bool console, bool fileOutput) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_logFilePath = logFilePath;
    m_minLevel = minLevel;
    m_console = console;
    m_fileOutput = fileOutput;

    if (m_fileOutput) {
        m_logFile.open(logFilePath, std::ios::out | std::ios::app);
        m_initialized = m_logFile.is_open();
        if (m_initialized) {
            Log(LogLevel::INFO, "=== Server started ===");
        } else {
            std::cerr << "Failed to open log file: " << logFilePath << std::endl;
        }
    } else {
        m_initialized = true;
    }
}

std::string Logger::LevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO ";
        case LogLevel::WARN:  return "WARN ";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::AUDIT: return "AUDIT";
        default:              return "UNKN ";
    }
}

LogLevel Logger::StringToLevel(const std::string& str) {
    if (str == "DEBUG") return LogLevel::DEBUG;
    if (str == "INFO")  return LogLevel::INFO;
    if (str == "WARN")  return LogLevel::WARN;
    if (str == "ERROR") return LogLevel::ERROR;
    if (str == "AUDIT") return LogLevel::AUDIT;
    return LogLevel::INFO;
}

std::string Logger::GetTimestamp() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void Logger::WriteToFile(const std::string& line) {
    if (m_fileOutput && m_logFile.is_open()) {
        m_logFile << line << std::endl;
        m_logFile.flush();
    }
}

#ifdef _WIN32
    static WORD GetConsoleColor(LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG:  return FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;    // Cyan
            case LogLevel::INFO:   return FOREGROUND_GREEN | FOREGROUND_INTENSITY;                      // Green
            case LogLevel::WARN:   return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;      // Yellow
            case LogLevel::ERROR:  return FOREGROUND_RED | FOREGROUND_INTENSITY;                        // Red
            case LogLevel::AUDIT:  return FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;       // Magenta
            default:               return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;           // White
        }
    }

    static void SetConsoleColor(WORD color) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        SetConsoleTextAttribute(hConsole, color);
    }

    static void RestoreConsoleColor() {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        SetConsoleTextAttribute(hConsole, csbi.wAttributes);
    }
#endif

void Logger::Log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(m_mutex);

    // Level filtering
    int levelVal = static_cast<int>(level);
    int minLevelVal = static_cast<int>(m_minLevel);
    if (levelVal < minLevelVal) return;

    std::string timestamp = GetTimestamp();
    std::string levelStr = LevelToString(level);
    std::string line = "[" + timestamp + "] [" + levelStr + "] " + message;

    // Output to console with color
    if (m_console) {
#ifdef _WIN32
        SetConsoleColor(GetConsoleColor(level));
        if (level == LogLevel::ERROR) {
            std::cerr << line << std::endl;
        } else {
            std::cout << line << std::endl;
        }
        RestoreConsoleColor();
#else
        if (level == LogLevel::ERROR) {
            std::cerr << line << std::endl;
        } else {
            std::cout << line << std::endl;
        }
#endif
    }

    // Output to file
    WriteToFile(line);
}

void Logger::Error(const std::string& message) {
    Log(LogLevel::ERROR, message);
}

void Logger::Warn(const std::string& message) {
    Log(LogLevel::WARN, message);
}

void Logger::Info(const std::string& message) {
    Log(LogLevel::INFO, message);
}

void Logger::Debug(const std::string& message) {
    Log(LogLevel::DEBUG, message);
}

void Logger::Audit(const std::string& message) {
    Log(LogLevel::AUDIT, message);
}

} // namespace common
} // namespace dm
