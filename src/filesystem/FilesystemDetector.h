#ifndef FILESYSTEM_DETECTOR_H
#define FILESYSTEM_DETECTOR_H

#include "../include/RecoveryTypes.h"

namespace FileRecovery {

class DiskIO;

class FilesystemDetector {
public:
    FilesystemType detectFilesystem(DiskIO& disk);

private:
    bool isNTFS(const ByteArray& bootSector);
    bool isFAT32(const ByteArray& bootSector);
    bool isexFAT(const ByteArray& bootSector);
    bool isEXT(const ByteArray& bootSector);
};

} // namespace FileRecovery

#endif // FILESYSTEM_DETECTOR_H
