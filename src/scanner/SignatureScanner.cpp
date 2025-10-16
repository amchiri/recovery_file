#include "SignatureScanner.h"
#include "../utils/Logger.h"

namespace FileRecovery {

SignatureScanner::SignatureScanner() {}

bool SignatureScanner::startScan(const DiskInfo& disk, const ScanConfig& config) {
    if (m_scanning) return false;
    
    m_scanning = true;
    m_thread = std::make_unique<std::thread>(&SignatureScanner::scanThread, this, disk, config);
    
    return true;
}

bool SignatureScanner::stopScan() {
    m_scanning = false;
    if (m_thread) {
        if (m_thread->joinable()) {
            m_thread->join();
        }
        m_thread.reset();
    }
    return true;
}

bool SignatureScanner::pauseScan() {
    m_paused = true;
    return true;
}

bool SignatureScanner::resumeScan() {
    m_paused = false;
    return true;
}

std::vector<RecoveredFile> SignatureScanner::getResults() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_results;
}

size_t SignatureScanner::getFileCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_results.size();
}

float SignatureScanner::getProgress() const {
    return m_progress;
}

bool SignatureScanner::isScanning() const {
    return m_scanning;
}

void SignatureScanner::setProgressCallback(std::function<void(float, const std::string&)> callback) {
    m_progressCallback = callback;
}

void SignatureScanner::setFileFoundCallback(std::function<void(const RecoveredFile&)> callback) {
    m_fileFoundCallback = callback;
}

void SignatureScanner::scanThread(const DiskInfo& disk, const ScanConfig& config) {
    Logger::getInstance().log(LogLevel::INFO, "Signature-based scan started");
    
    // Implémentation similaire au DeepScanner mais optimisée pour les signatures
    
    m_scanning = false;
    m_progress = 1.0f;
    
    Logger::getInstance().log(LogLevel::INFO, "Signature scan completed");
}

} // namespace FileRecovery
