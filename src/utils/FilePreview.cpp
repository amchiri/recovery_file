#include "utils/FilePreview.h"
#include "utils/Logger.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace FileRecovery {
namespace Utils {

FilePreview::FilePreview() {}
FilePreview::~FilePreview() {}

PreviewData FilePreview::generatePreview(const std::string& filePath, const std::string& fileType) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        PreviewData preview;
        preview.type = PreviewType::UNSUPPORTED;
        preview.error = "Cannot open file";
        return preview;
    }

    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    ByteArray data(fileSize);
    file.read(reinterpret_cast<char*>(data.data()), fileSize);
    file.close();

    return generatePreviewFromBuffer(data, fileType);
}

PreviewData FilePreview::generatePreviewFromBuffer(const ByteArray& data, const std::string& fileType) {
    PreviewType type = getPreviewType(fileType);

    switch (type) {
        case PreviewType::IMAGE:
            return generateImageThumbnail(data, fileType);

        case PreviewType::TEXT:
            return generateTextPreview(data);

        case PreviewType::HEX:
        case PreviewType::BINARY:
            return generateHexDump(data);

        default:
            PreviewData preview;
            preview.type = PreviewType::UNSUPPORTED;
            preview.fileType = fileType;
            preview.fileSize = data.size();
            preview.error = "Preview not supported for this file type";
            return preview;
    }
}

bool FilePreview::isPreviewable(const std::string& fileType) {
    return getPreviewType(fileType) != PreviewType::UNSUPPORTED;
}

PreviewType FilePreview::getPreviewType(const std::string& fileType) {
    std::string ext = fileType;
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    // Images
    if (ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "bmp" ||
        ext == "gif" || ext == "webp") {
        return PreviewType::IMAGE;
    }

    // Texte
    if (ext == "txt" || ext == "log" || ext == "md" || ext == "json" ||
        ext == "xml" || ext == "html" || ext == "css" || ext == "js" ||
        ext == "py" || ext == "cpp" || ext == "h" || ext == "c" ||
        ext == "csv" || ext == "ini" || ext == "conf") {
        return PreviewType::TEXT;
    }

    // Binaires avec hex dump
    if (ext == "exe" || ext == "dll" || ext == "dat" || ext == "bin") {
        return PreviewType::HEX;
    }

    return PreviewType::UNSUPPORTED;
}

// ============================================================================
// GÉNÉRATION DE MINIATURES D'IMAGES
// ============================================================================
PreviewData FilePreview::generateImageThumbnail(const ByteArray& data, const std::string& fileType) {
    PreviewData preview;
    preview.type = PreviewType::IMAGE;
    preview.fileType = fileType;
    preview.fileSize = data.size();

    // Pour l'instant, génération simplifiée sans décodage complet
    // On extrait juste les métadonnées de base et crée une miniature placeholder

    std::string ext = fileType;
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    // Placeholder: créer une miniature de 128x128 avec un dégradé basé sur les données
    preview.thumbnailWidth = 128;
    preview.thumbnailHeight = 128;
    preview.thumbnailData.resize(128 * 128 * 4); // RGBA

    // Générer un pattern visuel basé sur le contenu du fichier
    // (simple hash visuel pour identifier l'image)
    for (int y = 0; y < 128; ++y) {
        for (int x = 0; x < 128; ++x) {
            size_t idx = (y * 128 + x) * 4;
            size_t dataIdx = (y * 128 + x) % data.size();

            // Créer un pattern coloré basé sur les données
            preview.thumbnailData[idx + 0] = data[dataIdx];                     // R
            preview.thumbnailData[idx + 1] = data[(dataIdx + 1) % data.size()]; // G
            preview.thumbnailData[idx + 2] = data[(dataIdx + 2) % data.size()]; // B
            preview.thumbnailData[idx + 3] = 255;                                // A
        }
    }

    preview.error = "Placeholder thumbnail (full decoding not implemented)";
    return preview;
}

// ============================================================================
// EXTRACTION DE TEXTE
// ============================================================================
PreviewData FilePreview::generateTextPreview(const ByteArray& data, size_t maxLines) {
    PreviewData preview;
    preview.type = PreviewType::TEXT;
    preview.fileSize = data.size();
    preview.totalLines = 0;

    std::string content(data.begin(), data.end());
    std::istringstream stream(content);
    std::string line;

    while (std::getline(stream, line) && preview.textLines.size() < maxLines) {
        // Vérifier que la ligne est du texte valide (pas de caractères binaires)
        bool isText = true;
        for (char c : line) {
            if (!std::isprint(static_cast<unsigned char>(c)) && c != '\t') {
                isText = false;
                break;
            }
        }

        if (isText) {
            // Limiter la longueur de la ligne pour l'affichage
            if (line.length() > 120) {
                line = line.substr(0, 120) + "...";
            }
            preview.textLines.push_back(line);
        } else {
            // Ligne binaire détectée, arrêter l'extraction
            preview.error = "Binary data detected, stopping text extraction";
            break;
        }
        preview.totalLines++;
    }

    // Compter le nombre total de lignes restantes
    while (std::getline(stream, line)) {
        preview.totalLines++;
    }

    if (preview.textLines.empty()) {
        preview.error = "No readable text found";
    }

    return preview;
}

// ============================================================================
// GÉNÉRATION DE HEX DUMP
// ============================================================================
PreviewData FilePreview::generateHexDump(const ByteArray& data, size_t maxBytes) {
    PreviewData preview;
    preview.type = PreviewType::HEX;
    preview.fileSize = data.size();
    preview.hexDataOffset = 0;

    size_t bytesToShow = std::min(maxBytes, data.size());
    preview.hexData.assign(data.begin(), data.begin() + bytesToShow);

    // Formater en hex pour affichage futur dans la GUI
    // Le formatage sera fait côté GUI avec ImGui

    return preview;
}

// Décodeurs d'images (stubs pour l'instant - nécessiteraient stb_image ou similaire)
bool FilePreview::decodeJPEG(const ByteArray& data, std::vector<uint8_t>& rgba, int& width, int& height) {
    // TODO: Implémenter avec stb_image ou libjpeg
    return false;
}

bool FilePreview::decodePNG(const ByteArray& data, std::vector<uint8_t>& rgba, int& width, int& height) {
    // TODO: Implémenter avec stb_image ou libpng
    return false;
}

void FilePreview::resizeThumbnail(const std::vector<uint8_t>& srcRGBA, int srcW, int srcH,
                                  std::vector<uint8_t>& dstRGBA, int dstW, int dstH) {
    dstRGBA.resize(dstW * dstH * 4);

    // Redimensionnement nearest-neighbor simple
    for (int y = 0; y < dstH; ++y) {
        for (int x = 0; x < dstW; ++x) {
            int srcX = x * srcW / dstW;
            int srcY = y * srcH / dstH;

            size_t srcIdx = (srcY * srcW + srcX) * 4;
            size_t dstIdx = (y * dstW + x) * 4;

            dstRGBA[dstIdx + 0] = srcRGBA[srcIdx + 0];
            dstRGBA[dstIdx + 1] = srcRGBA[srcIdx + 1];
            dstRGBA[dstIdx + 2] = srcRGBA[srcIdx + 2];
            dstRGBA[dstIdx + 3] = srcRGBA[srcIdx + 3];
        }
    }
}

} // namespace Utils
} // namespace FileRecovery
