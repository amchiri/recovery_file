#ifndef SIGNATURE_SCANNER_H
#define SIGNATURE_SCANNER_H

#include "../include/DiskScanner.h"
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace FileRecovery {

/**
 * Scanner basé uniquement sur les signatures de fichiers
 * Utile pour les disques très corrompus
 */
class SignatureScanner : public DiskScanner {
public:
    SignatureScanner();
    ~SignatureScanner() noexcept override = default;

    bool startScan(const DiskInfo& disk, const ScanConfig& config) override;
    bool stopScan() override;
    bool pauseScan() override;
    bool resumeScan() override;

    std::vector<RecoveredFile> getResults() const override;
    size_t getFileCount() const override;

    float getProgress() const override;
    bool isScanning() const override;

    void setProgressCallback(std::function<void(float, const std::string&)> callback) override;
    void setFileFoundCallback(std::function<void(const RecoveredFile&)> callback) override;

private:
    // Implémentation similaire au DeepScanner mais plus spécialisée
    void scanThread(const DiskInfo& disk, const ScanConfig& config);
    
    std::vector<RecoveredFile> m_results;
    std::unique_ptr<std::thread> m_thread;
    mutable std::mutex m_mutex;
    
    std::function<void(float, const std::string&)> m_progressCallback;
    std::function<void(const RecoveredFile&)> m_fileFoundCallback;
};

} // namespace FileRecovery

#endif // SIGNATURE_SCANNER_H
