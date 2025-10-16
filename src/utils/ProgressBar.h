#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <string>
#include <chrono>

namespace FileRecovery {

class ProgressBar {
public:
    ProgressBar(int width = 50);
    
    void update(float progress, const std::string& status = "");
    void complete(const std::string& message = "Done!");
    void reset();

private:
    int m_width;
    float m_lastProgress;
    std::chrono::steady_clock::time_point m_startTime;
    
    std::string formatTime(double seconds) const;
    std::string formatSpeed(uint64_t bytesPerSecond) const;
};

} // namespace FileRecovery

#endif // PROGRESS_BAR_H
