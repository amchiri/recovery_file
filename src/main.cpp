#include "RecoveryEngine.h"
#include "utils/Logger.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <thread>
#include <chrono>

using namespace FileRecovery;

// Prototypes
void printBanner();
void printUsage();
void printDisks(const std::vector<DiskInfo>& disks);
void printResults(const std::vector<RecoveredFile>& files);
void printStats(const RecoveryStats& stats);
bool parseArguments(int argc, char* argv[], ScanConfig& config, std::string& diskPath);

int main(int argc, char* argv[]) {
    printBanner();
    
    if (argc < 2) {
        printUsage();
        return 1;
    }

    // Initialisation
    Logger::getInstance().setLogLevel(LogLevel::INFO);
    Logger::getInstance().log(LogLevel::INFO, "Starting File Recovery Tool");

    RecoveryEngine engine;
    if (!engine.initialize()) {
        std::cerr << "Failed to initialize recovery engine" << std::endl;
        return 1;
    }

    // Parse arguments
    ScanConfig config;
    std::string diskPath;
    if (!parseArguments(argc, argv, config, diskPath)) {
        printUsage();
        return 1;
    }

    // Liste les disques si demandé
    if (diskPath == "--list" || diskPath.empty()) {
        auto disks = engine.listAvailableDisks();
        printDisks(disks);
        return 0;
    }

    // Sélection du disque
    if (!engine.selectDisk(diskPath)) {
        std::cerr << "Failed to access disk: " << diskPath << std::endl;
        return 1;
    }

    DiskInfo diskInfo = engine.getCurrentDiskInfo();
    std::cout << "\nSelected disk: " << diskInfo.devicePath << std::endl;
    std::cout << "Filesystem: ";
    switch (diskInfo.fsType) {
        case FilesystemType::NTFS: std::cout << "NTFS"; break;
        case FilesystemType::FAT32: std::cout << "FAT32"; break;
        case FilesystemType::exFAT: std::cout << "exFAT"; break;
        default: std::cout << "Unknown"; break;
    }
    std::cout << std::endl;
    std::cout << "Total size: " << (diskInfo.totalSize / (1024*1024*1024)) << " GB" << std::endl;

    // Configuration du moteur
    engine.setConfig(config);

    // Callbacks pour la progression
    engine.setProgressCallback([](float progress, const std::string& status) {
        std::cout << "\r[" << std::setw(3) << (int)(progress * 100) << "%] " 
                  << status << std::flush;
    });

    int fileCount = 0;
    engine.setFileFoundCallback([&fileCount](const RecoveredFile& file) {
        fileCount++;
        if (fileCount % 100 == 0) {
            std::cout << "\rFiles found: " << fileCount << std::flush;
        }
    });

    // Démarrage du scan
    std::cout << "\nStarting scan..." << std::endl;
    if (!engine.startScan()) {
        std::cerr << "Failed to start scan" << std::endl;
        return 1;
    }

    // Attendre que le scan démarre vraiment
    int timeout = 50; // 5 secondes max
    while (!engine.isScanning() && timeout-- > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    if (timeout <= 0) {
        std::cerr << "Scan failed to start" << std::endl;
        return 1;
    }
    
    // Attente de fin de scan
    while (engine.isScanning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "\n\nScan completed!" << std::endl;

    // Résultats
    auto files = engine.getFoundFiles();
    printResults(files);

    // Statistiques
    auto stats = engine.getStats();
    printStats(stats);

    // Récupération si demandée
    if (!config.outputDirectory.empty() && !files.empty()) {
        std::cout << "\nRecovering files to: " << config.outputDirectory << std::endl;
        
        if (engine.recoverAllFiles(config.outputDirectory)) {
            std::cout << "Recovery completed successfully!" << std::endl;
        } else {
            std::cerr << "Recovery failed or partially completed" << std::endl;
        }
    }

    engine.shutdown();
    return 0;
}

void printBanner() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════╗
║         ADVANCED FILE RECOVERY TOOL v1.0                  ║
║         High-Performance Data Recovery System              ║
╚═══════════════════════════════════════════════════════════╝
)" << std::endl;
}

void printUsage() {
    std::cout << R"(
Usage: file_recovery [OPTIONS]

Options:
  --list                    List all available disks
  --scan <device>           Scan the specified device (e.g., C:, D:, \\.\PhysicalDrive0)
  --mode <mode>             Scan mode: quick, deep, signature, complete (default: quick)
  --types <extensions>      File types to recover (e.g., jpg,png,pdf,docx)
  --min-size <bytes>        Minimum file size to recover
  --max-size <bytes>        Maximum file size to recover
  --output <directory>      Output directory for recovered files
  --threads <count>         Number of threads to use (default: 4)
  --deep                    Enable deep scan
  --no-verify               Skip signature verification
  --help                    Show this help message

Examples:
  file_recovery --list
  file_recovery --scan C: --output recovered/
  file_recovery --scan D: --mode deep --types jpg,png,pdf --output recovered/
  file_recovery --scan \\.\PhysicalDrive1 --mode complete --threads 8
)" << std::endl;
}

