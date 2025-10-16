#include "DiskIO.h"

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winioctl.h>
#include <setupapi.h>
#include <vector>
#include <algorithm>
#include <cstring>
#pragma comment(lib, "setupapi.lib")
// Undef les macros Windows qui interfèrent
#ifdef ERROR
#undef ERROR
#endif
#else
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#endif

#include "Logger.h"

namespace FileRecovery {

class DiskIO::Impl {
public:
#ifdef _WIN32
    HANDLE hDevice = INVALID_HANDLE_VALUE;
#else
    int fd = -1;
#endif
    
    std::string devicePath;
    DiskInfo diskInfo;
    bool opened = false;
};

DiskIO::DiskIO() : pImpl(std::make_unique<Impl>()) {}

DiskIO::~DiskIO() {
    closeDevice();
}

bool DiskIO::openDevice(const std::string& devicePath) {
    if (pImpl->opened) {
        closeDevice();
    }
    
    // Normaliser le chemin du périphérique
    std::string normalizedPath = devicePath;
    
#ifdef _WIN32
    // Si c'est juste une lettre de lecteur (ex: "C:" ou "C"), convertir en \\.\C:
    if (devicePath.length() <= 2) {
        char driveLetter = std::toupper(devicePath[0]);
        if (driveLetter >= 'A' && driveLetter <= 'Z') {
            normalizedPath = std::string("\\\\.\\") + driveLetter + ":";
            Logger::getInstance().log(LogLevel::INFO, 
                "Converting drive letter to device path: " + devicePath + " -> " + normalizedPath);
        }
    }
    // Si c'est "C:" ou "D:" sans le préfixe \\.\, l'ajouter
    else if (devicePath.length() == 2 && devicePath[1] == ':') {
        normalizedPath = std::string("\\\\.\\") + devicePath;
        Logger::getInstance().log(LogLevel::INFO, 
            "Adding device prefix: " + devicePath + " -> " + normalizedPath);
    }
#endif
    
    pImpl->devicePath = normalizedPath;
    
#ifdef _WIN32
    // Windows - ouvrir le périphérique
    std::wstring wDevicePath(normalizedPath.begin(), normalizedPath.end());
    
    pImpl->hDevice = CreateFileW(
        wDevicePath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_NO_BUFFERING | FILE_FLAG_RANDOM_ACCESS,
        NULL
    );
    
    if (pImpl->hDevice == INVALID_HANDLE_VALUE) {
        const DWORD errorCode = GetLastError();
        std::string errorMsg = "Failed to open device: " + devicePath + 
                               " - Win32 Error: " + std::to_string(errorCode);
        Logger::getInstance().log(LogLevel::ERROR, errorMsg);
        return false;
    }
    
    // Obtenir les informations du disque
    DISK_GEOMETRY_EX geometry;
    DWORD bytesReturned;
    
    if (DeviceIoControl(pImpl->hDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
                       NULL, 0, &geometry, sizeof(geometry), &bytesReturned, NULL)) {
        pImpl->diskInfo.devicePath = devicePath;
        pImpl->diskInfo.totalSize = geometry.DiskSize.QuadPart;
        pImpl->diskInfo.sectorSize = geometry.Geometry.BytesPerSector;
        pImpl->diskInfo.isHealthy = true;
    } else {
        // Valeurs par défaut
        pImpl->diskInfo.sectorSize = 512;
    }
    
#else
    // Linux - ouvrir le périphérique
    pImpl->fd = open(devicePath.c_str(), O_RDONLY | O_DIRECT);
    
    if (pImpl->fd < 0) {
        Logger::getInstance().log(LogLevel::ERROR, "Failed to open device: " + devicePath);
        return false;
    }
    
    // Obtenir la taille
    uint64_t size;
    if (ioctl(pImpl->fd, BLKGETSIZE64, &size) == 0) {
        pImpl->diskInfo.totalSize = size;
    }
    
    pImpl->diskInfo.devicePath = devicePath;
    pImpl->diskInfo.sectorSize = 512;
    pImpl->diskInfo.isHealthy = true;
#endif
    
    pImpl->opened = true;
    Logger::getInstance().log(LogLevel::INFO, "Device opened: " + devicePath);
    
    return true;
}

bool DiskIO::closeDevice() {
    if (!pImpl->opened) {
        return true;
    }
    
#ifdef _WIN32
    if (pImpl->hDevice != INVALID_HANDLE_VALUE) {
        CloseHandle(pImpl->hDevice);
        pImpl->hDevice = INVALID_HANDLE_VALUE;
    }
#else
    if (pImpl->fd >= 0) {
        close(pImpl->fd);
        pImpl->fd = -1;
    }
#endif
    
    pImpl->opened = false;
    Logger::getInstance().log(LogLevel::INFO, "Device closed");
    
    return true;
}

bool DiskIO::isOpen() const {
    return pImpl->opened;
}

bool DiskIO::readBytes(Offset offset, Size size, Byte* buffer) {
    if (!pImpl->opened) {
        Logger::getInstance().log(LogLevel::ERROR, "Device not opened");
        return false;
    }
    
#ifdef _WIN32
    // Avec FILE_FLAG_NO_BUFFERING, on doit lire des blocs alignés
    // Calculer l'alignement
    const DWORD sectorSize = pImpl->diskInfo.sectorSize;
    const Offset alignedOffset = (offset / sectorSize) * sectorSize;
    const Size offsetInSector = offset - alignedOffset;
    const Size alignedSize = ((offsetInSector + size + sectorSize - 1) / sectorSize) * sectorSize;
    
    // Allouer un buffer aligné temporaire
    std::vector<Byte> alignedBuffer(alignedSize);
    
    LARGE_INTEGER liOffset;
    liOffset.QuadPart = alignedOffset;
    
    if (SetFilePointerEx(pImpl->hDevice, liOffset, NULL, FILE_BEGIN) == 0) {
        DWORD errorCode = GetLastError();
        Logger::getInstance().log(LogLevel::ERROR, 
            "SetFilePointerEx failed at offset " + std::to_string(alignedOffset) + 
            " - Error: " + std::to_string(errorCode));
        return false;
    }
    
    DWORD bytesRead;
    if (ReadFile(pImpl->hDevice, alignedBuffer.data(), static_cast<DWORD>(alignedSize), &bytesRead, NULL) == 0) {
        DWORD errorCode = GetLastError();
        Logger::getInstance().log(LogLevel::ERROR, 
            "ReadFile failed - Error: " + std::to_string(errorCode));
        return false;
    }
    
    if (bytesRead < offsetInSector + size) {
        Logger::getInstance().log(LogLevel::ERROR, 
            "Not enough bytes read: " + std::to_string(bytesRead) + 
            " < " + std::to_string(offsetInSector + size));
        return false;
    }
    
    // Copier uniquement les bytes demandés
    std::memcpy(buffer, alignedBuffer.data() + offsetInSector, size);
    
    return true;
#else
    if (lseek(pImpl->fd, offset, SEEK_SET) < 0) {
        return false;
    }
    
    ssize_t bytesRead = read(pImpl->fd, buffer, size);
    return bytesRead == static_cast<ssize_t>(size);
#endif
}

bool DiskIO::readSectors(uint64_t startSector, uint64_t count, Byte* buffer) {
    uint64_t offset = startSector * pImpl->diskInfo.sectorSize;
    uint64_t size = count * pImpl->diskInfo.sectorSize;
    
    return readBytes(offset, size, buffer);
}

bool DiskIO::writeBytes(Offset offset, Size size, const Byte* buffer) {
    // Implémentation pour l'écriture (si nécessaire)
    return false;
}

DiskInfo DiskIO::getDiskInfo() const {
    return pImpl->diskInfo;
}

std::vector<DiskInfo> DiskIO::enumerateDisks() const {
    std::vector<DiskInfo> disks;
    
#ifdef _WIN32
    // Énumérer les lecteurs logiques
    DWORD drives = GetLogicalDrives();
    
    for (int i = 0; i < 26; ++i) {
        if (drives & (1 << i)) {
            char driveLetter = 'A' + i;
            std::string drivePath = std::string(1, driveLetter) + ":";
            
            DiskInfo info;
            info.devicePath = drivePath;
            info.volumeName = drivePath;
            
            // Obtenir le type de lecteur
            std::string rootPath = drivePath + "\\";
            UINT driveType = GetDriveTypeA(rootPath.c_str());
            
            if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
                ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
                
                if (GetDiskFreeSpaceExA(rootPath.c_str(), &freeBytesAvailable, 
                                       &totalBytes, &totalFreeBytes)) {
                    info.totalSize = totalBytes.QuadPart;
                    info.freeSize = totalFreeBytes.QuadPart;
                    info.usedSize = info.totalSize - info.freeSize;
                }
                
                info.sectorSize = 512;
                info.isHealthy = true;
                
                disks.push_back(info);
            }
        }
    }
    
