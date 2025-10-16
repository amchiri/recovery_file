#include "RecoveryManager.h"
#include "../utils/DiskIO.h"
#include "../utils/Logger.h"
#include "../include/FileSignatures.h"
#include "../include/FileQuality.h"
#include <fstream>
#include <filesystem>
#include <algorithm>

namespace FileRecovery {

RecoveryManager::RecoveryManager() {}

RecoveryManager::~RecoveryManager() {}

bool RecoveryManager::recoverFile(const RecoveredFile& file, 
                                 const std::string& outputPath, DiskIO& disk) {
    Logger::getInstance().log(LogLevel::INFO, 
        "Recovering file: " + file.originalName + " to " + outputPath);
    
    // Créer le répertoire de sortie si nécessaire
    std::filesystem::path outPath(outputPath);
    if (!std::filesystem::exists(outPath.parent_path())) {
        if (!createOutputDirectory(outPath.parent_path().string())) {
            return false;
        }
    }
    
    // Vérifier si le fichier existe déjà
    if (std::filesystem::exists(outputPath) && !m_overwriteExisting) {
        Logger::getInstance().log(LogLevel::WARNING, 
            "File already exists, skipping: " + outputPath);
        return false;
    }
    
    // Lire les données du disque
    Size fileSize = file.fileSize;
    if (fileSize == 0) {
        // Essayer de déterminer la taille à partir de la signature
        fileSize = 1024 * 1024; // 1MB par défaut
    }
    
    ByteArray buffer(fileSize);
    if (!disk.readBytes(file.diskOffset, fileSize, buffer.data())) {
        Logger::getInstance().log(LogLevel::ERROR, 
            "Failed to read file data from disk");
        return false;
    }
    
    // Pour les fichiers .sav (GVAS), essayer de trouver la vraie fin du fichier
    Size actualSize = fileSize;
    if (file.detectedType == "sav" && fileSize > 1024) {
        // Chercher le pattern de fin GVAS: FString "None" 
        // Pattern: 05 00 00 00 4E 6F 6E 65 00
        // Ce tag marque la fin des Properties dans un fichier GVAS
        
        bool foundEnd = false;
        size_t endOffset = fileSize;
        
        // Chercher le dernier "None" FString dans le fichier
        // On scanne depuis le début car il peut y avoir plusieurs "None"
        size_t lastNoneOffset = 0;
        
        for (size_t i = 1024; i < fileSize - 9; ++i) {
            // Chercher: 05 00 00 00 4E 6F 6E 65 00
            if (buffer[i] == 0x05 && buffer[i+1] == 0x00 && 
                buffer[i+2] == 0x00 && buffer[i+3] == 0x00 &&
                buffer[i+4] == 0x4E && buffer[i+5] == 0x6F && 
                buffer[i+6] == 0x6E && buffer[i+7] == 0x65 && 
                buffer[i+8] == 0x00) {
                // Trouvé un tag "None"
                lastNoneOffset = i;
                // Continue à chercher le dernier
            }
        }
        
        if (lastNoneOffset > 0) {
            // Le fichier se termine juste après ce tag (9 bytes) + 4 bytes de trailing
            // Les sauvegardes Octopath Traveler 2 ont exactement 13 bytes après le début du pattern
            endOffset = lastNoneOffset + 13;
            foundEnd = true;
            Logger::getInstance().log(LogLevel::INFO, 
                "Found GVAS end marker 'None' at offset " + std::to_string(lastNoneOffset) +
                ", file should end at " + std::to_string(endOffset));
        }
        
        // Fallback: chercher 1MB de zéros ou une autre signature GVAS
        if (!foundEnd) {
            Logger::getInstance().log(LogLevel::WARNING, 
                "Could not find 'None' end marker, trying fallback methods");
            
            // Chercher une autre signature GVAS
            for (size_t i = 1024; i < fileSize - 4; ++i) {
                if (buffer[i] == 0x47 && buffer[i+1] == 0x56 && 
                    buffer[i+2] == 0x41 && buffer[i+3] == 0x53) {
                    endOffset = i;
                    foundEnd = true;
                    Logger::getInstance().log(LogLevel::INFO, 
                        "Found next GVAS signature at offset " + std::to_string(i));
                    break;
                }
            }
        }
        
        if (foundEnd && endOffset < fileSize) {
            // Pour GVAS, pas d'arrondi - la taille est exacte après le pattern
            actualSize = endOffset;
            
            size_t savedBytes = fileSize - actualSize;
            Logger::getInstance().log(LogLevel::INFO, 
                "Detected actual file size: " + std::to_string(actualSize) + 
                " bytes (saved " + std::to_string(savedBytes / 1024 / 1024) + " MB)");
        } else {
            Logger::getInstance().log(LogLevel::WARNING, 
                "Could not detect file end reliably, keeping full size");
        }
    }
    
    // Écrire dans le fichier de sortie
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile) {
        Logger::getInstance().log(LogLevel::ERROR, 
            "Failed to create output file: " + outputPath);
        return false;
    }
    
