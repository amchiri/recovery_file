#ifndef RECOVERY_MANAGER_H
#define RECOVERY_MANAGER_H

#include "../include/RecoveryTypes.h"
#include <string>
#include <vector>

namespace FileRecovery {

class DiskIO;

/**
 * Gestionnaire de récupération de fichiers
 * Gère l'extraction et la sauvegarde des fichiers trouvés
 */
class RecoveryManager {
public:
    RecoveryManager();
    ~RecoveryManager();

    // Récupération de fichiers
    bool recoverFile(const RecoveredFile& file, const std::string& outputPath, DiskIO& disk);
    bool recoverMultipleFiles(const std::vector<RecoveredFile>& files, 
                             const std::string& outputDir, DiskIO& disk);

    // Configuration
    void setVerifyIntegrity(bool verify);
    void setOverwriteExisting(bool overwrite);

private:
    bool createOutputDirectory(const std::string& path);
    std::string generateUniqueFilename(const std::string& basePath, 
                                      const std::string& filename);
    bool verifyFileIntegrity(const RecoveredFile& file, const std::string& recoveredPath);

    bool m_verifyIntegrity = true;
    bool m_overwriteExisting = false;
};

} // namespace FileRecovery

#endif // RECOVERY_MANAGER_H
