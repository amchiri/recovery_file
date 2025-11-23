#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include <chrono>
#include "RecoveryTypes.h"
#include "FileQuality.h"
#include "ResultsPagination.h"
#include "BatchOperations.h"
#include "utils/FilePreview.h"
#include "utils/DuplicateDetector.h"

// Forward declarations
namespace FileRecovery {
    class DiskScanner;  // Base class for DeepScanner and QuickScanner
    class RecoveryManager;
}

namespace FileRecovery {
namespace GUI {

struct RecoveredFileInfo {
    std::string filename;
    size_t size;
    FileQualityLevel quality;
    std::string path;
    std::string type;
    float zeroPercentage;
};

class RecoveryGUI {
public:
    RecoveryGUI();
    ~RecoveryGUI();

    bool initialize();
    void run();
    void shutdown();

private:
    void renderMainWindow();
    void renderConfigPanel();
    void renderProgressPanel();
    void renderResultsPanel();
    void renderFileDetails();
    void renderPreviewPanel();
    void renderDuplicatePanel();
    
    void startRecovery();
    void stopRecovery();
    void updateProgress();
    
    void applyModernTheme();
    void detectAvailableDisks();
    void initializeExtensions();
    
    // Recovery parameters
    std::vector<std::string> availableDisks_;
    int selectedDiskIndex_ = 0;
    char outputPath_[512] = ".\\recovered_files";  // Default: current directory

    // Extensions with checkboxes
    struct ExtensionInfo {
        std::string name;
        std::string description;
        bool selected;
    };
    std::vector<ExtensionInfo> extensions_;

    char filter_[128] = "";
    int threads_ = 4;
    bool deepScan_ = true;
    bool scanSpecificFolder_ = false;  // Scan specific folder instead of entire disk
    char scanFolderPath_[512] = "";  // Empty by default, user must specify
    
    // Recovery state
    bool isRecovering_ = false;
    bool stopRequested_ = false;
    std::atomic<float> progress_{0.0f};
    std::atomic<int> filesFound_{0};
    std::atomic<size_t> bytesScanned_{0};
    size_t totalBytes_ = 0;
    
    // Performance metrics
    std::chrono::steady_clock::time_point scanStartTime_;
    std::atomic<float> scanSpeedMBps_{0.0f};
    
    std::vector<RecoveredFileInfo> recoveredFiles_;
    int selectedFileIndex_ = -1;
    
    // Filters and sorting
    char searchFilter_[256] = "";
    bool filterByQuality_[5] = {true, true, true, true, true}; // EXCELLENT, GOOD, POOR, CORRUPTED, DEAD
    int sortColumn_ = 0; // 0=Filename, 1=Size, 2=Quality, 3=Zeros
    bool sortAscending_ = true;
    
    // Phase 8: Optimization options
    bool enableSmartScan_ = true;
    bool useLargeBuffers_ = false;
    std::atomic<uint64_t> sectorsSkipped_{0};
    std::chrono::steady_clock::time_point etaStartTime_;
    uint64_t etaBytesAtStart_ = 0;
    
    // Phase 9: Pagination and Batch Operations
    std::unique_ptr<class ResultsPagination> pagination_;
    std::unique_ptr<class BatchOperations> batchOps_;
    
    // Recovery engine
    std::unique_ptr<std::thread> recoveryThread_;
    std::unique_ptr<DiskScanner> scanner_;  // Changed from DeepScanner to DiskScanner (base class)
    std::unique_ptr<RecoveryManager> recoveryManager_;
    
    // Thread safety
    std::mutex dataMutex_;

    // Preview system
    std::unique_ptr<Utils::FilePreview> filePreview_;
    Utils::PreviewData currentPreview_;
    bool showPreview_ = false;

    // Duplicate detection
    std::unique_ptr<Utils::DuplicateDetector> duplicateDetector_;
    std::vector<Utils::DuplicateGroup> duplicateGroups_;
    bool showDuplicates_ = false;
    bool duplicatesAnalyzed_ = false;

    // UI state
    bool showAbout_ = false;
    float progressHistory_[100] = {0};
    int progressHistoryIndex_ = 0;
};

} // namespace GUI
} // namespace FileRecovery
