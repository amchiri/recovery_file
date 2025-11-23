#include "SignatureDatabase.h"
#include "utils/BoyerMoore.h"
#include <algorithm>

namespace FileRecovery {

void SignatureDatabase::addSignature(const FileSignature& signature) {
    m_signatures[signature.extension] = signature;
}

std::optional<FileSignature> SignatureDatabase::detectSignature(const ByteArray& data) const {
    // Chercher quelle signature correspond aux données
    for (const auto& [ext, sig] : m_signatures) {
        if (data.size() >= sig.header.size() + sig.headerOffset) {
            bool match = std::equal(sig.header.begin(), sig.header.end(),
                                   data.begin() + sig.headerOffset);
            if (match) {
                return sig;
            }
        }
    }
    return std::nullopt;
}

std::optional<FileSignature> SignatureDatabase::getSignatureByExtension(const std::string& extension) const {
    auto it = m_signatures.find(extension);
    if (it != m_signatures.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<Offset> SignatureDatabase::findSignaturesInData(const ByteArray& data,
                                                           const std::string& extension) const {
    std::vector<Offset> offsets;

    auto sigOpt = getSignatureByExtension(extension);
    if (!sigOpt) return offsets;

    const FileSignature& sig = *sigOpt;

    // Use Boyer-Moore algorithm for optimized pattern matching
    // Much faster than naive search for patterns >= 4 bytes
    Utils::BoyerMoore bm(sig.header);
    std::vector<size_t> matches = bm.searchAll(data.data(), data.size());

    // Convert to Offset type
    for (size_t pos : matches) {
        offsets.push_back(static_cast<Offset>(pos));
    }

    return offsets;
}

std::vector<FileSignature> SignatureDatabase::getAllSignatures() const {
    std::vector<FileSignature> signatures;
    for (const auto& [ext, sig] : m_signatures) {
        signatures.push_back(sig);
    }
    return signatures;
}

std::vector<std::string> SignatureDatabase::getSupportedExtensions() const {
    std::vector<std::string> extensions;
    for (const auto& [ext, sig] : m_signatures) {
        extensions.push_back(ext);
    }
    return extensions;
}

} // namespace FileRecovery
