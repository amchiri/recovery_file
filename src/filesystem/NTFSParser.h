#ifndef NTFS_PARSER_H
#define NTFS_PARSER_H

#include "../include/RecoveryTypes.h"
#include <vector>
#include <memory>

namespace FileRecovery {

class DiskIO;

/**
 * Parser pour système de fichiers NTFS
 * Permet de lire la MFT (Master File Table) et récupérer les fichiers supprimés
 */
class NTFSParser {
public:
    NTFSParser();
    ~NTFSParser();

    // Initialisation
    bool initialize(DiskIO& disk);
    bool isValid() const;

    // Lecture de la MFT
    bool parseMFT();
    std::vector<RecoveredFile> getDeletedFiles() const;

    // Informations sur le volume
    uint64_t getMFTOffset() const;
    uint32_t getClusterSize() const;
    uint32_t getBytesPerSector() const;
    uint64_t getTotalClusters() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// Structures NTFS
#pragma pack(push, 1)

struct NTFSBootSector {
    uint8_t jumpBoot[3];
    uint8_t oemId[8];
    uint16_t bytesPerSector;
    uint8_t sectorsPerCluster;
    uint16_t reservedSectors;
    uint8_t zeros1[3];
    uint16_t unused1;
    uint8_t mediaDescriptor;
    uint16_t zeros2;
    uint16_t sectorsPerTrack;
    uint16_t numberOfHeads;
    uint32_t hiddenSectors;
    uint32_t unused2;
    uint32_t unused3;
    uint64_t totalSectors;
    uint64_t mftClusterNumber;
    uint64_t mftMirrorClusterNumber;
    int8_t clustersPerMFTRecord;
    uint8_t unused4[3];
    int8_t clustersPerIndexBuffer;
    uint8_t unused5[3];
    uint64_t volumeSerialNumber;
    uint32_t checksum;
    uint8_t bootCode[426];
    uint16_t endOfSectorMarker;
};

struct MFTRecordHeader {
    uint32_t signature;        // FILE
    uint16_t updateSequenceOffset;
    uint16_t updateSequenceSize;
    uint64_t logFileSequenceNumber;
    uint16_t sequenceNumber;
    uint16_t hardLinkCount;
    uint16_t firstAttributeOffset;
    uint16_t flags;            // 0x01 = in use, 0x02 = directory
    uint32_t usedSize;
    uint32_t allocatedSize;
    uint64_t fileReferenceToBaseRecord;
    uint16_t nextAttributeId;
};

struct AttributeHeader {
    uint32_t attributeType;
    uint32_t length;
    uint8_t nonResident;
    uint8_t nameLength;
    uint16_t nameOffset;
    uint16_t flags;
    uint16_t attributeId;
};

#pragma pack(pop)

} // namespace FileRecovery

#endif // NTFS_PARSER_H
