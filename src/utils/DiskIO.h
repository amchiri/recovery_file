#ifndef DISK_IO_H
#define DISK_IO_H

#include "../include/RecoveryTypes.h"
#include <memory>
#include <string>

namespace FileRecovery {

/**
 * Classe pour l'accès bas niveau au disque
 * Gère les opérations de lecture secteur par secteur
 */
class DiskIO {
public:
    DiskIO();
    ~DiskIO();

    // Ouverture/fermeture de périphérique
    bool openDevice(const std::string& devicePath);
    bool closeDevice();
    bool isOpen() const;

    // Lecture
    bool readBytes(Offset offset, Size size, Byte* buffer);
    bool readSectors(uint64_t startSector, uint64_t count, Byte* buffer);
    
    // Écriture (pour la récupération)
    bool writeBytes(Offset offset, Size size, const Byte* buffer);

    // Informations
    DiskInfo getDiskInfo() const;
    std::vector<DiskInfo> enumerateDisks() const;
    
    // Utilitaires
    uint64_t getDeviceSize() const;
    uint32_t getSectorSize() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;

    // Non-copyable
    DiskIO(const DiskIO&) = delete;
    DiskIO& operator=(const DiskIO&) = delete;
};

} // namespace FileRecovery

#endif // DISK_IO_H
