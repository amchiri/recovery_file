#include "FilesystemDetector.h"
#include "../utils/DiskIO.h"
#include <cstring>

namespace FileRecovery {

FilesystemType FilesystemDetector::detectFilesystem(DiskIO& disk) {
    ByteArray bootSector(4096); // Lire plus pour détecter différents FS
    
    if (!disk.readSectors(0, 8, bootSector.data())) {
        return FilesystemType::UNKNOWN;
    }
    
    if (isNTFS(bootSector)) {
        return FilesystemType::NTFS;
    }
    
    if (isFAT32(bootSector)) {
        return FilesystemType::FAT32;
    }
    
    if (isexFAT(bootSector)) {
        return FilesystemType::exFAT;
    }
    
    if (isEXT(bootSector)) {
        return FilesystemType::EXT4; // Simplification
    }
    
    return FilesystemType::UNKNOWN;
}

bool FilesystemDetector::isNTFS(const ByteArray& bootSector) {
    if (bootSector.size() < 512) return false;
    return std::memcmp(&bootSector[3], "NTFS    ", 8) == 0;
}

bool FilesystemDetector::isFAT32(const ByteArray& bootSector) {
    if (bootSector.size() < 512) return false;
    return std::memcmp(&bootSector[82], "FAT32   ", 8) == 0;
}

bool FilesystemDetector::isexFAT(const ByteArray& bootSector) {
    if (bootSector.size() < 512) return false;
    return std::memcmp(&bootSector[3], "EXFAT   ", 8) == 0;
}

bool FilesystemDetector::isEXT(const ByteArray& bootSector) {
    if (bootSector.size() < 1024 + 2) return false;
    // Signature EXT à l'offset 0x438
    return bootSector[0x438] == 0x53 && bootSector[0x439] == 0xEF;
}

} // namespace FileRecovery
