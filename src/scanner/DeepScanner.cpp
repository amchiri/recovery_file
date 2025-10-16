#include "DeepScanner.h"
#include "../utils/DiskIO.h"
#include "../utils/Logger.h"
#include "../include/FileSignatures.h"
#include <algorithm>
#include <chrono>

namespace FileRecovery {

DeepScanner::DeepScanner() {}

bool DeepScanner::startScan(const DiskInfo& disk, const ScanConfig& config) {
    if (m_scanning) {
        return false;
    }
    
    m_scanning = true;
    m_paused = false;
    m_progress = 0.0f;
    m_sectorsScanned = 0;
    m_totalSectors = disk.totalSize / disk.sectorSize;
    
    // Debug: afficher les infos
    Logger::getInstance().log(LogLevel::INFO, 
        "DeepScanner starting: totalSize=" + std::to_string(disk.totalSize) + 
        " sectorSize=" + std::to_string(disk.sectorSize) + 
        " totalSectors=" + std::to_string(m_totalSectors) +
        " threads=" + std::to_string(config.threadCount));
    
    // Créer un thread principal qui coordonne le scan
    m_threads.push_back(std::make_unique<std::thread>(
        &DeepScanner::scanThread, this, disk, config));
    
    return true;
}

bool DeepScanner::stopScan() {
    m_scanning = false;
    for (auto& thread : m_threads) {
        if (thread && thread->joinable()) {
            thread->join();
        }
    }
    m_threads.clear();
    return true;
}

bool DeepScanner::pauseScan() {
    m_paused = true;
    return true;
}

bool DeepScanner::resumeScan() {
    m_paused = false;
    return true;
}

std::vector<RecoveredFile> DeepScanner::getResults() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_results;
}

size_t DeepScanner::getFileCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_results.size();
}

float DeepScanner::getProgress() const {
    if (m_totalSectors == 0) return 0.0f;
    return static_cast<float>(m_sectorsScanned.load()) / m_totalSectors;
}

bool DeepScanner::isScanning() const {
    return m_scanning;
}

void DeepScanner::setProgressCallback(std::function<void(float, const std::string&)> callback) {
    m_progressCallback = callback;
}

void DeepScanner::setFileFoundCallback(std::function<void(const RecoveredFile&)> callback) {
    m_fileFoundCallback = callback;
}

void DeepScanner::scanThread(const DiskInfo& disk, const ScanConfig& config) {
    Logger::getInstance().log(LogLevel::INFO, "Deep scan started");
    
    // Diviser le disque en chunks pour le multi-threading
    int threadCount = config.threadCount;
    uint64_t sectorsPerThread = m_totalSectors / threadCount;
    
    std::vector<std::thread> workers;
    
    for (int i = 0; i < threadCount; ++i) {
        uint64_t startSector = i * sectorsPerThread;
        uint64_t endSector = (i == threadCount - 1) ? m_totalSectors : (i + 1) * sectorsPerThread;
        
        workers.emplace_back(&DeepScanner::scanSectorRange, this, 
                            startSector, endSector, disk, config);
    }
    
    // Attendre tous les workers
    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    
    m_scanning = false;
    m_progress = 1.0f;
    
    if (m_progressCallback) {
        m_progressCallback(1.0f, "Deep scan complete");
    }
    
    Logger::getInstance().log(LogLevel::INFO, 
        "Deep scan completed. Found " + std::to_string(m_results.size()) + " files");
}