void printDisks(const std::vector<DiskInfo>& disks) {
    std::cout << "\nAvailable disks:\n" << std::endl;
    std::cout << std::setw(15) << "Device" 
              << std::setw(15) << "Volume"
              << std::setw(12) << "Filesystem"
              << std::setw(12) << "Size (GB)"
              << std::setw(10) << "Status" << std::endl;
    std::cout << std::string(64, '-') << std::endl;

    for (const auto& disk : disks) {
        std::cout << std::setw(15) << disk.devicePath
                  << std::setw(15) << disk.volumeName
                  << std::setw(12);
        
        switch (disk.fsType) {
            case FilesystemType::NTFS: std::cout << "NTFS"; break;
            case FilesystemType::FAT32: std::cout << "FAT32"; break;
            case FilesystemType::exFAT: std::cout << "exFAT"; break;
            default: std::cout << "Unknown"; break;
        }
        
        std::cout << std::setw(12) << (disk.totalSize / (1024*1024*1024))
                  << std::setw(10) << (disk.isHealthy ? "Healthy" : "Warning")
                  << std::endl;
    }
}

void printResults(const std::vector<RecoveredFile>& files) {
    std::cout << "\n=== Scan Results ===" << std::endl;
    std::cout << "Total files found: " << files.size() << std::endl;

    if (files.empty()) {
        std::cout << "No recoverable files found." << std::endl;
        return;
    }

    // Statistiques par type
    std::map<std::string, int> typeCount;
    for (const auto& file : files) {
        typeCount[file.detectedType]++;
    }

    std::cout << "\nFiles by type:" << std::endl;
    for (const auto& [type, count] : typeCount) {
        std::cout << "  " << type << ": " << count << std::endl;
    }

    // Top 10 des plus gros fichiers
    std::cout << "\nTop 10 largest files:" << std::endl;
    auto sortedFiles = files;
    size_t topCount = std::min(static_cast<size_t>(10), sortedFiles.size());
    std::partial_sort(sortedFiles.begin(), 
                     sortedFiles.begin() + topCount,
                     sortedFiles.end(),
                     [](const auto& a, const auto& b) { return a.fileSize > b.fileSize; });

    for (size_t i = 0; i < topCount; ++i) {
        const auto& file = sortedFiles[i];
        std::cout << "  " << (i+1) << ". " << file.originalName 
                  << " (" << (file.fileSize / 1024) << " KB, " 
                  << file.detectedType << ")" << std::endl;
    }
}

void printStats(const RecoveryStats& stats) {
    std::cout << "\n=== Statistics ===" << std::endl;
    std::cout << "Files scanned: " << stats.filesScanned << std::endl;
    std::cout << "Files found: " << stats.filesFound << std::endl;
    std::cout << "Files recovered: " << stats.filesRecovered << std::endl;
    std::cout << "Bytes copied: " << (stats.bytesCopied / (1024*1024)) << " MB" << std::endl;
    std::cout << "Sectors read: " << stats.sectorsRead << std::endl;
    std::cout << "Scan duration: " << stats.scanDuration.count() << " ms" << std::endl;
    std::cout << "Recovery duration: " << stats.recoveryDuration.count() << " ms" << std::endl;
}

bool parseArguments(int argc, char* argv[], ScanConfig& config, std::string& diskPath) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--list") {
            diskPath = "--list";
        } else if ((arg == "--scan" || arg == "--disk") && i + 1 < argc) {
            diskPath = argv[++i];
        } else if (arg == "--mode" && i + 1 < argc) {
            std::string mode = argv[++i];
            if (mode == "quick") config.mode = ScanMode::QUICK;
            else if (mode == "deep") config.mode = ScanMode::DEEP;
            else if (mode == "signature") config.mode = ScanMode::SIGNATURE;
            else if (mode == "complete") config.mode = ScanMode::COMPLETE;
        } else if (arg == "--types" && i + 1 < argc) {
            std::string types = argv[++i];
            // Parse comma-separated extensions
            size_t pos = 0;
            while ((pos = types.find(',')) != std::string::npos) {
                config.targetExtensions.push_back(types.substr(0, pos));
                types.erase(0, pos + 1);
            }
            if (!types.empty()) {
                config.targetExtensions.push_back(types);
            }
        } else if (arg == "--extension" && i + 1 < argc) {
            // Single extension
            config.targetExtensions.push_back(argv[++i]);
        } else if (arg == "--output" && i + 1 < argc) {
            config.outputDirectory = argv[++i];
        } else if (arg == "--threads" && i + 1 < argc) {
            config.threadCount = std::stoi(argv[++i]);
        } else if (arg == "--deep") {
            config.deepScan = true;
        } else if (arg == "--no-verify") {
            config.verifySignatures = false;
        } else if (arg == "--min-size" && i + 1 < argc) {
            config.minFileSize = std::stoull(argv[++i]);
        } else if (arg == "--max-size" && i + 1 < argc) {
            config.maxFileSize = std::stoull(argv[++i]);
        } else if (arg == "--help") {
            return false;
        }
    }
    
    return true;
}
