#include "../include/FileQuality.h"
#include "../utils/Logger.h"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <map>

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

    // Anciennes métriques (conservées pour compatibilité)
    report.zeroPercentage = calculateZeroPercentage(data);
    report.zeroBytes = static_cast<size_t>(data.size() * report.zeroPercentage / 100.0f);
    report.nonZeroBytes = data.size() - report.zeroBytes;

    // NOUVELLES MÉTRIQUES INTELLIGENTES
    report.entropy = calculateEntropy(data);
    report.hasValidHeader = checkHeader(data, fileType);
    report.hasValidFooter = checkFooter(data, fileType);
    report.hasValidStructure = validateStructure(data, fileType);
    report.hasRepeatingPatterns = detectRepeatingPatterns(data);
    report.dataConsistency = calculateDataConsistency(data, fileType);

    // NOUVELLE LOGIQUE : utilise entropie + structure au lieu du % zéros
    report.level = determineQualityLevel(report);

    // Générer une recommandation
    report.recommendation = generateRecommendation(report);
    report.detailedAnalysis = generateDetailedAnalysis(report, fileType);

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

// ============================================================================
// NOUVELLE LOGIQUE INTELLIGENTE DE DÉTERMINATION DE QUALITÉ
// ============================================================================
FileQualityLevel FileQuality::determineQualityLevel(const FileQualityReport& report) {
    // Critère 1: Fichier complètement vide ou écrasé
    if (report.zeroPercentage > 99.0f && report.entropy < 0.5f) {
        return FileQualityLevel::DEAD;
    }

    // Critère 2: Entropie anormalement haute = garbage aléatoire = DEAD
    // (Fichiers normaux ont entropie 4.0-7.5, garbage = ~7.9-8.0)
    if (report.entropy > 7.8f && !report.hasValidHeader) {
        return FileQualityLevel::DEAD;
    }

    // Critère 3: Patterns répétitifs suspects sans header valide = DEAD
    if (report.hasRepeatingPatterns && !report.hasValidHeader && report.dataConsistency < 0.3f) {
        return FileQualityLevel::DEAD;
    }

    // Critère 4: Header invalide + footer invalide + entropie bizarre = CORRUPTED
    if (!report.hasValidHeader && !report.hasValidFooter &&
        (report.entropy < 2.0f || report.entropy > 7.8f)) {
        return FileQualityLevel::CORRUPTED;
    }

    // Critère 5: Structure valide + header/footer OK = EXCELLENT
    if (report.hasValidStructure && report.hasValidHeader && report.hasValidFooter &&
        report.dataConsistency > 0.8f) {
        return FileQualityLevel::EXCELLENT;
    }

    // Critère 6: Header valide + entropie normale + cohérence bonne = GOOD
    if (report.hasValidHeader && report.entropy >= 3.0f && report.entropy <= 7.5f &&
        report.dataConsistency > 0.6f) {
        return FileQualityLevel::GOOD;
    }

    // Critère 7: Header valide mais footer absent = POOR (tronqué)
    if (report.hasValidHeader && !report.hasValidFooter) {
        return FileQualityLevel::POOR;
    }

    // Critère 8: Data consistency faible = POOR
    if (report.dataConsistency < 0.5f && report.hasValidHeader) {
        return FileQualityLevel::POOR;
    }

    // Par défaut: CORRUPTED si rien ne passe
    return FileQualityLevel::CORRUPTED;
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

// ============================================================================
// NOUVELLES FONCTIONS INTELLIGENTES
// ============================================================================

// Calculer l'entropie de Shannon (mesure du désordre/randomness)
// Entropie: 0.0 = données uniformes, 8.0 = maximum random
float FileQuality::calculateEntropy(const ByteArray& data) {
    if (data.empty()) return 0.0f;

    // Compter la fréquence de chaque byte
    std::vector<size_t> frequency(256, 0);
    for (uint8_t byte : data) {
        frequency[byte]++;
    }

    // Calculer l'entropie de Shannon: H = -Σ(p_i * log2(p_i))
    float entropy = 0.0f;
    size_t totalBytes = data.size();

    for (size_t count : frequency) {
        if (count > 0) {
            float probability = static_cast<float>(count) / totalBytes;
            entropy -= probability * std::log2(probability);
        }
    }

    return entropy;
}

// Valider la structure interne du fichier (spécifique au format)
bool FileQuality::validateStructure(const ByteArray& data, const std::string& fileType) {
    if (data.size() < 16) return false;

    // JPEG: Vérifier les marqueurs internes
    if (fileType == "jpg" || fileType == "jpeg") {
        // Un JPEG valide doit avoir des marqueurs FF Dx réguliers
        size_t markerCount = 0;
        for (size_t i = 0; i < data.size() - 1; ++i) {
            if (data[i] == 0xFF && (data[i+1] >= 0xC0 && data[i+1] <= 0xFE)) {
                markerCount++;
            }
        }
        return markerCount >= 3; // Au moins 3 marqueurs JPEG
    }

    // PNG: Vérifier les chunks (longueur + type + CRC)
    if (fileType == "png") {
        size_t pos = 8; // Skip header
        int validChunks = 0;
        while (pos + 12 < data.size() && validChunks < 3) {
            // PNG chunk: 4 bytes length + 4 bytes type + data + 4 bytes CRC
            uint32_t chunkLen = (data[pos] << 24) | (data[pos+1] << 16) |
                                (data[pos+2] << 8) | data[pos+3];
            if (chunkLen < 1000000) { // Sanity check
                validChunks++;
                pos += 12 + chunkLen;
            } else {
                break;
            }
        }
        return validChunks >= 2;
    }

    // PDF: Vérifier les objets internes
    if (fileType == "pdf") {
        std::string content(data.begin(), data.end());
        size_t objCount = 0;
        size_t pos = 0;
        while ((pos = content.find(" obj", pos)) != std::string::npos) {
            objCount++;
            pos += 4;
        }
        return objCount >= 2; // Au moins 2 objets PDF
    }

    // ZIP/DOCX/XLSX: Vérifier les entrées du directory central
    if (fileType == "zip" || fileType == "docx" || fileType == "xlsx") {
        // Chercher la signature du central directory (PK\x01\x02)
        for (size_t i = 0; i < data.size() - 4; ++i) {
            if (data[i] == 0x50 && data[i+1] == 0x4B &&
                data[i+2] == 0x01 && data[i+3] == 0x02) {
                return true; // Central directory trouvé
            }
        }
        return false;
    }

    // Pour les autres types, vérifier juste que le fichier n'est pas vide
    return data.size() > 100;
}

// Détecter des patterns répétitifs suspects (signe de corruption)
bool FileQuality::detectRepeatingPatterns(const ByteArray& data) {
    if (data.size() < 64) return false;

    // Vérifier si un pattern de 4 bytes se répète plus de 50% du fichier
    std::map<uint32_t, size_t> patternCounts;
    size_t totalPatterns = data.size() / 4;

    for (size_t i = 0; i < data.size() - 3; i += 4) {
        uint32_t pattern = (data[i] << 24) | (data[i+1] << 16) |
                          (data[i+2] << 8) | data[i+3];
        patternCounts[pattern]++;
    }

    // Si un pattern unique représente > 50% des données = suspect
    for (const auto& [pattern, count] : patternCounts) {
        if (count > totalPatterns / 2) {
            // Exception: pattern 0x00000000 peut être légitime (sparse files)
            if (pattern != 0x00000000) {
                return true;
            }
        }
    }

    return false;
}

// Calculer la cohérence des données (basé sur le type de fichier)
float FileQuality::calculateDataConsistency(const ByteArray& data, const std::string& fileType) {
    if (data.empty()) return 0.0f;

    float consistency = 1.0f;

    // Pour images: vérifier que les pixels ne sont pas complètement aléatoires
    if (fileType == "jpg" || fileType == "jpeg" || fileType == "png" || fileType == "bmp") {
        // Mesurer la variation entre bytes adjacents
        size_t highVariation = 0;
        size_t sampleSize = std::min(data.size() - 1, size_t(1000));

        for (size_t i = 0; i < sampleSize; ++i) {
            int diff = std::abs(static_cast<int>(data[i]) - static_cast<int>(data[i+1]));
            if (diff > 200) { // Variation très élevée
                highVariation++;
            }
        }

        float variationRatio = static_cast<float>(highVariation) / sampleSize;
        consistency = 1.0f - variationRatio; // Moins de variation = plus cohérent
    }

    // Pour PDF: vérifier la présence de mots-clés PDF
    if (fileType == "pdf") {
        std::string content(data.begin(), data.end());
        int keywordCount = 0;
        if (content.find("endobj") != std::string::npos) keywordCount++;
        if (content.find("stream") != std::string::npos) keywordCount++;
        if (content.find("endstream") != std::string::npos) keywordCount++;
        consistency = static_cast<float>(keywordCount) / 3.0f;
    }

    // Pour fichiers compressés: vérifier que l'entropie est élevée (normal pour compressé)
    if (fileType == "zip" || fileType == "7z" || fileType == "rar") {
        float entropy = calculateEntropy(data);
        // Fichiers compressés doivent avoir haute entropie (7.0+)
        consistency = (entropy >= 7.0f) ? 1.0f : (entropy / 7.0f);
    }

    return std::max(0.0f, std::min(1.0f, consistency));
}

// Générer une analyse détaillée pour le debug
std::string FileQuality::generateDetailedAnalysis(const FileQualityReport& report, const std::string& fileType) {
    std::stringstream ss;

    ss << "File Type: " << fileType << "\n";
    ss << "Size: " << report.totalBytes << " bytes\n";
    ss << "Entropy: " << report.entropy << " (0=uniform, 8=random)\n";
    ss << "Zero %: " << report.zeroPercentage << "%\n";
    ss << "Header Valid: " << (report.hasValidHeader ? "YES" : "NO") << "\n";
    ss << "Footer Valid: " << (report.hasValidFooter ? "YES" : "NO") << "\n";
    ss << "Structure Valid: " << (report.hasValidStructure ? "YES" : "NO") << "\n";
    ss << "Repeating Patterns: " << (report.hasRepeatingPatterns ? "DETECTED" : "None") << "\n";
    ss << "Data Consistency: " << (report.dataConsistency * 100.0f) << "%\n";

    // Interprétation de l'entropie
    ss << "\nEntropy Analysis:\n";
    if (report.entropy < 2.0f) {
        ss << "  - Very low entropy = mostly uniform data (likely overwritten)\n";
    } else if (report.entropy < 4.0f) {
        ss << "  - Low entropy = simple data or partially corrupted\n";
    } else if (report.entropy < 7.5f) {
        ss << "  - Normal entropy = typical file data\n";
    } else if (report.entropy < 7.9f) {
        ss << "  - High entropy = compressed or encrypted data\n";
    } else {
        ss << "  - Maximum entropy = random garbage or strong encryption\n";
    }

    return ss.str();
}

} // namespace FileRecovery
