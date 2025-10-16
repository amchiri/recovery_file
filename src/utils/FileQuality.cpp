#include "../include/FileQuality.h"
#include "../utils/Logger.h"
#include <fstream>
#include <algorithm>

namespace FileRecovery {

FileQuality::FileQuality() {}

FileQuality::~FileQuality() {}

FileQualityReport FileQuality::analyzeFile(const std::string& filePath) {
    FileQualityReport report;
    report.level = FileQualityLevel::CORRUPTED;
    report.zeroPercentage = 100.0f;
    report.totalBytes = 0;
    report.zeroBytes = 0;
    report.nonZeroBytes = 0;
    report.hasValidHeader = false;
    report.hasValidFooter = false;
    
    // Lire le fichier
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        report.recommendation = "Unable to open file for analysis";
        return report;
    }
    
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    ByteArray data(fileSize);
    file.read(reinterpret_cast<char*>(data.data()), fileSize);
    file.close();
    
    report.totalBytes = fileSize;
    
    // Extraire l'extension du fichier
    std::string fileType = "unknown";
    size_t dotPos = filePath.find_last_of('.');
    if (dotPos != std::string::npos) {
        fileType = filePath.substr(dotPos + 1);
        std::transform(fileType.begin(), fileType.end(), fileType.begin(), ::tolower);
    }
    
    return analyzeBuffer(data, fileType);
}

FileQualityReport FileQuality::analyzeBuffer(const ByteArray& data, const std::string& fileType) {
    FileQualityReport report;
    report.totalBytes = data.size();
    
    // Calculer le pourcentage de zéros
    report.zeroPercentage = calculateZeroPercentage(data);
    report.zeroBytes = static_cast<size_t>(data.size() * report.zeroPercentage / 100.0f);
    report.nonZeroBytes = data.size() - report.zeroBytes;
    
    // Vérifier l'en-tête et le footer
    report.hasValidHeader = checkHeader(data, fileType);
    report.hasValidFooter = checkFooter(data, fileType);
    
    // Déterminer le niveau de qualité
    report.level = determineQualityLevel(report.zeroPercentage, report.hasValidHeader, report.hasValidFooter);
    
    // Générer une recommandation
    report.recommendation = generateRecommendation(report);
    
    return report;
}

bool FileQuality::isWorthRecovering(const FileQualityReport& report, float minQuality) {
    // Fichiers DEAD ne valent jamais la peine
    if (report.level == FileQualityLevel::DEAD) {
        return false;
    }
    
    // Fichiers CORRUPTED seulement si l'en-tête est valide
    if (report.level == FileQualityLevel::CORRUPTED) {
        return report.hasValidHeader;
    }
    
    // Pour les autres niveaux, vérifier le pourcentage
    return (100.0f - report.zeroPercentage) >= minQuality;
}

std::string FileQuality::getQualityDescription(FileQualityLevel level) {
    switch (level) {
        case FileQualityLevel::EXCELLENT:
            return "Excellent - File fully intact";
        case FileQualityLevel::GOOD:
            return "Good - File recoverable";
        case FileQualityLevel::POOR:
            return "Poor - Partially corrupted";
        case FileQualityLevel::CORRUPTED:
            return "Corrupted - Heavily damaged";
        case FileQualityLevel::DEAD:
            return "Dead - Overwritten/Unusable";
        default:
            return "Unknown";
    }
}

std::string FileQuality::getQualityColor(FileQualityLevel level) {
    switch (level) {
        case FileQualityLevel::EXCELLENT:
            return "green";
        case FileQualityLevel::GOOD:
            return "lightgreen";
        case FileQualityLevel::POOR:
            return "yellow";
        case FileQualityLevel::CORRUPTED:
            return "orange";
        case FileQualityLevel::DEAD:
            return "red";
        default:
            return "gray";
    }
}

float FileQuality::calculateZeroPercentage(const ByteArray& data) {
    if (data.empty()) return 100.0f;
    
    size_t zeroCount = std::count(data.begin(), data.end(), 0);
    return (static_cast<float>(zeroCount) / data.size()) * 100.0f;
}

