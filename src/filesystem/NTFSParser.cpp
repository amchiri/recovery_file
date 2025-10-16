#include "NTFSParser.h"
#include "../utils/DiskIO.h"
#include "../utils/Logger.h"
#include <cstring>
#include <algorithm>

namespace FileRecovery {

class NTFSParser::Impl {
public:
    DiskIO* disk = nullptr;
    NTFSBootSector bootSector;
    uint64_t mftOffset = 0;
    uint32_t clusterSize = 0;
    uint32_t mftRecordSize = 0;
    bool valid = false;
    std::vector<RecoveredFile> deletedFiles;
};

NTFSParser::NTFSParser() : pImpl(std::make_unique<Impl>()) {}

NTFSParser::~NTFSParser() = default;

bool NTFSParser::initialize(DiskIO& disk) {
    pImpl->disk = &disk;
    
    Logger::getInstance().log(LogLevel::INFO, "Initializing NTFS parser");
    
    // Lire le boot sector
    ByteArray bootSectorData(512);
    if (!disk.readSectors(0, 1, bootSectorData.data())) {
        Logger::getInstance().log(LogLevel::ERROR, "Failed to read boot sector");
        return false;
    }
    
    std::memcpy(&pImpl->bootSector, bootSectorData.data(), sizeof(NTFSBootSector));
    
    // Vérifier la signature NTFS
    if (std::memcmp(pImpl->bootSector.oemId, "NTFS    ", 8) != 0) {
        Logger::getInstance().log(LogLevel::ERROR, "Not a valid NTFS volume");
        return false;
    }
    
    // Calculer les paramètres
    pImpl->clusterSize = pImpl->bootSector.bytesPerSector * 
                         pImpl->bootSector.sectorsPerCluster;
    
    pImpl->mftOffset = pImpl->bootSector.mftClusterNumber * pImpl->clusterSize;
    
    // Taille d'un enregistrement MFT (généralement 1024 octets)
    if (pImpl->bootSector.clustersPerMFTRecord < 0) {
        pImpl->mftRecordSize = 1 << (-pImpl->bootSector.clustersPerMFTRecord);
    } else {
        pImpl->mftRecordSize = pImpl->bootSector.clustersPerMFTRecord * pImpl->clusterSize;
    }
    
    pImpl->valid = true;
    
    Logger::getInstance().log(LogLevel::INFO, 
        "NTFS initialized - Cluster size: " + std::to_string(pImpl->clusterSize) +
        ", MFT offset: " + std::to_string(pImpl->mftOffset));
    
    return true;
}

bool NTFSParser::isValid() const {
    return pImpl->valid;
}

bool NTFSParser::parseMFT() {
    if (!pImpl->valid || !pImpl->disk) {
        return false;
    }
    
    Logger::getInstance().log(LogLevel::INFO, "Parsing MFT for deleted files");
    
    pImpl->deletedFiles.clear();
    
    // Lire les enregistrements MFT
    // Note: Une implémentation complète lirait toute la MFT
    // Pour cet exemple, nous lisons les 1000 premiers enregistrements
    
    const uint64_t maxRecords = 10000;
    const uint64_t recordSize = pImpl->mftRecordSize;
    
    ByteArray buffer(recordSize);
    
    for (uint64_t i = 0; i < maxRecords; ++i) {
        uint64_t offset = pImpl->mftOffset + (i * recordSize);
        
        if (!pImpl->disk->readBytes(offset, recordSize, buffer.data())) {
            continue;
        }
        
        MFTRecordHeader* header = reinterpret_cast<MFTRecordHeader*>(buffer.data());
        
        // Vérifier la signature FILE
        if (header->signature != 0x454C4946) { // 'FILE'
            continue;
        }
        
        // Fichier supprimé si le flag "in use" n'est pas activé
        bool isDeleted = (header->flags & 0x01) == 0;
        bool isDirectory = (header->flags & 0x02) != 0;
        
        if (isDeleted && !isDirectory) {
            RecoveredFile file;
            file.diskOffset = offset;
            file.status = RecoveryStatus::FOUND;
            file.originalName = "MFT_Record_" + std::to_string(i);
            
            // Parser les attributs pour obtenir plus d'infos
            // (Implémentation simplifiée)
            
            pImpl->deletedFiles.push_back(file);
            
            if (pImpl->deletedFiles.size() % 100 == 0) {
                Logger::getInstance().log(LogLevel::DEBUG, 
                    "Found " + std::to_string(pImpl->deletedFiles.size()) + " deleted files");
            }
        }
    }
    
    Logger::getInstance().log(LogLevel::INFO, 
        "MFT parsing complete. Found " + std::to_string(pImpl->deletedFiles.size()) + 
        " deleted files");
    
    return true;
}

std::vector<RecoveredFile> NTFSParser::getDeletedFiles() const {
    return pImpl->deletedFiles;
}

uint64_t NTFSParser::getMFTOffset() const {
    return pImpl->mftOffset;
}

uint32_t NTFSParser::getClusterSize() const {
    return pImpl->clusterSize;
}

uint32_t NTFSParser::getBytesPerSector() const {
    return pImpl->bootSector.bytesPerSector;
}

uint64_t NTFSParser::getTotalClusters() const {
    return pImpl->bootSector.totalSectors / pImpl->bootSector.sectorsPerCluster;
}

} // namespace FileRecovery
