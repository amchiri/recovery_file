#ifndef FILE_RECONSTRUCTOR_H
#define FILE_RECONSTRUCTOR_H

#include "../include/RecoveryTypes.h"

namespace FileRecovery {

class DiskIO;

/**
 * Reconstructeur de fichiers fragmentés
 * Assemble les fragments de fichiers dispersés sur le disque
 */
class FileReconstructor {
public:
    FileReconstructor();
    ~FileReconstructor();

    // Reconstruction de fichiers fragmentés
    bool reconstructFile(const RecoveredFile& file, DiskIO& disk, ByteArray& output);
    bool detectFragmentation(const RecoveredFile& file, DiskIO& disk);

private:
    bool reconstructFragmentedFile(const RecoveredFile& file, DiskIO& disk, ByteArray& output);
    bool reconstructContiguousFile(const RecoveredFile& file, DiskIO& disk, ByteArray& output);
};

} // namespace FileRecovery

#endif // FILE_RECONSTRUCTOR_H
