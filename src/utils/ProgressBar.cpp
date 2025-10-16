#include "ProgressBar.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace FileRecovery {

ProgressBar::ProgressBar(int width) 
    : m_width(width), m_lastProgress(0.0f), 
      m_startTime(std::chrono::steady_clock::now()) {}

void ProgressBar::update(float progress, const std::string& status) {
    if (progress < 0.0f) progress = 0.0f;
    if (progress > 1.0f) progress = 1.0f;
    
    m_lastProgress = progress;
    
    int barWidth = m_width;
    int pos = static_cast<int>(barWidth * progress);
    
    // Calculer le temps écoulé et estimé
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - m_startTime).count();
    
    double remaining = 0.0;
    if (progress > 0.001) {
        remaining = (elapsed / progress) - elapsed;
    }
    
    std::cout << "\r[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << std::fixed << std::setprecision(1) << (progress * 100.0) << "% ";
    
    if (!status.empty()) {
        std::cout << "| " << status << " ";
    }
    
    std::cout << "| Elapsed: " << formatTime(elapsed);
    
    if (progress > 0.001) {
        std::cout << " | ETA: " << formatTime(remaining);
    }
    
    std::cout << std::flush;
}

void ProgressBar::complete(const std::string& message) {
    update(1.0f, message);
    std::cout << std::endl;
}

void ProgressBar::reset() {
    m_lastProgress = 0.0f;
    m_startTime = std::chrono::steady_clock::now();
}

std::string ProgressBar::formatTime(double seconds) const {
    int hours = static_cast<int>(seconds) / 3600;
    int minutes = (static_cast<int>(seconds) % 3600) / 60;
    int secs = static_cast<int>(seconds) % 60;
    
    std::ostringstream oss;
    if (hours > 0) {
        oss << hours << "h " << minutes << "m";
    } else if (minutes > 0) {
        oss << minutes << "m " << secs << "s";
    } else {
        oss << secs << "s";
    }
    
    return oss.str();
}

std::string ProgressBar::formatSpeed(uint64_t bytesPerSecond) const {
    if (bytesPerSecond < 1024) {
        return std::to_string(bytesPerSecond) + " B/s";
    } else if (bytesPerSecond < 1024 * 1024) {
        return std::to_string(bytesPerSecond / 1024) + " KB/s";
    } else {
        return std::to_string(bytesPerSecond / (1024 * 1024)) + " MB/s";
    }
}

} // namespace FileRecovery
