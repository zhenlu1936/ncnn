#ifndef ZHENLU_LOG_H
#define ZHENLU_LOG_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <mutex>
#include <cstring>
#include <cstdarg>
#include <vector>

namespace Zhenlu {

// Log levels
enum class LogLevel
{
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    FATAL = 4
};

// Color codes for console output
namespace LogColors {
const std::string RESET = "\033[0m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string CYAN = "\033[36m";
const std::string BOLD = "\033[1m";
} // namespace LogColors

class Logger
{
private:
    static std::mutex& get_mutex()
    {
        static std::mutex log_mutex;
        return log_mutex;
    }

    static LogLevel& get_current_level()
    {
        static LogLevel current_level = LogLevel::INFO;
        return current_level;
    }

    static std::ofstream& get_log_file()
    {
        static std::ofstream log_file;
        return log_file;
    }

    static bool& get_log_to_file()
    {
        static bool log_to_file = false;
        return log_to_file;
    }

    static bool& get_log_to_console()
    {
        static bool log_to_console = true;
        return log_to_console;
    }

    static bool& get_use_colors()
    {
        static bool use_colors = true;
        return use_colors;
    }

    static std::string get_timestamp()
    {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch())
                  % 1000;

        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

    static std::string level_to_string(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
        }
    }

    static std::string get_level_color(LogLevel level)
    {
        if (!get_use_colors()) return "";

        switch (level)
        {
        case LogLevel::DEBUG:
            return LogColors::CYAN;
        case LogLevel::INFO:
            return LogColors::GREEN;
        case LogLevel::WARNING:
            return LogColors::YELLOW;
        case LogLevel::ERROR:
            return LogColors::RED;
        case LogLevel::FATAL:
            return LogColors::BOLD + LogColors::RED;
        default:
            return LogColors::RESET;
        }
    }

public:
    static void init(LogLevel level = LogLevel::INFO,
                     const std::string& filename = "",
                     bool console = true,
                     bool colors = true)
    {
        std::lock_guard<std::mutex> lock(get_mutex());
        get_current_level() = level;
        get_log_to_console() = console;
        get_use_colors() = colors;

        if (!filename.empty())
        {
            get_log_file().open(filename, std::ios::app);
            get_log_to_file() = get_log_file().is_open();
        }
        else
        {
            get_log_to_file() = false;
        }
    }

    static void set_level(LogLevel level)
    {
        std::lock_guard<std::mutex> lock(get_mutex());
        get_current_level() = level;
    }

    static void log(LogLevel level, const std::string& message,
                    const std::string& file = "", int line = 0)
    {
        if (level < get_current_level()) return;

        std::lock_guard<std::mutex> lock(get_mutex());

        std::string timestamp = get_timestamp();
        std::string level_str = level_to_string(level);
        std::string location = "";

        if (!file.empty() && line > 0)
        {
            size_t pos = file.find_last_of("/\\");
            std::string filename = (pos != std::string::npos) ? file.substr(pos + 1) : file;
            location = " [" + filename + ":" + std::to_string(line) + "]";
        }

        // Console output
        if (get_log_to_console())
        {
            std::string color = get_level_color(level);
            std::cout << color << "[" << timestamp << "] "
                      << std::setw(7) << std::left << level_str
                      << LogColors::RESET << location << " " << message << std::endl;
        }

        // File output
        if (get_log_to_file() && get_log_file().is_open())
        {
            get_log_file() << "[" << timestamp << "] "
                           << std::setw(7) << std::left << level_str
                           << location << " " << message << std::endl;
            get_log_file().flush();
        }
    }

    // Printf-style logging function
    static void logf(LogLevel level, const std::string& file, int line, const char* format, ...)
    {
        if (level < get_current_level()) return;

        va_list args;
        va_start(args, format);

        // Calculate required buffer size
        va_list args_copy;
        va_copy(args_copy, args);
        int size = vsnprintf(nullptr, 0, format, args_copy) + 1;
        va_end(args_copy);

        if (size <= 1)
        {
            va_end(args);
            return;
        }

        // Format the message
        std::vector<char> buffer(size);
        vsnprintf(buffer.data(), size, format, args);
        va_end(args);

        std::string message(buffer.data());
        log(level, message, file, line);
    }

    static void close()
    {
        std::lock_guard<std::mutex> lock(get_mutex());
        if (get_log_file().is_open())
        {
            get_log_file().close();
        }
    }
};

// Timer utility class
class Timer
{
private:
    std::chrono::steady_clock::time_point start_time;
    std::string name;

public:
    explicit Timer(const std::string& timer_name = "")
        : name(timer_name)
    {
        start_time = std::chrono::steady_clock::now();
    }

    double elapsed_ms() const
    {
        auto end_time = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(
                   end_time - start_time)
               .count()
               / 1000.0;
    }

