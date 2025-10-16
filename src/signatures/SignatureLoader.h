#ifndef SIGNATURE_LOADER_H
#define SIGNATURE_LOADER_H

#include "../include/RecoveryTypes.h"
#include <string>
#include <vector>

namespace FileRecovery {

/**
 * Chargeur de signatures depuis fichier externe
 * Permet d'ajouter de nouvelles signatures sans recompiler
 */
class SignatureLoader {
public:
    SignatureLoader();
    
    // Charger signatures depuis un fichier
    bool loadFromFile(const std::string& filename);
    bool loadFromString(const std::string& content);
    
    // Obtenir les signatures chargées
    std::vector<FileSignature> getSignatures() const;
    
    // Statistiques
    size_t getCount() const;
    std::string getLastError() const;

private:
    std::vector<FileSignature> m_signatures;
    std::string m_lastError;
    
    FileSignature parseSignatureLine(const std::string& line);
    ByteArray parseHexString(const std::string& hex);
};

} // namespace FileRecovery

#endif // SIGNATURE_LOADER_H
