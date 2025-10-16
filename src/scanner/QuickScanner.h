#ifndef QUICK_SCANNER_H
#define QUICK_SCANNER_H

#include "../include/DiskScanner.h"
#include "../filesystem/NTFSParser.h"
#include "../filesystem/FAT32Parser.h"
#include <thread>
#include <vector>
#include <mutex>
#include <memory>
#include <functional>

namespace FileRecovery {

/**
 * Scanner rapide utilisant les structures du système de fichiers (MFT, FAT, etc.)
 */
class QuickScanner : public DiskScanner {
public:
    QuickScanner();
    ~QuickScanner() noexcept override = default;

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
    void scanThread(const DiskInfo& disk, const ScanConfig& config);
    bool scanNTFS(const DiskInfo& disk, const ScanConfig& config);
    bool scanFAT32(const DiskInfo& disk, const ScanConfig& config);

    std::vector<RecoveredFile> m_results;
    mutable std::mutex m_mutex;
    std::unique_ptr<std::thread> m_thread;
    
    std::function<void(float, const std::string&)> m_progressCallback;
    std::function<void(const RecoveredFile&)> m_fileFoundCallback;
};

} // namespace FileRecovery

#endif // QUICK_SCANNER_H