    // Également énumérer les disques physiques
    for (int i = 0; i < 10; ++i) {
        std::string physicalPath = "\\\\.\\PhysicalDrive" + std::to_string(i);
        
        HANDLE hDisk = CreateFileA(
            physicalPath.c_str(),
            0,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            0,
            NULL
        );
        
        if (hDisk != INVALID_HANDLE_VALUE) {
            DISK_GEOMETRY_EX geometry;
            DWORD bytesReturned;
            
            if (DeviceIoControl(hDisk, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
                               NULL, 0, &geometry, sizeof(geometry), &bytesReturned, NULL)) {
                DiskInfo info;
                info.devicePath = physicalPath;
                info.volumeName = "Physical Drive " + std::to_string(i);
                info.totalSize = geometry.DiskSize.QuadPart;
                info.sectorSize = geometry.Geometry.BytesPerSector;
                info.isHealthy = true;
                
                disks.push_back(info);
            }
            
            CloseHandle(hDisk);
        }
    }
#else
    // Linux - lire /proc/partitions ou utiliser libudev
    // Implémentation simplifiée
#endif
    
    return disks;
}

uint64_t DiskIO::getDeviceSize() const {
    return pImpl->diskInfo.totalSize;
}

uint32_t DiskIO::getSectorSize() const {
    return pImpl->diskInfo.sectorSize;
}

} // namespace FileRecovery