    void log_elapsed(LogLevel level = LogLevel::INFO) const
    {
        std::string msg = name.empty() ? "Timer elapsed" : name + " took";
        Logger::log(level, msg + ": " + std::to_string(elapsed_ms()) + " ms");
    }

    ~Timer()
    {
        if (!name.empty()) log_elapsed();
    }
};

// Logging control switches
#if defined(ZHENLU_LOG_OFF)
// When ZHENLU_LOG_OFF is defined, all logging macros are disabled
#define ZHENLU_LOG(level, msg) \
    do                         \
    {                          \
    } while (0)
#define ZHENLU_LOG_DEBUG(msg) \
    do                        \
    {                         \
    } while (0)
#define ZHENLU_LOG_INFO(msg) \
    do                       \
    {                        \
    } while (0)
#define ZHENLU_LOG_WARNING(msg) \
    do                          \
    {                           \
    } while (0)
#define ZHENLU_LOG_ERROR(msg) \
    do                        \
    {                         \
    } while (0)
#define ZHENLU_LOG_FATAL(msg) \
    do                        \
    {                         \
    } while (0)

#define ZHENLU_LOGF(level, format, ...) \
    do                                  \
    {                                   \
    } while (0)
#define ZHENLU_LOGF_DEBUG(format, ...) \
    do                                 \
    {                                  \
    } while (0)
#define ZHENLU_LOGF_INFO(format, ...) \
    do                                \
    {                                 \
    } while (0)
#define ZHENLU_LOGF_WARNING(format, ...) \
    do                                   \
    {                                    \
    } while (0)
#define ZHENLU_LOGF_ERROR(format, ...) \
    do                                 \
    {                                  \
    } while (0)
#define ZHENLU_LOGF_FATAL(format, ...) \
    do                                 \
    {                                  \
    } while (0)

#define ZHENLU_LOG_IF(condition, level, msg) \
    do                                       \
    {                                        \
    } while (0)
#define ZHENLU_LOGF_IF(condition, level, format, ...) \
    do                                                \
    {                                                 \
    } while (0)

#define ZHENLU_SCOPED_TIMER(name) \
    do                            \
    {                             \
    } while (0)

#elif defined(ZHENLU_LOG_ON) || !defined(ZHENLU_LOG_OFF)
// When ZHENLU_LOG_ON is defined or neither switch is defined, logging is enabled (default behavior)

// Convenience macros with stream support
#define ZHENLU_LOG(level, msg)                                                      \
    do                                                                              \
    {                                                                               \
        std::stringstream ss;                                                       \
        ss << msg;                                                                  \
        Zhenlu::Logger::log(Zhenlu::LogLevel::level, ss.str(), __FILE__, __LINE__); \
    } while (0)

#define ZHENLU_LOG_DEBUG(msg)   ZHENLU_LOG(DEBUG, msg)
#define ZHENLU_LOG_INFO(msg)    ZHENLU_LOG(INFO, msg)
#define ZHENLU_LOG_WARNING(msg) ZHENLU_LOG(WARNING, msg)
#define ZHENLU_LOG_ERROR(msg)   ZHENLU_LOG(ERROR, msg)
#define ZHENLU_LOG_FATAL(msg)   ZHENLU_LOG(FATAL, msg)

// Printf-style convenience macros
#define ZHENLU_LOGF(level, format, ...) \
    Zhenlu::Logger::logf(Zhenlu::LogLevel::level, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define ZHENLU_LOGF_DEBUG(format, ...)   ZHENLU_LOGF(DEBUG, format, ##__VA_ARGS__)
#define ZHENLU_LOGF_INFO(format, ...)    ZHENLU_LOGF(INFO, format, ##__VA_ARGS__)
#define ZHENLU_LOGF_WARNING(format, ...) ZHENLU_LOGF(WARNING, format, ##__VA_ARGS__)
#define ZHENLU_LOGF_ERROR(format, ...)   ZHENLU_LOGF(ERROR, format, ##__VA_ARGS__)
#define ZHENLU_LOGF_FATAL(format, ...)   ZHENLU_LOGF(FATAL, format, ##__VA_ARGS__)

// Conditional logging macros
#define ZHENLU_LOG_IF(condition, level, msg)   \
    do                                         \
    {                                          \
        if (condition) ZHENLU_LOG(level, msg); \
    } while (0)

#define ZHENLU_LOGF_IF(condition, level, format, ...)             \
    do                                                            \
    {                                                             \
        if (condition) ZHENLU_LOGF(level, format, ##__VA_ARGS__); \
    } while (0)

// Timer macros
#define ZHENLU_SCOPED_TIMER(name) Zhenlu::Timer scoped_timer_##__LINE__(name)

#endif // ZHENLU_LOG_ON || !ZHENLU_LOG_OFF

} // namespace Zhenlu

#endif // ZHENLU_LOG_H