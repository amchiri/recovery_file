#ifndef FILE_SIGNATURES_H
#define FILE_SIGNATURES_H

#include "RecoveryTypes.h"
#include <vector>
#include <string>
#include <optional>

namespace FileRecovery {

/**
 * Base de données de signatures de fichiers
 * Permet de détecter le type de fichier par analyse des octets
 */
class FileSignatures {
public:
    static FileSignatures& getInstance();

    // Détection de type
    std::optional<FileSignature> detectFileType(const ByteArray& data) const;
    std::optional<FileSignature> detectFileTypeByExtension(const std::string& extension) const;
    
    // Validation
    bool validateSignature(const ByteArray& data, const FileSignature& signature) const;
    bool hasValidHeader(const ByteArray& data, const std::string& extension) const;
    bool hasValidFooter(const ByteArray& data, const std::string& extension) const;

    // Recherche de signatures dans les données
    std::vector<Offset> findSignatures(const ByteArray& data, const std::string& extension) const;
    
    // Gestion des signatures
    void addSignature(const FileSignature& signature);
    std::vector<FileSignature> getAllSignatures() const;
    std::vector<std::string> getSupportedExtensions() const;
    size_t getSignatureCount() const;

private:
    FileSignatures();
    ~FileSignatures() = default;
    
    FileSignatures(const FileSignatures&) = delete;
    FileSignatures& operator=(const FileSignatures&) = delete;

    void initializeSignatures();
    bool loadFromExternalFile(const std::string& filename);
    void addCommonSignatures();
    void addImageSignatures();
    void addVideoSignatures();
    void addAudioSignatures();
    void addDocumentSignatures();
    void addArchiveSignatures();
    void addGameSignatures();  // Signatures de jeux vidéo

    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace FileRecovery

#endif // FILE_SIGNATURES_H