void DeepScanner::scanSectorRange(uint64_t startSector, uint64_t endSector,
                                 const DiskInfo& disk, const ScanConfig& config) {
    DiskIO diskIO;
    if (!diskIO.openDevice(disk.devicePath)) {
        Logger::getInstance().log(LogLevel::ERROR, "Failed to open device for scanning");
        return;
    }
    
    Logger::getInstance().log(LogLevel::INFO, 
        "Worker thread scanning sectors " + std::to_string(startSector) + 
        " to " + std::to_string(endSector));
    
    // Phase 8: Adjust buffer size based on settings
    const size_t bufferSectors = m_largeBuffersEnabled ? 2048 : 128; // 1MB or 64KB
    ByteArray buffer(bufferSectors * disk.sectorSize);
    
    auto& sigDb = FileSignatures::getInstance();
    
    uint64_t sectorsRead = 0;
    uint64_t readFailures = 0;
    
    for (uint64_t sector = startSector; sector < endSector && m_scanning; sector += bufferSectors) {
        // Pause si demandé
        while (m_paused && m_scanning) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        if (!m_scanning) break;
        
        uint64_t sectorsToRead = std::min(bufferSectors, endSector - sector);
        
        if (diskIO.readSectors(sector, sectorsToRead, buffer.data())) {
            sectorsRead++;
            
            // Phase 8: Smart Scan - Skip empty sectors
            if (m_smartScanEnabled && isEmptySector(buffer, 0, buffer.size())) {
                m_sectorsSkipped += sectorsToRead;
                m_sectorsScanned += sectorsToRead;
                continue; // Skip this empty block
            }
            
            // Chercher des signatures de fichiers
            std::vector<std::string> extensionsToScan = config.targetExtensions;
            
            // Si aucune extension spécifiée, scanner toutes les extensions supportées
            if (extensionsToScan.empty()) {
                extensionsToScan = sigDb.getSupportedExtensions();
            }
            
            for (const auto& ext : extensionsToScan) {
                auto offsets = sigDb.findSignatures(buffer, ext);
                
                for (auto offset : offsets) {
                    RecoveredFile file;
                    file.diskOffset = sector * disk.sectorSize + offset;
                    file.detectedType = ext;
                    file.status = RecoveryStatus::FOUND;
                    file.originalName = "recovered_" + std::to_string(file.diskOffset) + "." + ext;
                    
                    // Essayer de déterminer la taille du fichier
                    file.fileSize = 0;
                    
                    // Pour les fichiers GVAS (Unreal Engine saves)
                    if (ext == "sav" && offset + 12 < buffer.size()) {
                        // Structure GVAS: 
                        // 0x00: Magic "GVAS" (4 bytes)
                        // 0x04: Version (4 bytes, little-endian)
                        // 0x08: Package File Version (4 bytes)
                        // 0x0C: Engine Version (variable)
                        // La taille totale n'est pas dans l'en-tête, on doit lire jusqu'à la fin
                        // Pour l'instant, on utilise une estimation basée sur la structure
                        
                        // Lire les 4 premiers octets après la signature pour vérifier
                        if (buffer[offset] == 0x47 && buffer[offset+1] == 0x56 && 
                            buffer[offset+2] == 0x41 && buffer[offset+3] == 0x53) {
                            
                            // Vérifier si c'est un vrai fichier de sauvegarde Octopath Traveler 2
                            // en cherchant le SaveGameClassName dans les premiers 2KB
                            bool isOctopathSave = false;
                            size_t searchEnd = std::min(offset + 2048, buffer.size() - 50);
                            
                            for (size_t i = offset; i < searchEnd; ++i) {
                                // Chercher la string "KSSaveGameBP" (partie du SaveGameClassName d'Octopath)
                                if (i + 12 < buffer.size() &&
                                    buffer[i] == 'K' && buffer[i+1] == 'S' &&
                                    buffer[i+2] == 'S' && buffer[i+3] == 'a' &&
                                    buffer[i+4] == 'v' && buffer[i+5] == 'e' &&
                                    buffer[i+6] == 'G' && buffer[i+7] == 'a' &&
                                    buffer[i+8] == 'm' && buffer[i+9] == 'e' &&
                                    buffer[i+10] == 'B' && buffer[i+11] == 'P') {
                                    isOctopathSave = true;
                                    break;
                                }
                            }
                            
                            // Si ce n'est pas un fichier de sauvegarde Octopath, ignorer
                            if (!isOctopathSave) {
                                continue; // Passer au prochain offset
                            }
                            
                            // Lire une grande taille pour capturer même les gros fichiers de sauvegarde
                            // La vraie taille sera détectée dans RecoveryManager en cherchant la fin
                            file.fileSize = 1000 * 1024 * 1024; // 1000MB (1GB) max
                            
                            Logger::getInstance().log(LogLevel::INFO, 
                                "Found valid Octopath Traveler 2 save at offset " + std::to_string(file.diskOffset));
                        }
                    }
                    
                    // Si aucune taille détectée, utiliser une valeur par défaut
                    if (file.fileSize == 0) {
                        file.fileSize = 1024 * 1024; // 1MB par défaut
                    }
                    
                    // Ajouter aux résultats
                    {
                        std::lock_guard<std::mutex> lock(m_mutex);
                        m_results.push_back(file);
                    }
                    
                    if (m_fileFoundCallback) {
                        m_fileFoundCallback(file);
                    }
                }
            }
        } else {
            readFailures++;
            if (readFailures < 10) {  // Log seulement les 10 premières erreurs
                Logger::getInstance().log(LogLevel::WARNING, 
                    "Failed to read sector " + std::to_string(sector));
            }
        }
        
        m_sectorsScanned += sectorsToRead;
        
        // Mettre à jour la progression périodiquement
        if (m_sectorsScanned % 10000 == 0 && m_progressCallback) {
            float progress = getProgress();
            m_progressCallback(progress, 
                "Scanning sector " + std::to_string(m_sectorsScanned.load()) + 
                " / " + std::to_string(m_totalSectors));
        }
    }
    
    Logger::getInstance().log(LogLevel::INFO, 
        "Worker finished: read=" + std::to_string(sectorsRead) + 
        " failures=" + std::to_string(readFailures));
}

bool DeepScanner::isEmptySector(const ByteArray& buffer, size_t offset, size_t length) const {
    if (offset + length > buffer.size()) return false;
    
    // Sample every 64 bytes for performance (instead of checking every byte)
    uint8_t firstByte = buffer[offset];
    for (size_t i = offset + 64; i < offset + length; i += 64) {
        if (buffer[i] != firstByte) return false;
    }
    
    // If all samples matched, it's likely empty (all zeros or all same byte)
    return (firstByte == 0x00 || firstByte == 0xFF);
}

} // namespace FileRecovery
