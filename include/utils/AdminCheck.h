#pragma once

#include <string>

namespace FileRecovery {
namespace Utils {

/**
 * @brief Vérifie si le programme s'exécute avec les privilèges d'administrateur
 *
 * Cette vérification est nécessaire pour l'accès direct aux disques et partitions.
 * Sans privilèges administrateur, le programme ne peut pas fonctionner correctement.
 *
 * @return true si le programme a les privilèges administrateur, false sinon
 */
bool isRunningAsAdmin();

/**
 * @brief Vérifie les privilèges et affiche un message si nécessaire
 *
 * Cette fonction vérifie si le programme s'exécute en tant qu'administrateur.
 * Si ce n'est pas le cas, elle affiche un message d'erreur explicite et retourne false.
 *
 * @param programName Nom du programme (pour affichage dans les messages)
 * @return true si admin, false sinon
 */
bool checkAdminPrivileges(const std::string& programName = "file_recovery");

/**
 * @brief Affiche un message d'avertissement sur les privilèges requis
 */
void printAdminWarning(const std::string& programName = "file_recovery");

} // namespace Utils
} // namespace FileRecovery
