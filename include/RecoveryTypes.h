#ifndef RECOVERY_TYPES_H
#define RECOVERY_TYPES_H

#include <string>
#include <vector>
#include <cstdint>
#include <chrono>
#include <memory>

namespace FileRecovery {

// Types de base
using Byte = uint8_t;
using ByteArray = std::vector<Byte>;
using Offset = uint64_t;
using Size = uint64_t;

// Énumération des systèmes de fichiers
enum class FilesystemType {
    UNKNOWN,
    NTFS,
    FAT12,
    FAT16,
    FAT32,
    exFAT,
    EXT2,
    EXT3,
    EXT4,
    HFS_PLUS,
    APFS
};

// État de récupération d'un fichier
enum class RecoveryStatus {
    PENDING,
    SCANNING,
    FOUND,
    RECOVERING,
    RECOVERED,
    FAILED,
    CORRUPTED
};

// Qualité de récupération
enum class RecoveryQuality {
    EXCELLENT,    // 100% récupérable
    GOOD,         // 75-99% récupérable
    FAIR,         // 50-74% récupérable
    POOR,         // 25-49% récupérable
    BAD           // <25% récupérable
};

// Type de scan
enum class ScanMode {
    QUICK,        // Scan rapide via MFT/FAT
    DEEP,         // Scan profond secteur par secteur
    SIGNATURE,    // Scan par signatures uniquement
    COMPLETE      // Combinaison de tous les modes
};

// Structure représentant un fichier trouvé
struct RecoveredFile {
    std::string originalPath;
    std::string originalName;
    std::string detectedType;
    Size fileSize;
    Offset diskOffset;
    std::chrono::system_clock::time_point creationTime;
    std::chrono::system_clock::time_point modificationTime;
    std::chrono::system_clock::time_point deletionTime;
    RecoveryStatus status;
    RecoveryQuality quality;
    bool isFragmented;
    std::vector<Offset> fragmentOffsets;
    ByteArray signature;
    
    RecoveredFile() 
        : fileSize(0), diskOffset(0), status(RecoveryStatus::PENDING),
          quality(RecoveryQuality::GOOD), isFragmented(false) {}
};

// Structure d'information sur un disque
struct DiskInfo {
    std::string devicePath;
    std::string volumeName;
    FilesystemType fsType;
    Size totalSize;
    Size usedSize;
    Size freeSize;
    uint32_t clusterSize;
    uint32_t sectorSize;
    bool isHealthy;
    
    DiskInfo() 
        : fsType(FilesystemType::UNKNOWN), totalSize(0), usedSize(0),
          freeSize(0), clusterSize(0), sectorSize(0), isHealthy(true) {}
};

// Configuration de scan
struct ScanConfig {
    ScanMode mode;
    std::vector<std::string> targetExtensions;
    Size minFileSize;
    Size maxFileSize;
    int threadCount;
    bool deepScan;
    bool verifySignatures;
    bool recoverFragmented;
    std::string outputDirectory;
    
    ScanConfig() 
        : mode(ScanMode::QUICK), minFileSize(0), maxFileSize(SIZE_MAX),
          threadCount(4), deepScan(false), verifySignatures(true),
          recoverFragmented(true), outputDirectory("recovered") {}
};

// Statistiques de récupération
struct RecoveryStats {
    uint64_t filesScanned;
    uint64_t filesFound;
    uint64_t filesRecovered;
    uint64_t bytesCopied;
    uint64_t sectorsRead;
    std::chrono::milliseconds scanDuration;
    std::chrono::milliseconds recoveryDuration;
    
    RecoveryStats() 
        : filesScanned(0), filesFound(0), filesRecovered(0),
          bytesCopied(0), sectorsRead(0), scanDuration(0), recoveryDuration(0) {}
};

// Signature de fichier pour la détection
struct FileSignature {
    std::string extension;
    std::string mimeType;
    std::string description;
    ByteArray header;
    ByteArray footer;
    Offset headerOffset;
    Offset footerOffset;
    Size maxSize;
    
    FileSignature() 
        : headerOffset(0), footerOffset(0), maxSize(SIZE_MAX) {}
};

} // namespace FileRecovery

#endif // RECOVERY_TYPES_H
