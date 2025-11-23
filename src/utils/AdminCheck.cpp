#include "utils/AdminCheck.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#pragma comment(lib, "shell32.lib")
#endif

namespace FileRecovery {
namespace Utils {

bool isRunningAsAdmin() {
#ifdef _WIN32
    BOOL isAdmin = FALSE;
    PSID administratorsGroup = NULL;

    // Allocate and initialize a SID of the administrators group
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(
            &ntAuthority,
            2,
            SECURITY_BUILTIN_DOMAIN_RID,
            DOMAIN_ALIAS_RID_ADMINS,
            0, 0, 0, 0, 0, 0,
            &administratorsGroup)) {

        // Check whether the token is a member of the administrators group
        if (!CheckTokenMembership(NULL, administratorsGroup, &isAdmin)) {
            isAdmin = FALSE;
        }

        FreeSid(administratorsGroup);
    }

    return isAdmin == TRUE;
#else
    // Sur Linux/Unix, vérifier si l'utilisateur est root (UID 0)
    return geteuid() == 0;
#endif
}

void printAdminWarning(const std::string& programName) {
    std::cerr << "\n";
    std::cerr << "╔═══════════════════════════════════════════════════════════════════╗\n";
    std::cerr << "║                    ⚠️  PRIVILÈGES REQUIS  ⚠️                       ║\n";
    std::cerr << "╠═══════════════════════════════════════════════════════════════════╣\n";
    std::cerr << "║                                                                   ║\n";
    std::cerr << "║  Ce programme nécessite des privilèges d'administrateur pour     ║\n";
    std::cerr << "║  fonctionner correctement.                                        ║\n";
    std::cerr << "║                                                                   ║\n";
    std::cerr << "║  Pourquoi ? L'accès direct aux disques et partitions requiert    ║\n";
    std::cerr << "║  des permissions élevées pour :                                   ║\n";
    std::cerr << "║    • Lire les secteurs bruts des disques                         ║\n";
    std::cerr << "║    • Accéder aux tables MFT (NTFS) et FAT                        ║\n";
    std::cerr << "║    • Scanner les fichiers supprimés                              ║\n";
    std::cerr << "║                                                                   ║\n";
    std::cerr << "║  Comment relancer en mode administrateur :                       ║\n";
#ifdef _WIN32
    std::cerr << "║                                                                   ║\n";
    std::cerr << "║  Windows :                                                        ║\n";
    std::cerr << "║    1. Clic-droit sur " << programName << ".exe                   ║\n";
    std::cerr << "║    2. Sélectionnez \"Exécuter en tant qu'administrateur\"          ║\n";
    std::cerr << "║                                                                   ║\n";
    std::cerr << "║  OU utilisez le script batch fourni :                            ║\n";
    std::cerr << "║    > launch_gui_admin.bat                                        ║\n";
#else
    std::cerr << "║                                                                   ║\n";
    std::cerr << "║  Linux/Unix :                                                     ║\n";
    std::cerr << "║    sudo ./" << programName << "                                  ║\n";
#endif
    std::cerr << "║                                                                   ║\n";
    std::cerr << "╚═══════════════════════════════════════════════════════════════════╝\n";
    std::cerr << "\n";
}

bool checkAdminPrivileges(const std::string& programName) {
    if (!isRunningAsAdmin()) {
        printAdminWarning(programName);
        return false;
    }

    // Message de confirmation si on est admin
    std::cout << "✓ Privilèges administrateur détectés\n" << std::endl;
    return true;
}

} // namespace Utils
} // namespace FileRecovery
