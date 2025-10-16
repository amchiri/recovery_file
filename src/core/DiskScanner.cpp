#include "DiskScanner.h"
#include "../scanner/QuickScanner.h"
#include "../scanner/DeepScanner.h"
#include "../scanner/SignatureScanner.h"

namespace FileRecovery {

std::unique_ptr<DiskScanner> ScannerFactory::createScanner(ScanMode mode) {
    switch (mode) {
        case ScanMode::QUICK:
            return std::make_unique<QuickScanner>();
        case ScanMode::DEEP:
            return std::make_unique<DeepScanner>();
        case ScanMode::SIGNATURE:
            return std::make_unique<SignatureScanner>();
        case ScanMode::COMPLETE:
            // Mode complet combine plusieurs scanners
            return std::make_unique<DeepScanner>(); // Pour l'instant
        default:
            return std::make_unique<QuickScanner>();
    }
}

} // namespace FileRecovery
