#pragma once

#include "RecoveryTypes.h"
#include <string>
#include <vector>
#include <memory>

namespace FileRecovery {
namespace Utils {

enum class PreviewType {
    IMAGE,      // Miniature d'image
    TEXT,       // Aperçu texte (100 premières lignes)
    HEX,        // Hex dump
    BINARY,     // Fichier binaire non prévisualisable
    UNSUPPORTED // Type non supporté pour preview
};

// Structure contenant les données de preview
struct PreviewData {
    PreviewType type;
    std::string fileType;
    size_t fileSize;

    // Pour images : dimensions originales et miniature
    int width = 0;              // Original image width
    int height = 0;             // Original image height
    std::vector<uint8_t> thumbnailData;
    int thumbnailWidth = 0;
    int thumbnailHeight = 0;

    // Pour texte : lignes extraites
    std::vector<std::string> textLines;
    size_t totalLines = 0;

    // Pour hex : premiers bytes
    std::vector<uint8_t> hexData;
    size_t hexDataOffset = 0;

    std::string error; // Message d'erreur si échec
};

/**
 * @brief Générateur d'aperçus de fichiers pour la GUI
 *
 * Génère des miniatures pour images, extrait du texte, ou affiche hex dump
 * selon le type de fichier.
 */
class FilePreview {
public:
    FilePreview();
    ~FilePreview();

    /**
     * @brief Génère un aperçu depuis un chemin de fichier
     * @param filePath Chemin du fichier à prévisualiser
     * @param fileType Extension du fichier (jpg, png, txt, etc.)
     * @return PreviewData contenant l'aperçu généré
     */
    PreviewData generatePreview(const std::string& filePath, const std::string& fileType);

    /**
     * @brief Génère un aperçu depuis des données en mémoire
     * @param data Données du fichier
     * @param fileType Extension du fichier
     * @return PreviewData contenant l'aperçu généré
     */
    PreviewData generatePreviewFromBuffer(const ByteArray& data, const std::string& fileType);

    /**
     * @brief Vérifie si un type de fichier est prévisualisable
     */
    static bool isPreviewable(const std::string& fileType);

    /**
     * @brief Obtient le type de preview pour un type de fichier
     */
    static PreviewType getPreviewType(const std::string& fileType);

private:
    // Génération de miniatures d'images
    PreviewData generateImageThumbnail(const ByteArray& data, const std::string& fileType);

    // Extraction de texte
    PreviewData generateTextPreview(const ByteArray& data, size_t maxLines = 100);

    // Génération de hex dump
    PreviewData generateHexDump(const ByteArray& data, size_t maxBytes = 512);

    // Décodeurs d'images simplifiés (sans bibliothèque externe)
    bool decodeJPEG(const ByteArray& data, std::vector<uint8_t>& rgba, int& width, int& height);
    bool decodePNG(const ByteArray& data, std::vector<uint8_t>& rgba, int& width, int& height);

    // Redimensionnement de miniature
    void resizeThumbnail(const std::vector<uint8_t>& srcRGBA, int srcW, int srcH,
                        std::vector<uint8_t>& dstRGBA, int dstW, int dstH);
};

} // namespace Utils
} // namespace FileRecovery