bool FileQuality::checkHeader(const ByteArray& data, const std::string& fileType) {
    if (data.size() < 16) return false;
    
    // GVAS (Unreal Engine saves)
    if (fileType == "sav") {
        return data[0] == 0x47 && data[1] == 0x56 && 
               data[2] == 0x41 && data[3] == 0x53; // "GVAS"
    }
    
    // JPEG
    if (fileType == "jpg" || fileType == "jpeg") {
        return data[0] == 0xFF && data[1] == 0xD8 && data[2] == 0xFF;
    }
    
    // PNG
    if (fileType == "png") {
        return data[0] == 0x89 && data[1] == 0x50 && 
               data[2] == 0x4E && data[3] == 0x47;
    }
    
    // PDF
    if (fileType == "pdf") {
        return data[0] == 0x25 && data[1] == 0x50 && 
               data[2] == 0x44 && data[3] == 0x46; // "%PDF"
    }
    
    // ZIP/DOCX/XLSX
    if (fileType == "zip" || fileType == "docx" || fileType == "xlsx") {
        return data[0] == 0x50 && data[1] == 0x4B && 
               data[2] == 0x03 && data[3] == 0x04; // "PK\x03\x04"
    }
    
    // MP4
    if (fileType == "mp4") {
        return data[4] == 0x66 && data[5] == 0x74 && 
               data[6] == 0x79 && data[7] == 0x70; // "ftyp"
    }
    
    // Pour les types inconnus, considérer valide si les 4 premiers bytes ne sont pas tous 0
    return data[0] != 0 || data[1] != 0 || data[2] != 0 || data[3] != 0;
}

bool FileQuality::checkFooter(const ByteArray& data, const std::string& fileType) {
    if (data.size() < 16) return false;
    
    // GVAS - vérifier le pattern "None" + footer
    if (fileType == "sav") {
        // Chercher le pattern 05 00 00 00 4E 6F 6E 65 00 dans les derniers 100 bytes
        if (data.size() < 100) return false;
        
        for (size_t i = data.size() - 100; i < data.size() - 9; ++i) {
            if (data[i] == 0x05 && data[i+1] == 0x00 && 
                data[i+2] == 0x00 && data[i+3] == 0x00 &&
                data[i+4] == 'N' && data[i+5] == 'o' && 
                data[i+6] == 'n' && data[i+7] == 'e' &&
                data[i+8] == 0x00) {
                // Vérifier qu'il reste environ 13 bytes après ce pattern
                size_t bytesAfter = data.size() - i;
                return bytesAfter >= 9 && bytesAfter <= 20; // Tolérance de quelques bytes
            }
        }
        return false;
    }
    
    // JPEG
    if (fileType == "jpg" || fileType == "jpeg") {
        size_t size = data.size();
        return size >= 2 && data[size-2] == 0xFF && data[size-1] == 0xD9;
    }
    
    // PNG
    if (fileType == "png") {
        size_t size = data.size();
        return size >= 8 && 
               data[size-8] == 0x49 && data[size-7] == 0x45 &&
               data[size-6] == 0x4E && data[size-5] == 0x44 &&
               data[size-4] == 0xAE && data[size-3] == 0x42 &&
               data[size-2] == 0x60 && data[size-1] == 0x82; // IEND + CRC
    }
    
    // PDF
    if (fileType == "pdf") {
        size_t size = data.size();
        if (size < 6) return false;
        // Chercher "%%EOF" dans les derniers 100 bytes
        std::string end(data.end() - std::min(size_t(100), size), data.end());
        return end.find("%%EOF") != std::string::npos;
    }
    
    // Pour les autres types, pas de vérification spécifique
    return true;
}

FileQualityLevel FileQuality::determineQualityLevel(float zeroPercentage, bool hasHeader, bool hasFooter) {
    // Fichier mort (>99% zeros)
    if (zeroPercentage > 99.0f) {
        return FileQualityLevel::DEAD;
    }
    
    // Fichier très corrompu (90-99% zeros)
    if (zeroPercentage > 90.0f) {
        return FileQualityLevel::CORRUPTED;
    }
    
    // Fichier partiellement corrompu (60-90% zeros)
    if (zeroPercentage > 60.0f) {
        return FileQualityLevel::POOR;
    }
    
    // Fichier bon (30-60% zeros)
    if (zeroPercentage > 30.0f) {
        // Si l'en-tête et le footer sont valides, upgrader à EXCELLENT
        if (hasHeader && hasFooter) {
            return FileQualityLevel::EXCELLENT;
        }
        return FileQualityLevel::GOOD;
    }
    
    // Fichier excellent (<30% zeros)
    return FileQualityLevel::EXCELLENT;
}

std::string FileQuality::generateRecommendation(const FileQualityReport& report) {
    switch (report.level) {
        case FileQualityLevel::EXCELLENT:
            return "File is in excellent condition and should be fully usable.";
        
        case FileQualityLevel::GOOD:
            return "File is in good condition. Minor data loss possible but should be usable.";
        
        case FileQualityLevel::POOR:
            if (report.hasValidHeader) {
                return "File is partially corrupted but may contain recoverable data. Try opening it.";
            }
            return "File is partially corrupted. Recovery uncertain.";
        
        case FileQualityLevel::CORRUPTED:
            if (report.hasValidHeader) {
                return "File is heavily corrupted but header is intact. May be partially recoverable.";
            }
            return "File is heavily corrupted. Recovery unlikely.";
        
        case FileQualityLevel::DEAD:
            return "File has been overwritten with zeros. Data is unrecoverable. Skip this file.";
        
        default:
            return "Unable to determine file quality.";
    }
}

} // namespace FileRecovery
