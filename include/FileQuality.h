#pragma once

#include "RecoveryTypes.h"
#include <string>
#include <vector>

namespace FileRecovery {

enum class FileQualityLevel {
    EXCELLENT,  // Fichier intact avec structure valide et entropie normale
    GOOD,       // Fichier récupérable avec quelques anomalies mineures
    POOR,       // Fichier partiellement corrompu mais récupérable
    CORRUPTED,  // Fichier très corrompu, récupération partielle possible
    DEAD        // Fichier inutilisable (overwritten, entropie anormale)
};

struct FileQualityReport {
    FileQualityLevel level;

    // Anciennes métriques (conservées pour compatibilité)
    float zeroPercentage;
    size_t totalBytes;
    size_t zeroBytes;
    size_t nonZeroBytes;

    // Nouvelles métriques intelligentes
    float entropy;              // Entropie de Shannon (0.0-8.0)
    bool hasValidHeader;
    bool hasValidFooter;
    bool hasValidStructure;     // Cohérence interne du format
    bool hasRepeatingPatterns;  // Détection de patterns suspects
    float dataConsistency;      // Cohérence des données (0.0-1.0)

    std::string recommendation;
    std::string detailedAnalysis; // Analyse détaillée pour debug
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
    // Anciennes métriques (conservées)
    float calculateZeroPercentage(const ByteArray& data);

    // NOUVELLES MÉTRIQUES INTELLIGENTES
    float calculateEntropy(const ByteArray& data);
    bool validateStructure(const ByteArray& data, const std::string& fileType);
    bool detectRepeatingPatterns(const ByteArray& data);
    float calculateDataConsistency(const ByteArray& data, const std::string& fileType);

    // Vérifications header/footer (améliorées)
    bool checkHeader(const ByteArray& data, const std::string& fileType);
    bool checkFooter(const ByteArray& data, const std::string& fileType);

    // NOUVELLE LOGIQUE DE DÉTERMINATION
    FileQualityLevel determineQualityLevel(const FileQualityReport& report);

    // Générer une recommandation basée sur l'analyse
    std::string generateRecommendation(const FileQualityReport& report);
    std::string generateDetailedAnalysis(const FileQualityReport& report, const std::string& fileType);
};

} // namespace FileRecovery
