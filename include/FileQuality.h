#pragma once

#include "RecoveryTypes.h"
#include <string>
#include <vector>

namespace FileRecovery {

enum class FileQualityLevel {
    EXCELLENT,  // Fichier complet et non corrompu (< 30% zeros)
    GOOD,       // Fichier récupérable (30-60% zeros)
    POOR,       // Fichier partiellement corrompu (60-90% zeros)
    CORRUPTED,  // Fichier très corrompu (90-99% zeros)
    DEAD        // Fichier écrasé/inutilisable (> 99% zeros)
};

struct FileQualityReport {
    FileQualityLevel level;
    float zeroPercentage;
    size_t totalBytes;
    size_t zeroBytes;
    size_t nonZeroBytes;
    bool hasValidHeader;
    bool hasValidFooter;
    std::string recommendation;
};

class FileQuality {
public:
    FileQuality();
    ~FileQuality();
    
    // Analyser la qualité d'un fichier depuis son chemin
    FileQualityReport analyzeFile(const std::string& filePath);
    
    // Analyser la qualité depuis des données en mémoire
    FileQualityReport analyzeBuffer(const ByteArray& data, const std::string& fileType);
    
    // Vérifier si un fichier vaut la peine d'être récupéré
    bool isWorthRecovering(const FileQualityReport& report, float minQuality = 60.0f);
    
    // Obtenir une description textuelle du niveau de qualité
    static std::string getQualityDescription(FileQualityLevel level);
    
    // Obtenir une couleur pour l'affichage (pour future GUI)
    static std::string getQualityColor(FileQualityLevel level);
    
private:
    // Calculer le pourcentage de zéros dans les données
    float calculateZeroPercentage(const ByteArray& data);
    
    // Vérifier l'en-tête du fichier selon son type
    bool checkHeader(const ByteArray& data, const std::string& fileType);
    
    // Vérifier le footer du fichier selon son type
    bool checkFooter(const ByteArray& data, const std::string& fileType);
    
    // Déterminer le niveau de qualité basé sur les métriques
    FileQualityLevel determineQualityLevel(float zeroPercentage, bool hasHeader, bool hasFooter);
    
    // Générer une recommandation basée sur l'analyse
    std::string generateRecommendation(const FileQualityReport& report);
};

} // namespace FileRecovery
