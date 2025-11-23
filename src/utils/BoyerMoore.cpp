#include "utils/BoyerMoore.h"
#include <algorithm>
#include <limits>

namespace FileRecovery {
namespace Utils {

BoyerMoore::BoyerMoore(const std::vector<uint8_t>& pattern)
    : pattern_(pattern),
      badCharTable_(256, -1),
      goodSuffixTable_(pattern.size() + 1, 0) {

    if (pattern_.empty()) {
        return;
    }

    buildBadCharTable();
    buildGoodSuffixTable();
}

void BoyerMoore::buildBadCharTable() {
    // Initialize: all characters have shift of -1
    std::fill(badCharTable_.begin(), badCharTable_.end(), -1);

    // For each character in pattern, record its rightmost position
    for (size_t i = 0; i < pattern_.size(); ++i) {
        badCharTable_[pattern_[i]] = static_cast<int>(i);
    }
}

void BoyerMoore::buildGoodSuffixTable() {
    size_t m = pattern_.size();
    std::vector<int> suffix(m + 1);

    // Phase 1: Compute suffix array
    suffix[m] = m + 1;
    int g = m + 1;
    for (int i = m - 1; i >= 0; --i) {
        if (i > g && suffix[i + m - 1 - static_cast<size_t>(g)] < i - g) {
            suffix[i] = suffix[i + m - 1 - static_cast<size_t>(g)];
        } else {
            if (i < g) {
                g = i;
            }
            int s = i;
            while (g >= 0 && pattern_[g] == pattern_[g + m - 1 - s]) {
                --g;
            }
            suffix[i] = s - g;
        }
    }

    // Phase 2: Compute good suffix table
    std::fill(goodSuffixTable_.begin(), goodSuffixTable_.end(), static_cast<int>(m));

    int j = 0;
    for (int i = m - 1; i >= -1; --i) {
        if (i == -1 || suffix[i] == i + 1) {
            for (; j < m - 1 - i; ++j) {
                if (goodSuffixTable_[j] == static_cast<int>(m)) {
                    goodSuffixTable_[j] = m - 1 - i;
                }
            }
        }
    }

    for (size_t i = 0; i <= m - 2; ++i) {
        goodSuffixTable_[m - 1 - suffix[i]] = m - 1 - static_cast<int>(i);
    }
}

int64_t BoyerMoore::search(const uint8_t* data, size_t dataSize) const {
    if (pattern_.empty() || dataSize < pattern_.size()) {
        return -1;
    }

    size_t m = pattern_.size();
    size_t n = dataSize;
    size_t s = 0; // shift of the pattern relative to text

    while (s <= n - m) {
        int j = static_cast<int>(m) - 1;

        // Compare pattern from right to left
        while (j >= 0 && pattern_[j] == data[s + j]) {
            --j;
        }

        if (j < 0) {
            // Pattern found at position s
            return static_cast<int64_t>(s);
        } else {
            // Calculate shift using both bad character and good suffix rules
            int badCharShift = j - badCharTable_[data[s + j]];
            int goodSuffixShift = goodSuffixTable_[j];

            // Use the maximum shift
            s += std::max(badCharShift, goodSuffixShift);
        }
    }

    return -1; // Pattern not found
}

std::vector<size_t> BoyerMoore::searchAll(const uint8_t* data, size_t dataSize) const {
    std::vector<size_t> results;

    if (pattern_.empty() || dataSize < pattern_.size()) {
        return results;
    }

    size_t m = pattern_.size();
    size_t n = dataSize;
    size_t s = 0;

    while (s <= n - m) {
        int j = static_cast<int>(m) - 1;

        while (j >= 0 && pattern_[j] == data[s + j]) {
            --j;
        }

        if (j < 0) {
            // Pattern found
            results.push_back(s);
            s += goodSuffixTable_[0];
        } else {
            int badCharShift = j - badCharTable_[data[s + j]];
            int goodSuffixShift = goodSuffixTable_[j];
            s += std::max(badCharShift, goodSuffixShift);
        }
    }

    return results;
}

bool BoyerMoore::contains(const uint8_t* data, size_t dataSize) const {
    return search(data, dataSize) != -1;
}

// Utility function for quick search without creating an object
int64_t boyerMooreSearch(const std::vector<uint8_t>& pattern,
                         const uint8_t* data,
                         size_t dataSize) {
    BoyerMoore bm(pattern);
    return bm.search(data, dataSize);
}

} // namespace Utils
} // namespace FileRecovery
