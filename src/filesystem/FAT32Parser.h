#ifndef FAT32_PARSER_H
#define FAT32_PARSER_H

#include "../include/RecoveryTypes.h"
#include <vector>
#include <memory>

namespace FileRecovery {

class DiskIO;

/**
 * Parser pour système de fichiers FAT32
 */
class FAT32Parser {
public:
    FAT32Parser();
    ~FAT32Parser();

    bool initialize(DiskIO& disk);
    bool isValid() const;
    
    bool parseRootDirectory();
    std::vector<RecoveredFile> getDeletedFiles() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

#pragma pack(push, 1)

struct FAT32BootSector {
    uint8_t jumpBoot[3];
    uint8_t oemName[8];
    uint16_t bytesPerSector;
    uint8_t sectorsPerCluster;
    uint16_t reservedSectors;
    uint8_t numFATs;
    uint16_t rootEntryCount;
    uint16_t totalSectors16;
    uint8_t media;
    uint16_t fatSize16;
    uint16_t sectorsPerTrack;
    uint16_t numberOfHeads;
    uint32_t hiddenSectors;
    uint32_t totalSectors32;
    uint32_t fatSize32;
    uint16_t extFlags;
    uint16_t fsVersion;
    uint32_t rootCluster;
    uint16_t fsInfo;
    uint16_t backupBootSector;
    uint8_t reserved[12];
    uint8_t driveNumber;
    uint8_t reserved1;
    uint8_t bootSignature;
    uint32_t volumeId;
    uint8_t volumeLabel[11];
    uint8_t fsType[8];
};

struct FAT32DirectoryEntry {
    uint8_t name[11];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t creationTimeTenth;
    uint16_t creationTime;
    uint16_t creationDate;
    uint16_t lastAccessDate;
    uint16_t firstClusterHigh;
    uint16_t writeTime;
    uint16_t writeDate;
    uint16_t firstClusterLow;
    uint32_t fileSize;
};

#pragma pack(pop)

} // namespace FileRecovery

#endif // FAT32_PARSER_H
