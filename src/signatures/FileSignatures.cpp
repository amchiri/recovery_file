#include "../include/FileSignatures.h"
#include "SignatureDatabase.h"
#include "SignatureLoader.h"
#include "../utils/Logger.h"
#include <algorithm>
#include <filesystem>

namespace FileRecovery {

class FileSignatures::Impl {
public:
    SignatureDatabase database;
};

FileSignatures& FileSignatures::getInstance() {
    static FileSignatures instance;
    return instance;
}

FileSignatures::FileSignatures() : pImpl(std::make_unique<Impl>()) {
    initializeSignatures();
}

void FileSignatures::initializeSignatures() {
    // Essayer de charger depuis fichier externe d'abord
    if (loadFromExternalFile("signatures.txt")) {
        Logger::getInstance().log(LogLevel::INFO, "Loaded signatures from external file");
    } else {
        // Fallback: signatures codées en dur
        Logger::getInstance().log(LogLevel::WARNING, "Could not load external signatures, using built-in ones");
        addCommonSignatures();
        addImageSignatures();
        addVideoSignatures();
        addAudioSignatures();
        addDocumentSignatures();
        addArchiveSignatures();
        addGameSignatures();  // Nouvelle section pour les jeux !
    }
    
    Logger::getInstance().log(LogLevel::INFO, "Initialized " + std::to_string(getSignatureCount()) + " file signatures");
}

bool FileSignatures::loadFromExternalFile(const std::string& filename) {
    // Chercher dans plusieurs emplacements
    std::vector<std::string> searchPaths = {
        filename,                           // Chemin relatif
        "./" + filename,                    // Dossier courant
        "../" + filename,                   // Dossier parent
        "config/" + filename,               // Sous-dossier config
    };
    
    SignatureLoader loader;
    
    for (const auto& path : searchPaths) {
        std::error_code ec;
        if (std::filesystem::exists(path, ec)) {
            if (loader.loadFromFile(path)) {
                auto signatures = loader.getSignatures();
                for (const auto& sig : signatures) {
                    pImpl->database.addSignature(sig);
                }
                Logger::getInstance().log(LogLevel::INFO, "Loaded " + std::to_string(signatures.size()) + 
                                         " signatures from: " + path);
                return true;
            } else {
                Logger::getInstance().log(LogLevel::WARNING, "Failed to parse: " + path + " - " + loader.getLastError());
            }
        }
    }
    
    return false;
}

std::optional<FileSignature> FileSignatures::detectFileType(const ByteArray& data) const {
    return pImpl->database.detectSignature(data);
}

std::optional<FileSignature> FileSignatures::detectFileTypeByExtension(const std::string& extension) const {
    return pImpl->database.getSignatureByExtension(extension);
}

bool FileSignatures::validateSignature(const ByteArray& data, const FileSignature& signature) const {
    if (data.size() < signature.header.size()) {
        return false;
    }
    
    return std::equal(signature.header.begin(), signature.header.end(), 
                     data.begin() + signature.headerOffset);
}

bool FileSignatures::hasValidHeader(const ByteArray& data, const std::string& extension) const {
    auto sig = detectFileTypeByExtension(extension);
    if (!sig) return false;
    
    return validateSignature(data, *sig);
}

bool FileSignatures::hasValidFooter(const ByteArray& data, const std::string& extension) const {
    auto sig = detectFileTypeByExtension(extension);
    if (!sig || sig->footer.empty()) return false;
    
    if (data.size() < sig->footer.size()) return false;
    
    size_t footerStart = data.size() - sig->footer.size();
    return std::equal(sig->footer.begin(), sig->footer.end(), data.begin() + footerStart);
}

std::vector<Offset> FileSignatures::findSignatures(const ByteArray& data, 
                                                   const std::string& extension) const {
    return pImpl->database.findSignaturesInData(data, extension);
}

void FileSignatures::addSignature(const FileSignature& signature) {
    pImpl->database.addSignature(signature);
}

std::vector<FileSignature> FileSignatures::getAllSignatures() const {
    return pImpl->database.getAllSignatures();
}

std::vector<std::string> FileSignatures::getSupportedExtensions() const {
    return pImpl->database.getSupportedExtensions();
}

size_t FileSignatures::getSignatureCount() const {
    return pImpl->database.getCount();
}

void FileSignatures::addCommonSignatures() {
    // Signatures communes
}

void FileSignatures::addImageSignatures() {
    // JPEG
    FileSignature jpeg;
    jpeg.extension = "jpg";
    jpeg.mimeType = "image/jpeg";
    jpeg.description = "JPEG Image";
    jpeg.header = {0xFF, 0xD8, 0xFF};
    jpeg.footer = {0xFF, 0xD9};
    addSignature(jpeg);
    
    // PNG
    FileSignature png;
    png.extension = "png";
    png.mimeType = "image/png";
    png.description = "PNG Image";
    png.header = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    png.footer = {0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82};
    addSignature(png);
    
    // GIF
    FileSignature gif;
    gif.extension = "gif";
    gif.mimeType = "image/gif";
    gif.description = "GIF Image";
    gif.header = {0x47, 0x49, 0x46, 0x38}; // GIF8
    addSignature(gif);
    
    // BMP
    FileSignature bmp;
    bmp.extension = "bmp";
    bmp.mimeType = "image/bmp";
    bmp.description = "Bitmap Image";
    bmp.header = {0x42, 0x4D}; // BM
    addSignature(bmp);
}

void FileSignatures::addVideoSignatures() {
    // MP4
    FileSignature mp4;
    mp4.extension = "mp4";
    mp4.mimeType = "video/mp4";
    mp4.description = "MP4 Video";
    mp4.header = {0x00, 0x00, 0x00, 0x18, 0x66, 0x74, 0x79, 0x70}; // ftyp
    mp4.headerOffset = 4;
    addSignature(mp4);
    
    // AVI
    FileSignature avi;
    avi.extension = "avi";
    avi.mimeType = "video/x-msvideo";
    avi.description = "AVI Video";
    avi.header = {0x52, 0x49, 0x46, 0x46}; // RIFF
    addSignature(avi);
}

void FileSignatures::addAudioSignatures() {
    // MP3
    FileSignature mp3;
    mp3.extension = "mp3";
    mp3.mimeType = "audio/mpeg";
    mp3.description = "MP3 Audio";
    mp3.header = {0xFF, 0xFB}; // ID3 ou frame header
    addSignature(mp3);
    
    // WAV
    FileSignature wav;
    wav.extension = "wav";
    wav.mimeType = "audio/wav";
    wav.description = "WAV Audio";
    wav.header = {0x52, 0x49, 0x46, 0x46}; // RIFF
    addSignature(wav);
}

void FileSignatures::addDocumentSignatures() {
    // PDF
    FileSignature pdf;
    pdf.extension = "pdf";
    pdf.mimeType = "application/pdf";
    pdf.description = "PDF Document";
    pdf.header = {0x25, 0x50, 0x44, 0x46}; // %PDF
    pdf.footer = {0x25, 0x25, 0x45, 0x4F, 0x46}; // %%EOF
    addSignature(pdf);
    
    // DOC (Office 97-2003)
    FileSignature doc;
    doc.extension = "doc";
    doc.mimeType = "application/msword";
    doc.description = "Microsoft Word Document";
    doc.header = {0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1};
    addSignature(doc);
    
    // DOCX (Office 2007+)
    FileSignature docx;
    docx.extension = "docx";
    docx.mimeType = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    docx.description = "Microsoft Word Document (XML)";
    docx.header = {0x50, 0x4B, 0x03, 0x04}; // PK (ZIP)
    addSignature(docx);
}

void FileSignatures::addArchiveSignatures() {
    // ZIP
    FileSignature zip;
    zip.extension = "zip";
    zip.mimeType = "application/zip";
    zip.description = "ZIP Archive";
    zip.header = {0x50, 0x4B, 0x03, 0x04}; // PK
    addSignature(zip);
    
    // RAR
    FileSignature rar;
    rar.extension = "rar";
    rar.mimeType = "application/x-rar-compressed";
    rar.description = "RAR Archive";
    rar.header = {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07}; // Rar!
    addSignature(rar);
    
    // 7Z
    FileSignature sevenZ;
    sevenZ.extension = "7z";
    sevenZ.mimeType = "application/x-7z-compressed";
    sevenZ.description = "7-Zip Archive";
    sevenZ.header = {0x37, 0x7A, 0xBC, 0xAF, 0x27, 0x1C}; // 7z
    addSignature(sevenZ);
}

void FileSignatures::addGameSignatures() {
    // GVAS (Unreal Engine Save File)
    FileSignature gvas;
    gvas.extension = "sav";
    gvas.mimeType = "application/x-unreal-savegame";
    gvas.description = "Unreal Engine Game Save (GVAS)";
    gvas.header = {0x47, 0x56, 0x41, 0x53}; // "GVAS" en ASCII
    addSignature(gvas);
    
    Logger::getInstance().log(LogLevel::INFO, "Added GVAS signature for Unreal Engine saves (Octopath Traveler 2, etc.)");
}

} // namespace FileRecovery
