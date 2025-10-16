#include "../include/RecoveryEngine.h"
#include "../include/DiskScanner.h"
#include "../include/FileSignatures.h"
#include "../recovery/RecoveryManager.h"
#include "../signatures/SignatureDatabase.h"
#include "../filesystem/FilesystemDetector.h"
#include "../utils/DiskIO.h"
#include "../utils/Logger.h"
#include <thread>
#include <algorithm>
#include <mutex>

namespace FileRecovery {

class RecoveryEngine::Impl {
public:
    std::unique_ptr<DiskScanner> scanner;
    std::unique_ptr<RecoveryManager> recoveryManager;
    std::unique_ptr<DiskIO> diskIO;
    
    DiskInfo currentDisk;
    ScanConfig config;
    RecoveryStats stats;
    
    ProgressCallback progressCallback;
    FileFoundCallback fileFoundCallback;
    
    std::mutex mutex;
    bool initialized = false;
    std::unique_ptr<std::thread> scanThread;  // Garder le thread de scan

    Impl() 
        : recoveryManager(std::make_unique<RecoveryManager>()),
          diskIO(std::make_unique<DiskIO>()) {}
};

RecoveryEngine::RecoveryEngine() : pImpl(std::make_unique<Impl>()) {}

RecoveryEngine::~RecoveryEngine() {
    shutdown();
}

bool RecoveryEngine::initialize() {
    Logger::getInstance().log(LogLevel::INFO, "Initializing Recovery Engine");
    
    // Initialiser la base de signatures - appel pour forcer l'initialisation
    FileSignatures& sigDb = FileSignatures::getInstance();
    (void)sigDb; // Éviter warning unused
    
    pImpl->initialized = true;
    return true;
}

bool RecoveryEngine::shutdown() {
    if (!pImpl->initialized) return true;
    
    Logger::getInstance().log(LogLevel::INFO, "Shutting down Recovery Engine");
    
    if (pImpl->scanner && pImpl->scanner->isScanning()) {
        pImpl->scanner->stopScan();
    }
    
    // Attendre que le thread de scan se termine
    if (pImpl->scanThread && pImpl->scanThread->joinable()) {
        pImpl->scanThread->join();
    }
    
    pImpl->initialized = false;
    return true;
}

std::vector<DiskInfo> RecoveryEngine::listAvailableDisks() {
    return pImpl->diskIO->enumerateDisks();
}

bool RecoveryEngine::selectDisk(const std::string& devicePath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    Logger::getInstance().log(LogLevel::INFO, "Selecting disk: " + devicePath);
    
    if (!pImpl->diskIO->openDevice(devicePath)) {
        Logger::getInstance().log(LogLevel::ERROR, "Failed to open device: " + devicePath);
        return false;
    }
    
    pImpl->currentDisk = pImpl->diskIO->getDiskInfo();
    
    // Détection du système de fichiers
    FilesystemDetector detector;
    pImpl->currentDisk.fsType = detector.detectFilesystem(*pImpl->diskIO);
    
    Logger::getInstance().log(LogLevel::INFO, "Disk selected successfully");
    return true;
}

DiskInfo RecoveryEngine::getCurrentDiskInfo() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentDisk;
}

void RecoveryEngine::setConfig(const ScanConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config = config;
}

ScanConfig RecoveryEngine::getConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->config;
}

bool RecoveryEngine::startScan() {
    if (!pImpl->initialized) {
        Logger::getInstance().log(LogLevel::ERROR, "Engine not initialized");
        return false;
    }
    
    if (pImpl->scanner && pImpl->scanner->isScanning()) {
        Logger::getInstance().log(LogLevel::WARNING, "Scan already in progress");
        return false;
    }
    
    Logger::getInstance().log(LogLevel::INFO, "Starting scan");
    
    // Créer le scanner approprié
    pImpl->scanner = ScannerFactory::createScanner(pImpl->config.mode);
    
    // Configurer les callbacks
    if (pImpl->progressCallback) {
        pImpl->scanner->setProgressCallback(pImpl->progressCallback);
    }
    
    if (pImpl->fileFoundCallback) {
        pImpl->scanner->setFileFoundCallback(pImpl->fileFoundCallback);
    }
    
    // Démarrer le scan dans un thread séparé
    pImpl->scanThread = std::make_unique<std::thread>([this]() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        bool success = pImpl->scanner->startScan(pImpl->currentDisk, pImpl->config);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        pImpl->stats.scanDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
            endTime - startTime);
        
        if (success) {
            Logger::getInstance().log(LogLevel::INFO, "Scan completed successfully");
        } else {
            Logger::getInstance().log(LogLevel::ERROR, "Scan failed");
        }
    });
    
    // Ne pas detacher - garder le thread vivant
    
    return true;
}

bool RecoveryEngine::stopScan() {
    if (pImpl->scanner) {
        Logger::getInstance().log(LogLevel::INFO, "Stopping scan");
        return pImpl->scanner->stopScan();
    }
    return false;
}

bool RecoveryEngine::pauseScan() {
    if (pImpl->scanner) {
        Logger::getInstance().log(LogLevel::INFO, "Pausing scan");
        return pImpl->scanner->pauseScan();
    }
    return false;
}

bool RecoveryEngine::resumeScan() {
    if (pImpl->scanner) {
        Logger::getInstance().log(LogLevel::INFO, "Resuming scan");
        return pImpl->scanner->resumeScan();
    }
    return false;
}

std::vector<RecoveredFile> RecoveryEngine::getFoundFiles() const {
    if (pImpl->scanner) {
        return pImpl->scanner->getResults();
    }
    return {};
}

bool RecoveryEngine::recoverFile(const RecoveredFile& file, const std::string& outputPath) {
    Logger::getInstance().log(LogLevel::INFO, "Recovering file: " + file.originalName);
    return pImpl->recoveryManager->recoverFile(file, outputPath, *pImpl->diskIO);
}

bool RecoveryEngine::recoverAllFiles(const std::string& outputDir) {
    auto files = getFoundFiles();
    Logger::getInstance().log(LogLevel::INFO, 
        "Recovering " + std::to_string(files.size()) + " files to " + outputDir);
    
    return pImpl->recoveryManager->recoverMultipleFiles(files, outputDir, *pImpl->diskIO);
}

bool RecoveryEngine::recoverSelectedFiles(const std::vector<RecoveredFile>& files, 
                                         const std::string& outputDir) {
    Logger::getInstance().log(LogLevel::INFO, 
        "Recovering " + std::to_string(files.size()) + " selected files to " + outputDir);
    
    return pImpl->recoveryManager->recoverMultipleFiles(files, outputDir, *pImpl->diskIO);
}

RecoveryStats RecoveryEngine::getStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->scanner) {
        pImpl->stats.filesFound = pImpl->scanner->getFileCount();
    }
    
    return pImpl->stats;
}

float RecoveryEngine::getProgress() const {
    if (pImpl->scanner) {
        return pImpl->scanner->getProgress();
    }
    return 0.0f;
}

bool RecoveryEngine::isScanning() const {
    return pImpl->scanner && pImpl->scanner->isScanning();
}

void RecoveryEngine::setProgressCallback(ProgressCallback callback) {
    pImpl->progressCallback = callback;
}

void RecoveryEngine::setFileFoundCallback(FileFoundCallback callback) {
    pImpl->fileFoundCallback = callback;
}

} // namespace FileRecovery
