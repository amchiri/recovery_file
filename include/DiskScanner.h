#ifndef DISK_SCANNER_H
#define DISK_SCANNER_H

#include "RecoveryTypes.h"
#include <memory>
#include <atomic>
#include <functional>

namespace FileRecovery {

/**
 * Scanner de disque - Interface pour différents types de scanners
 */
class DiskScanner {
public:
    virtual ~DiskScanner() noexcept = default;

    // Démarrage et arrêt du scan
    virtual bool startScan(const DiskInfo& disk, const ScanConfig& config) = 0;
    virtual bool stopScan() = 0;
    virtual bool pauseScan() = 0;
    virtual bool resumeScan() = 0;

    // Résultats
    virtual std::vector<RecoveredFile> getResults() const = 0;
    virtual size_t getFileCount() const = 0;

    // Progression
    virtual float getProgress() const = 0;
    virtual bool isScanning() const = 0;

    // Callbacks
    virtual void setProgressCallback(std::function<void(float, const std::string&)> callback) = 0;
    virtual void setFileFoundCallback(std::function<void(const RecoveredFile&)> callback) = 0;

protected:
    std::atomic<bool> m_scanning{false};
    std::atomic<bool> m_paused{false};
    std::atomic<float> m_progress{0.0f};
};

/**
 * Factory pour créer des scanners
 */
class ScannerFactory {
public:
    static std::unique_ptr<DiskScanner> createScanner(ScanMode mode);
};

} // namespace FileRecovery

#endif // DISK_SCANNER_H
