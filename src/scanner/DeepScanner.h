#ifndef DEEP_SCANNER_H
#define DEEP_SCANNER_H

#include "../include/DiskScanner.h"
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <memory>
#include <functional>

namespace FileRecovery {

/**
 * Scanner profond - scanne secteur par secteur en cherchant des signatures
 * Plus lent mais plus exhaustif que le QuickScanner
 */
class DeepScanner : public DiskScanner {
public:
    DeepScanner();
    ~DeepScanner() noexcept override = default;

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
    
    // Phase 8: Optimization options
    void setSmartScanEnabled(bool enabled) { m_smartScanEnabled = enabled; }
    void setLargeBuffersEnabled(bool enabled) { m_largeBuffersEnabled = enabled; }
    uint64_t getSectorsSkipped() const { return m_sectorsSkipped.load(); }

private:
    void scanThread(const DiskInfo& disk, const ScanConfig& config);
    void scanSectorRange(uint64_t startSector, uint64_t endSector, 
                        const DiskInfo& disk, const ScanConfig& config);
    bool isEmptySector(const ByteArray& buffer, size_t offset, size_t length) const;

    std::vector<RecoveredFile> m_results;
    mutable std::mutex m_mutex;
    std::vector<std::unique_ptr<std::thread>> m_threads;
    
    std::atomic<uint64_t> m_sectorsScanned{0};
    uint64_t m_totalSectors{0};
    
    // Phase 8: Optimization features
    bool m_smartScanEnabled{true};
    bool m_largeBuffersEnabled{false};
    std::atomic<uint64_t> m_sectorsSkipped{0};
    
    std::function<void(float, const std::string&)> m_progressCallback;
    std::function<void(const RecoveredFile&)> m_fileFoundCallback;
};

} // namespace FileRecovery

#endif // DEEP_SCANNER_H
