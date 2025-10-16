#include "SignatureLoader.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace FileRecovery {

SignatureLoader::SignatureLoader() {}

bool SignatureLoader::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        m_lastError = "Cannot open file: " + filename;
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    return loadFromString(content);
}

bool SignatureLoader::loadFromString(const std::string& content) {
    m_signatures.clear();
    m_lastError.clear();
    
    std::istringstream stream(content);
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(stream, line)) {
        lineNumber++;
        
        // Ignorer lignes vides et commentaires
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        try {
            FileSignature sig = parseSignatureLine(line);
            if (!sig.extension.empty()) {
                m_signatures.push_back(sig);
            }
        } catch (const std::exception& e) {
            m_lastError += "Line " + std::to_string(lineNumber) + ": " + e.what() + "\n";
        }
    }
    
    return m_lastError.empty();
}

std::vector<FileSignature> SignatureLoader::getSignatures() const {
    return m_signatures;
}

size_t SignatureLoader::getCount() const {
    return m_signatures.size();
}

std::string SignatureLoader::getLastError() const {
    return m_lastError;
}

FileSignature SignatureLoader::parseSignatureLine(const std::string& line) {
    FileSignature sig;
    
    // Format: extension|mime_type|description|header_hex|footer_hex|header_offset|max_size
    std::istringstream stream(line);
    std::string token;
    std::vector<std::string> parts;
    
    while (std::getline(stream, token, '|')) {
        parts.push_back(token);
    }
    
    if (parts.size() < 4) {
        throw std::runtime_error("Invalid format, expected at least 4 fields");
    }
    
    sig.extension = parts[0];
    sig.mimeType = parts[1];
    sig.description = parts[2];
    sig.header = parseHexString(parts[3]);
    
    if (parts.size() > 4 && !parts[4].empty()) {
        sig.footer = parseHexString(parts[4]);
    }
    
    if (parts.size() > 5 && !parts[5].empty()) {
        sig.headerOffset = std::stoull(parts[5]);
    }
    
    if (parts.size() > 6 && !parts[6].empty()) {
        sig.maxSize = std::stoull(parts[6]);
    }
    
    return sig;
}

ByteArray SignatureLoader::parseHexString(const std::string& hex) {
    ByteArray bytes;
    
    if (hex.empty()) {
        return bytes;
    }
    
    std::istringstream stream(hex);
    std::string byteStr;
    
    while (stream >> byteStr) {
        // Support pour wildcard (??)
        if (byteStr == "??") {
            // Pour l'instant, on skip les wildcards
            // Une vraie implémentation gérerait ça différemment
            continue;
        }
        
        // Convertir hex en byte
        unsigned int byte;
        std::istringstream converter(byteStr);
        converter >> std::hex >> byte;
        
        bytes.push_back(static_cast<Byte>(byte));
    }
    
    return bytes;
}

} // namespace FileRecovery
