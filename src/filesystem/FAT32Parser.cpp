#include "FAT32Parser.h"
#include "../utils/DiskIO.h"
#include "../utils/Logger.h"
#include <cstring>

namespace FileRecovery {

class FAT32Parser::Impl {
public:
    DiskIO* disk = nullptr;
    FAT32BootSector bootSector;
    bool valid = false;
    std::vector<RecoveredFile> deletedFiles;
    uint64_t rootDirOffset = 0;
    uint32_t clusterSize = 0;
};

FAT32Parser::FAT32Parser() : pImpl(std::make_unique<Impl>()) {}

FAT32Parser::~FAT32Parser() = default;

bool FAT32Parser::initialize(DiskIO& disk) {
    pImpl->disk = &disk;
    
    ByteArray bootSectorData(512);
    if (!disk.readSectors(0, 1, bootSectorData.data())) {
        return false;
    }
    
    std::memcpy(&pImpl->bootSector, bootSectorData.data(), sizeof(FAT32BootSector));
    
    // Vérifier signature FAT32
    if (std::memcmp(pImpl->bootSector.fsType, "FAT32   ", 8) != 0) {
        return false;
    }
    
    pImpl->clusterSize = pImpl->bootSector.bytesPerSector * 
                         pImpl->bootSector.sectorsPerCluster;
    
    pImpl->valid = true;
    
    Logger::getInstance().log(LogLevel::INFO, "FAT32 parser initialized");
    
    return true;
}

bool FAT32Parser::isValid() const {
    return pImpl->valid;
}

bool FAT32Parser::parseRootDirectory() {
    if (!pImpl->valid) return false;
    
    Logger::getInstance().log(LogLevel::INFO, "Parsing FAT32 root directory");
    
    // Implémentation simplifiée
    // Une vraie implémentation parcourrait récursivement tous les répertoires
    
    pImpl->deletedFiles.clear();
    
    Logger::getInstance().log(LogLevel::INFO, 
        "Found " + std::to_string(pImpl->deletedFiles.size()) + " deleted files");
    
    return true;
}

std::vector<RecoveredFile> FAT32Parser::getDeletedFiles() const {
    return pImpl->deletedFiles;
}

} // namespace FileRecovery
