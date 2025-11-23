#include "utils/DuplicateDetector.h"
#include "utils/Logger.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>

namespace FileRecovery {
namespace Utils {

// ============================================================================
// SHA256 IMPLEMENTATION (Simplified)
// ============================================================================

// SHA256 constants
static const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

#define ROTR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define EP1(x) (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SIG0(x) (ROTR(x, 7) ^ ROTR(x, 18) ^ ((x) >> 3))
#define SIG1(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ ((x) >> 10))

void DuplicateDetector::sha256(const uint8_t* data, size_t length, uint8_t* hash) {
    uint32_t h[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    size_t paddedLen = ((length + 8) / 64 + 1) * 64;
    std::vector<uint8_t> padded(paddedLen, 0);
    std::memcpy(padded.data(), data, length);

    padded[length] = 0x80;
    uint64_t bitLen = length * 8;
    for (int i = 0; i < 8; ++i) {
        padded[paddedLen - 1 - i] = (bitLen >> (i * 8)) & 0xFF;
    }

    for (size_t chunk = 0; chunk < paddedLen; chunk += 64) {
        uint32_t w[64];

        for (int i = 0; i < 16; ++i) {
            w[i] = (padded[chunk + i * 4] << 24) |
                   (padded[chunk + i * 4 + 1] << 16) |
                   (padded[chunk + i * 4 + 2] << 8) |
                   (padded[chunk + i * 4 + 3]);
        }

        for (int i = 16; i < 64; ++i) {
            w[i] = SIG1(w[i - 2]) + w[i - 7] + SIG0(w[i - 15]) + w[i - 16];
        }

        uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
        uint32_t e = h[4], f = h[5], g = h[6], h7 = h[7];

        for (int i = 0; i < 64; ++i) {
            uint32_t t1 = h7 + EP1(e) + CH(e, f, g) + K[i] + w[i];
            uint32_t t2 = EP0(a) + MAJ(a, b, c);
            h7 = g; g = f; f = e; e = d + t1;
            d = c; c = b; b = a; a = t1 + t2;
        }

        h[0] += a; h[1] += b; h[2] += c; h[3] += d;
        h[4] += e; h[5] += f; h[6] += g; h[7] += h7;
    }

    for (int i = 0; i < 8; ++i) {
        hash[i * 4] = (h[i] >> 24) & 0xFF;
        hash[i * 4 + 1] = (h[i] >> 16) & 0xFF;
        hash[i * 4 + 2] = (h[i] >> 8) & 0xFF;
        hash[i * 4 + 3] = h[i] & 0xFF;
    }
}

std::string DuplicateDetector::bytesToHex(const uint8_t* bytes, size_t length) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (size_t i = 0; i < length; ++i) {
        oss << std::setw(2) << static_cast<int>(bytes[i]);
    }
    return oss.str();
}

// ============================================================================
// DUPLICATE DETECTOR IMPLEMENTATION
// ============================================================================

DuplicateDetector::DuplicateDetector() {}
DuplicateDetector::~DuplicateDetector() {}

std::string DuplicateDetector::hashFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return "";
    }

    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    ByteArray data(fileSize);
    file.read(reinterpret_cast<char*>(data.data()), fileSize);
    file.close();

    return hashBuffer(data);
}

std::string DuplicateDetector::hashBuffer(const ByteArray& data) {
    uint8_t hash[32];
    sha256(data.data(), data.size(), hash);
    return bytesToHex(hash, 32);
}

void DuplicateDetector::addFile(const FileHash& fileHash) {
    size_t index = files_.size();
    files_.push_back(fileHash);
    hashToIndices_[fileHash.sha256].push_back(index);
}

std::vector<DuplicateGroup> DuplicateDetector::findDuplicates() {
    std::vector<DuplicateGroup> duplicateGroups;

    for (const auto& [hash, indices] : hashToIndices_) {
        if (indices.size() > 1) {
            // C'est un groupe de duplicatas
            DuplicateGroup group;
            group.hash = hash;
            group.fileSize = files_[indices[0]].fileSize;

            for (size_t idx : indices) {
                group.files.push_back(files_[idx]);
            }

            // Trouver le meilleur fichier du groupe
            group.bestQualityIndex = selectBestFile(group.files);

            duplicateGroups.push_back(group);
        }
    }

    return duplicateGroups;
}

int DuplicateDetector::selectBestFile(const std::vector<FileHash>& group) {
    int bestIdx = 0;
    float bestScore = -1.0f;

    for (size_t i = 0; i < group.size(); ++i) {
        const FileHash& file = group[i];

        // Score basé sur : qualité + validité header/footer + entropie
        float score = file.quality;

        if (file.hasValidHeader) score += 0.3f;
        if (file.hasValidFooter) score += 0.2f;

        // Bonus pour entropie normale (entre 4.0 et 7.5)
        if (file.entropy >= 4.0f && file.entropy <= 7.5f) {
            score += 0.2f;
        }

        if (score > bestScore) {
            bestScore = score;
            bestIdx = static_cast<int>(i);
        }
    }

    return bestIdx;
}

void DuplicateDetector::getStatistics(size_t& totalFiles, size_t& uniqueFiles,
                                      size_t& duplicateFiles, size_t& wastedSpace) {
    totalFiles = files_.size();
    uniqueFiles = hashToIndices_.size();
    duplicateFiles = 0;
    wastedSpace = 0;

    for (const auto& [hash, indices] : hashToIndices_) {
        if (indices.size() > 1) {
            duplicateFiles += indices.size() - 1; // -1 car on garde le meilleur
            wastedSpace += files_[indices[0]].fileSize * (indices.size() - 1);
        }
    }
}

std::vector<std::string> DuplicateDetector::getFilesToDelete() {
    std::vector<std::string> toDelete;
    auto duplicates = findDuplicates();

    for (const auto& group : duplicates) {
        for (size_t i = 0; i < group.files.size(); ++i) {
            if (static_cast<int>(i) != group.bestQualityIndex) {
                toDelete.push_back(group.files[i].filePath);
            }
        }
    }

    return toDelete;
}

void DuplicateDetector::clear() {
    files_.clear();
    hashToIndices_.clear();
}

} // namespace Utils
} // namespace FileRecovery
