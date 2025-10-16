#ifndef SIGNATURE_DATABASE_H
#define SIGNATURE_DATABASE_H

#include "../include/RecoveryTypes.h"
#include <map>
#include <optional>

namespace FileRecovery {

class SignatureDatabase {
public:
    void addSignature(const FileSignature& signature);
    std::optional<FileSignature> detectSignature(const ByteArray& data) const;
    std::optional<FileSignature> getSignatureByExtension(const std::string& extension) const;
    std::vector<Offset> findSignaturesInData(const ByteArray& data, 
                                            const std::string& extension) const;
    
    std::vector<FileSignature> getAllSignatures() const;
    std::vector<std::string> getSupportedExtensions() const;
    size_t getCount() const { return m_signatures.size(); }

private:
    std::map<std::string, FileSignature> m_signatures;
};

} // namespace FileRecovery

#endif // SIGNATURE_DATABASE_H
