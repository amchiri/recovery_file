#include "QuickScanner.h"
#include "../utils/DiskIO.h"
#include "../utils/Logger.h"

namespace FileRecovery {

QuickScanner::QuickScanner() {}

bool QuickScanner::startScan(const DiskInfo& disk, const ScanConfig& config) {
    if (m_scanning) {
        return false;
    }
    
    m_scanning = true;
    m_paused = false;
    m_progress = 0.0f;
    
    m_thread = std::make_unique<std::thread>(&QuickScanner::scanThread, this, disk, config);
    
    return true;
}

bool QuickScanner::stopScan() {
    m_scanning = false;
    if (m_thread) {
        if (m_thread->joinable()) {
            m_thread->join();
        }
        m_thread.reset();
    }
    return true;
}

bool QuickScanner::pauseScan() {
    m_paused = true;
    return true;
}

bool QuickScanner::resumeScan() {
    m_paused = false;
    return true;
}

std::vector<RecoveredFile> QuickScanner::getResults() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_results;
}

size_t QuickScanner::getFileCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_results.size();
}

float QuickScanner::getProgress() const {
    return m_progress;
}

bool QuickScanner::isScanning() const {
    return m_scanning;
}

void QuickScanner::setProgressCallback(std::function<void(float, const std::string&)> callback) {
    m_progressCallback = callback;
}

void QuickScanner::setFileFoundCallback(std::function<void(const RecoveredFile&)> callback) {
    m_fileFoundCallback = callback;
}

void QuickScanner::scanThread(const DiskInfo& disk, const ScanConfig& config) {
    Logger::getInstance().log(LogLevel::INFO, "Quick scan started");
    
    bool success = false;
    
    switch (disk.fsType) {
        case FilesystemType::NTFS:
            success = scanNTFS(disk, config);
            break;
        case FilesystemType::FAT32:
            success = scanFAT32(disk, config);
            break;
        default:
            Logger::getInstance().log(LogLevel::ERROR, "Unsupported filesystem");
            break;
    }
    
    m_scanning = false;
    m_progress = 1.0f;
    
    if (m_progressCallback) {
        m_progressCallback(1.0f, "Scan complete");
    }
    
    Logger::getInstance().log(LogLevel::INFO, 
        "Quick scan completed. Found " + std::to_string(m_results.size()) + " files");
}

bool QuickScanner::scanNTFS(const DiskInfo& disk, const ScanConfig& config) {
    DiskIO diskIO;
    if (!diskIO.openDevice(disk.devicePath)) {
        return false;
    }
    
    NTFSParser parser;
    if (!parser.initialize(diskIO)) {
        return false;
    }
    
    if (m_progressCallback) {
        m_progressCallback(0.1f, "Parsing MFT...");
    }
    
    if (!parser.parseMFT()) {
        return false;
    }
    
    auto deletedFiles = parser.getDeletedFiles();
    
    std::lock_guard<std::mutex> lock(m_mutex);
    m_results = deletedFiles;
    
    // Notifier pour chaque fichier trouvé
    if (m_fileFoundCallback) {
        for (const auto& file : deletedFiles) {
            m_fileFoundCallback(file);
        }
    }
    
    return true;
}

bool QuickScanner::scanFAT32(const DiskInfo& disk, const ScanConfig& config) {
    DiskIO diskIO;
    if (!diskIO.openDevice(disk.devicePath)) {
        return false;
    }
    
    FAT32Parser parser;
    if (!parser.initialize(diskIO)) {
        return false;
    }
    
    if (m_progressCallback) {
        m_progressCallback(0.1f, "Parsing FAT...");
    }
    
    if (!parser.parseRootDirectory()) {
        return false;
    }
    
    auto deletedFiles = parser.getDeletedFiles();
    
    std::lock_guard<std::mutex> lock(m_mutex);
    m_results = deletedFiles;
    
    if (m_fileFoundCallback) {
        for (const auto& file : deletedFiles) {
            m_fileFoundCallback(file);
        }
    }
    
    return true;
}

} // namespace FileRecovery
