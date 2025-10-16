#pragma once

#include <fstream>
#include <string>
#include <mutex>
#include <chrono>
#include <sstream>
#include <iomanip>

class FileLogger {
public:
    static FileLogger& getInstance() {
        static FileLogger instance;
        return instance;
    }

    void init(const std::string& filename = "log.txt") {
        try {
            std::lock_guard<std::mutex> lock(mutex_);
            if (logFile_.is_open()) {
                logFile_.close();
            }
            logFile_.open(filename, std::ios::out | std::ios::trunc);
            if (logFile_.is_open()) {
                log("INFO", "Log file initialized: " + filename);
            }
        } catch (...) {
            // Ignore logger errors to prevent crashes
        }
    }

    void log(const std::string& level, const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!logFile_.is_open()) return;

        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::tm tm_buf;
        std::tm* tm_ptr = nullptr;
        
        #ifdef _WIN32
            if (localtime_s(&tm_buf, &time) == 0) {
                tm_ptr = &tm_buf;
            }
        #else
            tm_ptr = std::localtime(&time);
        #endif

        if (tm_ptr) {
            logFile_ << std::put_time(tm_ptr, "%Y-%m-%d %H:%M:%S")
                     << "." << std::setfill('0') << std::setw(3) << ms.count()
                     << " [" << level << "] " << message << std::endl;
        } else {
            // Fallback si le timestamp échoue
            logFile_ << "[" << level << "] " << message << std::endl;
        }
        logFile_.flush();
    }

    void info(const std::string& message) { try { log("INFO", message); } catch(...) {} }
    void warning(const std::string& message) { try { log("WARNING", message); } catch(...) {} }
    void error(const std::string& message) { try { log("ERROR", message); } catch(...) {} }
    void debug(const std::string& message) { try { log("DEBUG", message); } catch(...) {} }

    ~FileLogger() {
        try {
            if (logFile_.is_open()) {
                log("INFO", "Closing log file");
                logFile_.close();
            }
        } catch (...) {
            // Ignore errors on cleanup
        }
    }

private:
    FileLogger() = default;
    FileLogger(const FileLogger&) = delete;
    FileLogger& operator=(const FileLogger&) = delete;

    std::ofstream logFile_;
    std::mutex mutex_;
};

// Macros pour faciliter l'utilisation
#define LOG_INFO(msg) FileLogger::getInstance().info(msg)
#define LOG_WARNING(msg) FileLogger::getInstance().warning(msg)
#define LOG_ERROR(msg) FileLogger::getInstance().error(msg)
#define LOG_DEBUG(msg) FileLogger::getInstance().debug(msg)
