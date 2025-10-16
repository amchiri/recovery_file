#ifndef RECOVERY_ENGINE_H
#define RECOVERY_ENGINE_H

#include "RecoveryTypes.h"
#include <memory>
#include <functional>
#include <mutex>

namespace FileRecovery {

// Forward declarations
class DiskScanner;
class RecoveryManager;
class SignatureDatabase;

/**
 * Moteur principal de récupération
 * Orchestre toutes les opérations de scan et récupération
 */
class RecoveryEngine {
public:
    RecoveryEngine();
    ~RecoveryEngine();

    // Initialisation
    bool initialize();
    bool shutdown();

    // Gestion des disques
    std::vector<DiskInfo> listAvailableDisks();
    bool selectDisk(const std::string& devicePath);
    DiskInfo getCurrentDiskInfo() const;

    // Configuration
    void setConfig(const ScanConfig& config);
    ScanConfig getConfig() const;

    // Scanning
    bool startScan();
    bool stopScan();
    bool pauseScan();
    bool resumeScan();
    
    // Récupération
    std::vector<RecoveredFile> getFoundFiles() const;
    bool recoverFile(const RecoveredFile& file, const std::string& outputPath);
    bool recoverAllFiles(const std::string& outputDir);
    bool recoverSelectedFiles(const std::vector<RecoveredFile>& files, const std::string& outputDir);

    // Statistiques et progression
    RecoveryStats getStats() const;
    float getProgress() const; // 0.0 à 1.0
    bool isScanning() const;

    // Callbacks pour la progression
    using ProgressCallback = std::function<void(float progress, const std::string& status)>;
    using FileFoundCallback = std::function<void(const RecoveredFile& file)>;
    
    void setProgressCallback(ProgressCallback callback);
    void setFileFoundCallback(FileFoundCallback callback);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;

    // Non-copyable
    RecoveryEngine(const RecoveryEngine&) = delete;
    RecoveryEngine& operator=(const RecoveryEngine&) = delete;
};

} // namespace FileRecovery

#endif // RECOVERY_ENGINE_H
