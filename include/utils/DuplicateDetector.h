#pragma once

#include "RecoveryTypes.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace FileRecovery {
namespace Utils {

// Structure représentant un fichier avec son hash
struct FileHash {
    std::string filePath;
    std::string sha256;         // Hash SHA256 (64 caractères hex)
    size_t fileSize;
    std::string fileType;
    float quality;              // Score de qualité (0.0-1.0)

    // Métadonnées optionnelles
    bool hasValidHeader;
    bool hasValidFooter;
    float entropy;
};

// Groupe de fichiers dupliqués
struct DuplicateGroup {
    std::string hash;           // Hash commun
    size_t fileSize;            // Taille commune
    std::vector<FileHash> files; // Tous les fichiers avec ce hash
    int bestQualityIndex;       // Index du fichier de meilleure qualité
};

/**
 * @brief Détecteur de fichiers dupliqués basé sur SHA256
 *
 * Permet de:
 * - Hash tous les fichiers récupérés
 * - Grouper les duplicatas
 * - Identifier le meilleur exemplaire de chaque groupe
 * - Supprimer les copies de moindre qualité
 */
class DuplicateDetector {
public:
    DuplicateDetector();
    ~DuplicateDetector();

    /**
     * @brief Hash un fichier avec SHA256
     * @param filePath Chemin du fichier
     * @return Hash SHA256 en hexadécimal (64 caractères)
     */
    std::string hashFile(const std::string& filePath);

    /**
     * @brief Hash des données en mémoire
     * @param data Données à hasher
     * @return Hash SHA256 en hexadécimal
     */
    std::string hashBuffer(const ByteArray& data);

    /**
     * @brief Ajoute un fichier au détecteur
     * @param fileHash Structure FileHash complète
     */
    void addFile(const FileHash& fileHash);

    /**
     * @brief Analyse tous les fichiers et trouve les duplicatas
     * @return Vector de groupes de duplicatas (seulement groupes avec >= 2 fichiers)
     */
    std::vector<DuplicateGroup> findDuplicates();

    /**
     * @brief Obtient les statistiques de duplication
     * @param totalFiles Nombre total de fichiers
     * @param uniqueFiles Nombre de fichiers uniques
     * @param duplicateFiles Nombre de fichiers en double
     * @param wastedSpace Espace gaspillé par les duplicatas (bytes)
     */
    void getStatistics(size_t& totalFiles, size_t& uniqueFiles,
                       size_t& duplicateFiles, size_t& wastedSpace);

    /**
     * @brief Garde seulement les meilleurs exemplaires de chaque groupe
     * @return Liste des fichiers à supprimer (duplicatas de moindre qualité)
     */
    std::vector<std::string> getFilesToDelete();

    /**
     * @brief Efface toutes les données
     */
    void clear();

private:
    std::vector<FileHash> files_;
    std::map<std::string, std::vector<size_t>> hashToIndices_; // hash -> indices dans files_

    // Détermine quel fichier est le meilleur dans un groupe
    int selectBestFile(const std::vector<FileHash>& group);

    // SHA256 implementation (simple)
    void sha256(const uint8_t* data, size_t length, uint8_t* hash);
    std::string bytesToHex(const uint8_t* bytes, size_t length);
};

} // namespace Utils
} // namespace FileRecovery
