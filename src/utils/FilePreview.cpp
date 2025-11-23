#include "utils/FilePreview.h"
#include "utils/Logger.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

// STB Image libraries for image decoding and resizing
#include "external/stb_image.h"
#include "external/stb_image_resize.h"

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

    std::string ext = fileType;
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    // Decode image using stb_image
    int width, height, channels;
    unsigned char* imgData = stbi_load_from_memory(
        data.data(),
        static_cast<int>(data.size()),
        &width,
        &height,
        &channels,
        4  // Force RGBA output
    );

    if (!imgData) {
        // Decoding failed, create error preview
        Logger::getInstance().log(LogLevel::WARNING, "Failed to decode " + ext + " image: " + std::string(stbi_failure_reason()));
        preview.error = "Failed to decode image: " + std::string(stbi_failure_reason());

        // Create placeholder thumbnail
        preview.thumbnailWidth = 128;
        preview.thumbnailHeight = 128;
        preview.thumbnailData.resize(128 * 128 * 4);

        // Fill with error pattern (red/gray checkerboard)
        for (int y = 0; y < 128; ++y) {
            for (int x = 0; x < 128; ++x) {
                size_t idx = (y * 128 + x) * 4;
                bool isRed = ((x / 16) + (y / 16)) % 2 == 0;
                preview.thumbnailData[idx + 0] = isRed ? 200 : 50;  // R
                preview.thumbnailData[idx + 1] = isRed ? 50 : 50;   // G
                preview.thumbnailData[idx + 2] = isRed ? 50 : 50;   // B
                preview.thumbnailData[idx + 3] = 255;               // A
            }
        }
        return preview;
    }

    Logger::getInstance().log(LogLevel::INFO, "Successfully decoded " + ext + " image: " + std::to_string(width) +
             "x" + std::to_string(height) + " (" + std::to_string(channels) + " channels)");

    // Store original dimensions
    preview.width = width;
    preview.height = height;

    // Generate thumbnail (128x128) using stb_image_resize
    const int thumbWidth = 128;
    const int thumbHeight = 128;

    preview.thumbnailWidth = thumbWidth;
    preview.thumbnailHeight = thumbHeight;
    preview.thumbnailData.resize(thumbWidth * thumbHeight * 4);

    // Resize using stb_image_resize with high-quality algorithm
    bool result = stbir_resize_uint8_linear(
        imgData, width, height, 0,
        preview.thumbnailData.data(), thumbWidth, thumbHeight, 0,
        STBIR_RGBA
    ) != 0;

    if (!result) {
        Logger::getInstance().log(LogLevel::WARNING, "Failed to resize thumbnail, using simple copy");
        // Fallback: just copy top-left corner
        int copyW = std::min(width, thumbWidth);
        int copyH = std::min(height, thumbHeight);
        for (int y = 0; y < copyH; ++y) {
            for (int x = 0; x < copyW; ++x) {
                size_t srcIdx = (y * width + x) * 4;
                size_t dstIdx = (y * thumbWidth + x) * 4;
                preview.thumbnailData[dstIdx + 0] = imgData[srcIdx + 0];
                preview.thumbnailData[dstIdx + 1] = imgData[srcIdx + 1];
                preview.thumbnailData[dstIdx + 2] = imgData[srcIdx + 2];
                preview.thumbnailData[dstIdx + 3] = imgData[srcIdx + 3];
            }
        }
    }

    // Free the original decoded image
    stbi_image_free(imgData);

    preview.error = ""; // No error
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

// ============================================================================
// DÉCODEURS D'IMAGES (utilisant stb_image)
// ============================================================================
bool FilePreview::decodeJPEG(const ByteArray& data, std::vector<uint8_t>& rgba, int& width, int& height) {
    int channels;
    unsigned char* imgData = stbi_load_from_memory(
        data.data(),
        static_cast<int>(data.size()),
        &width,
        &height,
        &channels,
        4  // Force RGBA output
    );

    if (!imgData) {
        Logger::getInstance().log(LogLevel::ERROR, "Failed to decode JPEG: " + std::string(stbi_failure_reason()));
        return false;
    }

    // Copy data to output vector
    size_t dataSize = width * height * 4;
    rgba.resize(dataSize);
    std::memcpy(rgba.data(), imgData, dataSize);

    // Free stb_image buffer
    stbi_image_free(imgData);

    Logger::getInstance().log(LogLevel::DEBUG, "JPEG decoded successfully: " + std::to_string(width) + "x" + std::to_string(height));
    return true;
}

bool FilePreview::decodePNG(const ByteArray& data, std::vector<uint8_t>& rgba, int& width, int& height) {
    int channels;
    unsigned char* imgData = stbi_load_from_memory(
        data.data(),
        static_cast<int>(data.size()),
        &width,
        &height,
        &channels,
        4  // Force RGBA output
    );

    if (!imgData) {
        Logger::getInstance().log(LogLevel::ERROR, "Failed to decode PNG: " + std::string(stbi_failure_reason()));
        return false;
    }

    // Copy data to output vector
    size_t dataSize = width * height * 4;
    rgba.resize(dataSize);
    std::memcpy(rgba.data(), imgData, dataSize);

    // Free stb_image buffer
    stbi_image_free(imgData);

    Logger::getInstance().log(LogLevel::DEBUG, "PNG decoded successfully: " + std::to_string(width) + "x" + std::to_string(height));
    return true;
}

void FilePreview::resizeThumbnail(const std::vector<uint8_t>& srcRGBA, int srcW, int srcH,
                                  std::vector<uint8_t>& dstRGBA, int dstW, int dstH) {
    dstRGBA.resize(dstW * dstH * 4);

    // Use stb_image_resize for high-quality resizing
    bool result = stbir_resize_uint8_linear(
        srcRGBA.data(), srcW, srcH, 0,
        dstRGBA.data(), dstW, dstH, 0,
        STBIR_RGBA
    ) != 0;

    if (!result) {
        Logger::getInstance().log(LogLevel::WARNING, "stb_image_resize failed, using fallback nearest-neighbor");

        // Fallback: simple nearest-neighbor
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
}

} // namespace Utils
} // namespace FileRecovery
