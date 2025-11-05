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

// Log levels
enum class LogLevel {
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
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    const std::string BOLD = "\033[1m";
}

class Logger {
private:
    static std::mutex& get_mutex() {
        static std::mutex log_mutex;
        return log_mutex;
    }
    
    static LogLevel& get_current_level() {
        static LogLevel current_level = LogLevel::INFO;
        return current_level;
    }
    
    static std::ofstream& get_log_file() {
        static std::ofstream log_file;
        return log_file;
    }
    
    static bool& get_log_to_file() {
        static bool log_to_file = false;
        return log_to_file;
    }
    
    static bool& get_log_to_console() {
        static bool log_to_console = true;
        return log_to_console;
    }
    
    static bool& get_use_colors() {
        static bool use_colors = true;
        return use_colors;
    }

    static std::string get_timestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

    static std::string level_to_string(LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::FATAL: return "FATAL";
            default: return "UNKNOWN";
        }
    }

    static std::string get_level_color(LogLevel level) {
        if (!get_use_colors()) return "";
        
        switch (level) {
            case LogLevel::DEBUG: return LogColors::CYAN;
            case LogLevel::INFO: return LogColors::GREEN;
            case LogLevel::WARNING: return LogColors::YELLOW;
            case LogLevel::ERROR: return LogColors::RED;
            case LogLevel::FATAL: return LogColors::BOLD + LogColors::RED;
            default: return LogColors::RESET;
        }
    }

public:
    static void init(LogLevel level = LogLevel::INFO, 
                    const std::string& filename = "", 
                    bool console = true, 
                    bool colors = true) {
        std::lock_guard<std::mutex> lock(get_mutex());
        get_current_level() = level;
        get_log_to_console() = console;
        get_use_colors() = colors;
        
        if (!filename.empty()) {
            get_log_file().open(filename, std::ios::app);
            get_log_to_file() = get_log_file().is_open();
        } else {
            get_log_to_file() = false;
        }
    }

    static void set_level(LogLevel level) {
        std::lock_guard<std::mutex> lock(get_mutex());
        get_current_level() = level;
    }

    static void log(LogLevel level, const std::string& message, 
                   const std::string& file = "", int line = 0) {
        if (level < get_current_level()) return;

        std::lock_guard<std::mutex> lock(get_mutex());
        
        std::string timestamp = get_timestamp();
        std::string level_str = level_to_string(level);
        std::string location = "";
        
        if (!file.empty() && line > 0) {
            size_t pos = file.find_last_of("/\\");
            std::string filename = (pos != std::string::npos) ? file.substr(pos + 1) : file;
            location = " [" + filename + ":" + std::to_string(line) + "]";
        }

        // Console output
        if (get_log_to_console()) {
            std::string color = get_level_color(level);
            std::cout << color << "[" << timestamp << "] " 
                     << std::setw(7) << std::left << level_str 
                     << LogColors::RESET << location << " " << message << std::endl;
        }

        // File output
        if (get_log_to_file() && get_log_file().is_open()) {
            get_log_file() << "[" << timestamp << "] " 
                          << std::setw(7) << std::left << level_str 
                          << location << " " << message << std::endl;
            get_log_file().flush();
        }
    }

    static void close() {
        std::lock_guard<std::mutex> lock(get_mutex());
        if (get_log_file().is_open()) {
            get_log_file().close();
        }
    }
};

// Timer utility class
class Timer {
private:
    std::chrono::steady_clock::time_point start_time;
    std::string name;

public:
    Timer(const std::string& timer_name = "") : name(timer_name) {
        start();
    }

    void start() {
        start_time = std::chrono::steady_clock::now();
    }

    double elapsed_ms() const {
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            end_time - start_time);
        return duration.count() / 1000.0;
    }

    double elapsed_seconds() const {
        return elapsed_ms() / 1000.0;
    }

    void log_elapsed(LogLevel level = LogLevel::INFO) const {
        if (!name.empty()) {
            Logger::log(level, name + " took " + std::to_string(elapsed_ms()) + " ms");
        } else {
            Logger::log(level, "Timer elapsed: " + std::to_string(elapsed_ms()) + " ms");
        }
    }

    ~Timer() {
        if (!name.empty()) {
            log_elapsed();
        }
    }
};

// Convenience macros with stream support
#define ZHENLU_LOG_DEBUG(msg) do { \
    std::stringstream ss; ss << msg; \
    Logger::log(LogLevel::DEBUG, ss.str(), __FILE__, __LINE__); \
} while(0)

#define ZHENLU_LOG_INFO(msg) do { \
    std::stringstream ss; ss << msg; \
    Logger::log(LogLevel::INFO, ss.str(), __FILE__, __LINE__); \
} while(0)

#define ZHENLU_LOG_WARNING(msg) do { \
    std::stringstream ss; ss << msg; \
    Logger::log(LogLevel::WARNING, ss.str(), __FILE__, __LINE__); \
} while(0)

#define ZHENLU_LOG_ERROR(msg) do { \
    std::stringstream ss; ss << msg; \
    Logger::log(LogLevel::ERROR, ss.str(), __FILE__, __LINE__); \
} while(0)

#define ZHENLU_LOG_FATAL(msg) do { \
    std::stringstream ss; ss << msg; \
    Logger::log(LogLevel::FATAL, ss.str(), __FILE__, __LINE__); \
} while(0)

// Conditional logging macro
#define ZHENLU_LOG_IF(condition, level, msg) do { \
    if (condition) { \
        std::stringstream ss; ss << msg; \
        Logger::log(level, ss.str(), __FILE__, __LINE__); \
    } \
} while(0)

// Timer macros
#define ZHENLU_SCOPED_TIMER(name) Timer scoped_timer_##__LINE__(name)
#define ZHENLU_TIMER_CHECKPOINT(timer_name, checkpoint_name) \
    Timer::log(LogLevel::INFO, std::string(timer_name) + " checkpoint " + checkpoint_name + ": " + std::to_string(timer_name.elapsed_ms()) + " ms")

#endif // ZHENLU_LOG_H