    outFile.write(reinterpret_cast<const char*>(buffer.data()), actualSize);
    outFile.close();
    
    // Analyser la qualité du fichier récupéré
    FileQuality qualityAnalyzer;
    ByteArray recoveredData(buffer.begin(), buffer.begin() + actualSize);
    FileQualityReport qualityReport = qualityAnalyzer.analyzeBuffer(recoveredData, file.detectedType);
    
    // Logger la qualité
    std::string qualityLevel = FileQuality::getQualityDescription(qualityReport.level);
    Logger::getInstance().log(LogLevel::INFO, 
        "File quality: " + qualityLevel + 
        " (zeros: " + std::to_string(static_cast<int>(qualityReport.zeroPercentage)) + "%)");
    
    // Avertir si le fichier est de mauvaise qualité
    if (qualityReport.level == FileQualityLevel::DEAD) {
        Logger::getInstance().log(LogLevel::WARNING, 
            "⚠️  File is DEAD (99%+ zeros) - likely overwritten and unusable");
        Logger::getInstance().log(LogLevel::WARNING, 
            "   Recommendation: " + qualityReport.recommendation);
    } else if (qualityReport.level == FileQualityLevel::CORRUPTED) {
        Logger::getInstance().log(LogLevel::WARNING, 
            "⚠️  File is CORRUPTED (90%+ zeros) - may be partially damaged");
        Logger::getInstance().log(LogLevel::WARNING, 
            "   Recommendation: " + qualityReport.recommendation);
    } else if (qualityReport.level == FileQualityLevel::EXCELLENT) {
        Logger::getInstance().log(LogLevel::INFO, 
            "✓ File quality is EXCELLENT - should be fully usable");
    }
    
    // Vérifier l'intégrité si demandé
    if (m_verifyIntegrity) {
        if (!verifyFileIntegrity(file, outputPath)) {
            Logger::getInstance().log(LogLevel::WARNING, 
                "File integrity check failed: " + outputPath);
        }
    }
    
    Logger::getInstance().log(LogLevel::INFO, 
        "File recovered successfully: " + outputPath + 
        " (" + std::to_string(actualSize) + " bytes)");
    
    return true;
}

bool RecoveryManager::recoverMultipleFiles(const std::vector<RecoveredFile>& files,
                                          const std::string& outputDir, DiskIO& disk) {
    Logger::getInstance().log(LogLevel::INFO, 
        "Recovering " + std::to_string(files.size()) + " files to " + outputDir);
    
    // Créer le répertoire de sortie
    if (!createOutputDirectory(outputDir)) {
        return false;
    }
    
    int successCount = 0;
    int failCount = 0;
    
    for (const auto& file : files) {
        std::string outputPath = outputDir + "/" + file.originalName;
        
        // Générer un nom unique si le fichier existe déjà
        if (std::filesystem::exists(outputPath) && !m_overwriteExisting) {
            outputPath = generateUniqueFilename(outputDir, file.originalName);
        }
        
        if (recoverFile(file, outputPath, disk)) {
            successCount++;
        } else {
            failCount++;
        }
    }
    
    Logger::getInstance().log(LogLevel::INFO, 
        "Recovery complete - Success: " + std::to_string(successCount) + 
        ", Failed: " + std::to_string(failCount));
    
    return failCount == 0;
}

void RecoveryManager::setVerifyIntegrity(bool verify) {
    m_verifyIntegrity = verify;
}

void RecoveryManager::setOverwriteExisting(bool overwrite) {
    m_overwriteExisting = overwrite;
}

bool RecoveryManager::createOutputDirectory(const std::string& path) {
    try {
        std::filesystem::create_directories(path);
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().log(LogLevel::ERROR, 
            "Failed to create directory: " + path + " - " + e.what());
        return false;
    }
}

std::string RecoveryManager::generateUniqueFilename(const std::string& basePath,
                                                   const std::string& filename) {
    std::filesystem::path filePath(filename);
    std::string stem = filePath.stem().string();
    std::string ext = filePath.extension().string();
    
    int counter = 1;
    std::string newFilename;
    
    do {
        newFilename = basePath + "/" + stem + "_" + std::to_string(counter) + ext;
        counter++;
    } while (std::filesystem::exists(newFilename));
    
    return newFilename;
}

bool RecoveryManager::verifyFileIntegrity(const RecoveredFile& file, 
                                         const std::string& recoveredPath) {
    // Lire le fichier récupéré
    std::ifstream inFile(recoveredPath, std::ios::binary);
    if (!inFile) {
        return false;
    }
    
    // Lire les premiers octets pour vérifier la signature
    ByteArray header(512);
    inFile.read(reinterpret_cast<char*>(header.data()), header.size());
    inFile.close();
    
    // Vérifier la signature
    auto& sigDb = FileSignatures::getInstance();
    return sigDb.hasValidHeader(header, file.detectedType);
}

} // namespace FileRecovery
