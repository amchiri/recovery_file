#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>

namespace FileRecovery {
namespace Utils {

/**
 * @brief Implémentation de l'algorithme Boyer-Moore pour recherche de pattern optimisée
 *
 * L'algorithme Boyer-Moore est beaucoup plus rapide que la recherche naïve
 * pour les patterns de taille moyenne à grande (>3 bytes).
 * Complexité: O(n/m) dans le meilleur cas, O(n*m) dans le pire cas (rare)
 * où n = taille du texte, m = taille du pattern
 *
 * Avantages:
 * - Skip des caractères: peut sauter plusieurs caractères d'un coup
 * - Optimisé pour les recherches de signatures de fichiers (magic bytes)
 * - Meilleur que la recherche naïve pour patterns >= 4 bytes
 */
class BoyerMoore {
public:
    /**
     * @brief Constructeur avec le pattern à rechercher
     * @param pattern Séquence de bytes à rechercher (magic bytes)
     */
    explicit BoyerMoore(const std::vector<uint8_t>& pattern);

    /**
     * @brief Recherche le pattern dans un buffer de données
     * @param data Buffer où rechercher
     * @param dataSize Taille du buffer
     * @return Position de la première occurrence, ou -1 si non trouvé
     */
    int64_t search(const uint8_t* data, size_t dataSize) const;

    /**
     * @brief Recherche toutes les occurrences du pattern
     * @param data Buffer où rechercher
     * @param dataSize Taille du buffer
     * @return Vector contenant toutes les positions trouvées
     */
    std::vector<size_t> searchAll(const uint8_t* data, size_t dataSize) const;

    /**
     * @brief Vérifie si le pattern existe dans le buffer
     * @param data Buffer où rechercher
     * @param dataSize Taille du buffer
     * @return true si le pattern est trouvé
     */
    bool contains(const uint8_t* data, size_t dataSize) const;

private:
    void buildBadCharTable();
    void buildGoodSuffixTable();

    std::vector<uint8_t> pattern_;
    std::vector<int> badCharTable_;      // Table des mauvais caractères
    std::vector<int> goodSuffixTable_;   // Table des bons suffixes
};

/**
 * @brief Fonction utilitaire pour recherche rapide sans créer d'objet
 * @param pattern Pattern à rechercher
 * @param data Buffer de données
 * @param dataSize Taille du buffer
 * @return Position de la première occurrence, ou -1 si non trouvé
 */
int64_t boyerMooreSearch(const std::vector<uint8_t>& pattern,
                         const uint8_t* data,
                         size_t dataSize);

} // namespace Utils
} // namespace FileRecovery
