#include "FileReconstructor.h"
#include "../utils/DiskIO.h"
#include "../utils/Logger.h"

namespace FileRecovery {

FileReconstructor::FileReconstructor() {}

FileReconstructor::~FileReconstructor() {}

bool FileReconstructor::reconstructFile(const RecoveredFile& file, 
                                       DiskIO& disk, ByteArray& output) {
    if (file.isFragmented) {
        return reconstructFragmentedFile(file, disk, output);
    } else {
        return reconstructContiguousFile(file, disk, output);
    }
}

bool FileReconstructor::detectFragmentation(const RecoveredFile& file, DiskIO& disk) {
    // Analyser si le fichier est fragmenté
    // Une vraie implémentation analyserait les patterns de données
    return false;
}

bool FileReconstructor::reconstructFragmentedFile(const RecoveredFile& file,
                                                 DiskIO& disk, ByteArray& output) {
    Logger::getInstance().log(LogLevel::INFO, 
        "Reconstructing fragmented file: " + file.originalName);
    
    output.clear();
    
    for (const auto& fragmentOffset : file.fragmentOffsets) {
        // Lire chaque fragment
        // Implémentation simplifiée
        ByteArray fragment(4096); // Taille de bloc arbitraire
        
        if (disk.readBytes(fragmentOffset, fragment.size(), fragment.data())) {
            output.insert(output.end(), fragment.begin(), fragment.end());
        } else {
            Logger::getInstance().log(LogLevel::WARNING, 
                "Failed to read fragment at offset: " + std::to_string(fragmentOffset));
        }
    }
    
    return !output.empty();
}

bool FileReconstructor::reconstructContiguousFile(const RecoveredFile& file,
                                                 DiskIO& disk, ByteArray& output) {
    output.resize(file.fileSize);
    return disk.readBytes(file.diskOffset, file.fileSize, output.data());
}

} // namespace FileRecovery
