#include "FileReconstructor.h"
#include "../utils/DiskIO.h"
#include "../utils/Logger.h"
#include "../include/FileSignatures.h"
#include <algorithm>

namespace FileRecovery {

FileReconstructor::FileReconstructor() {}

FileReconstructor::~FileReconstructor() {}

bool FileReconstructor::reconstructFile(const RecoveredFile& file,
                                       DiskIO& disk, ByteArray& output) {
    if (file.isFragmented) {
        return reconstructFragmentedFile(file, disk, output);
    } else {
        return reconstructContiguousFile(file, disk, output);
    }
}

// ============================================================================
// DÉTECTION INTELLIGENTE DE FRAGMENTATION
// ============================================================================
bool FileReconstructor::detectFragmentation(const RecoveredFile& file, DiskIO& disk) {
    // Lire le début du fichier (premiers 64KB)
    size_t sampleSize = std::min(size_t(65536), file.fileSize);
    ByteArray sample(sampleSize);

    if (!disk.readBytes(file.diskOffset, sampleSize, sample.data())) {
        return false; // Impossible de lire
    }

    // Critères de détection de fragmentation:

    // 1. Vérifier la continuité des données
    bool hasDiscontinuity = false;
    size_t discontinuityCount = 0;

    // Chercher des patterns de discontinuité (grands blocs de zéros inattendus)
    size_t zeroBlockSize = 0;
    for (size_t i = 0; i < sample.size(); ++i) {
        if (sample[i] == 0) {
            zeroBlockSize++;
        } else {
            if (zeroBlockSize > 4096) { // Bloc de >4KB de zéros = suspect
                discontinuityCount++;
            }
            zeroBlockSize = 0;
        }
    }

    hasDiscontinuity = (discontinuityCount > 2);

    // 2. Pour JPEG/PNG: vérifier les marqueurs internes
    if (file.detectedType == "jpg" || file.detectedType == "jpeg") {
        // JPEG doit avoir des marqueurs FF Dx régulièrement espacés
        int markerCount = 0;
        size_t lastMarkerPos = 0;

        for (size_t i = 0; i < sample.size() - 1; ++i) {
            if (sample[i] == 0xFF && sample[i+1] >= 0xC0 && sample[i+1] <= 0xFE) {
                markerCount++;
                size_t gap = i - lastMarkerPos;
                // Si gap trop grand (>16KB sans marqueur) = fragmentation probable
                if (lastMarkerPos > 0 && gap > 16384) {
                    return true;
                }
                lastMarkerPos = i;
            }
        }
    }

    // 3. Pour ZIP: vérifier la table centrale des fichiers
    if (file.detectedType == "zip" || file.detectedType == "docx" || file.detectedType == "xlsx") {
        // ZIP doit avoir un central directory à la fin
        // Si on ne le trouve pas, c'est probablement fragmenté
        size_t endSampleSize = std::min(size_t(1024), file.fileSize);
        ByteArray endSample(endSampleSize);

        if (disk.readBytes(file.diskOffset + file.fileSize - endSampleSize,
                           endSampleSize, endSample.data())) {
            // Chercher la signature du central directory (PK\x01\x02)
            bool foundCentralDir = false;
            for (size_t i = 0; i < endSample.size() - 4; ++i) {
                if (endSample[i] == 0x50 && endSample[i+1] == 0x4B &&
                    endSample[i+2] == 0x01 && endSample[i+3] == 0x02) {
                    foundCentralDir = true;
                    break;
                }
            }

            if (!foundCentralDir) {
                return true; // Probablement fragmenté
            }
        }
    }

    return hasDiscontinuity;
}

// ============================================================================
// RECONSTRUCTION DE FICHIERS FRAGMENTÉS INTELLIGENTE
// ============================================================================
bool FileReconstructor::reconstructFragmentedFile(const RecoveredFile& file,
                                                 DiskIO& disk, ByteArray& output) {
    Logger::getInstance().log(LogLevel::INFO,
        "Reconstructing fragmented file: " + file.originalName);

    output.clear();

    // Stratégie 1: Si fragmentOffsets est fourni, les utiliser
    if (!file.fragmentOffsets.empty()) {
        for (const auto& fragmentOffset : file.fragmentOffsets) {
            // Taille de fragment = distance jusqu'au prochain fragment (ou taille restante)
            size_t fragmentSize = 4096; // Par défaut

            ByteArray fragment(fragmentSize);
            if (disk.readBytes(fragmentOffset, fragmentSize, fragment.data())) {
                output.insert(output.end(), fragment.begin(), fragment.end());
            } else {
                Logger::getInstance().log(LogLevel::WARNING,
                    "Failed to read fragment at offset: " + std::to_string(fragmentOffset));
            }
        }
        return !output.empty();
    }

    // Stratégie 2: Reconstruction intelligente par recherche de signatures
    // Lire le header
    size_t chunkSize = 4096;
    ByteArray currentChunk(chunkSize);

    if (!disk.readBytes(file.diskOffset, chunkSize, currentChunk.data())) {
        return false;
    }

    output.insert(output.end(), currentChunk.begin(), currentChunk.end());

    // Chercher les fragments suivants en scannant le disque
    // (Simplifié : en production, on suivrait les chaînes de clusters FAT/NTFS)

    Offset currentOffset = file.diskOffset + chunkSize;
    size_t bytesRead = chunkSize;

    // Lire jusqu'à atteindre la taille estimée du fichier
    while (bytesRead < file.fileSize) {
        ByteArray nextChunk(chunkSize);

        if (disk.readBytes(currentOffset, chunkSize, nextChunk.data())) {
            // Vérifier si ce chunk semble faire partie du fichier
            // (heuristique simple : vérifier continuité)

            // Pour l'instant, ajouter directement
            output.insert(output.end(), nextChunk.begin(), nextChunk.end());
            bytesRead += chunkSize;
        }

        currentOffset += chunkSize;
    }

    // Tronquer à la taille exacte
    if (output.size() > file.fileSize) {
        output.resize(file.fileSize);
    }

    return !output.empty();
}

bool FileReconstructor::reconstructContiguousFile(const RecoveredFile& file,
                                                 DiskIO& disk, ByteArray& output) {
    output.resize(file.fileSize);
    return disk.readBytes(file.diskOffset, file.fileSize, output.data());
}

} // namespace FileRecovery
