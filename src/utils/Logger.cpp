#include "Logger.h"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace FileRecovery {

class Logger::Impl {
public:
    LogLevel currentLevel = LogLevel::INFO;
    std::ofstream logFile;
    bool consoleOutput = true;
    std::mutex mutex;
};

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() {
    if (pImpl->logFile.is_open()) {
        pImpl->logFile.close();
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < pImpl->currentLevel) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Format du timestamp
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    
    // Niveau de log
    std::string levelStr;
    switch (level) {
        case LogLevel::DEBUG:   levelStr = "DEBUG"; break;
        case LogLevel::INFO:    levelStr = "INFO "; break;
        case LogLevel::WARNING: levelStr = "WARN "; break;
        case LogLevel::ERROR:   levelStr = "ERROR"; break;
        case LogLevel::FATAL:   levelStr = "FATAL"; break;
    }
    
    std::string logLine = "[" + oss.str() + "] [" + levelStr + "] " + message;
    
    // Console
    if (pImpl->consoleOutput) {
        if (level >= LogLevel::ERROR) {
            std::cerr << logLine << std::endl;
        } else {
            std::cout << logLine << std::endl;
        }
    }
    
    // Fichier
    if (pImpl->logFile.is_open()) {
        pImpl->logFile << logLine << std::endl;
        pImpl->logFile.flush();
    }
}

void Logger::setLogLevel(LogLevel level) {
    pImpl->currentLevel = level;
}

void Logger::setLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->logFile.is_open()) {
        pImpl->logFile.close();
    }
    
    pImpl->logFile.open(filename, std::ios::app);
    if (!pImpl->logFile.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

void Logger::enableConsoleOutput(bool enable) {
    pImpl->consoleOutput = enable;
}

} // namespace FileRecovery
