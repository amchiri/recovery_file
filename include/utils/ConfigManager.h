#pragma once

#include <string>
#include <map>
#include <optional>

namespace FileRecovery {
namespace Utils {

/**
 * @brief Gestionnaire de configuration pour le File Recovery Tool
 *
 * Lit et écrit un fichier config.ini avec des paires clé=valeur.
 * Supporte les sections [Section] et les commentaires #
 */
class ConfigManager {
public:
    static ConfigManager& getInstance();

    /**
     * @brief Charge la configuration depuis un fichier
     * @param filename Chemin du fichier config.ini (par défaut: "config.ini")
     * @return true si chargé avec succès, false sinon
     */
    bool load(const std::string& filename = "config.ini");

    /**
     * @brief Sauvegarde la configuration dans un fichier
     * @param filename Chemin du fichier config.ini
     * @return true si sauvegardé avec succès, false sinon
     */
    bool save(const std::string& filename = "config.ini") const;

    /**
     * @brief Récupère une valeur string
     * @param section Section dans le fichier INI
     * @param key Clé à rechercher
     * @param defaultValue Valeur par défaut si non trouvée
     * @return Valeur trouvée ou valeur par défaut
     */
    std::string getString(const std::string& section, const std::string& key,
                          const std::string& defaultValue = "") const;

    /**
     * @brief Récupère une valeur entière
     */
    int getInt(const std::string& section, const std::string& key, int defaultValue = 0) const;

    /**
     * @brief Récupère une valeur booléenne
     */
    bool getBool(const std::string& section, const std::string& key, bool defaultValue = false) const;

    /**
     * @brief Définit une valeur string
     */
    void setString(const std::string& section, const std::string& key, const std::string& value);

    /**
     * @brief Définit une valeur entière
     */
    void setInt(const std::string& section, const std::string& key, int value);

    /**
     * @brief Définit une valeur booléenne
     */
    void setBool(const std::string& section, const std::string& key, bool value);

    /**
     * @brief Crée un fichier de configuration par défaut
     */
    static void createDefaultConfig(const std::string& filename = "config.ini");

private:
    ConfigManager() = default;
    ~ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    std::string makeKey(const std::string& section, const std::string& key) const;
    std::map<std::string, std::string> config_;
    std::string configFile_;
};

} // namespace Utils
} // namespace FileRecovery
