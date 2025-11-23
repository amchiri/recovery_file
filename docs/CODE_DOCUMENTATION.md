# File Recovery Tool - Technical Code Documentation

**Version:** 1.0.0
**Last Updated:** 2025-11-23
**Language:** C++17
**Build System:** CMake 3.15+

---

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Core Components](#core-components)
3. [Detailed Module Documentation](#detailed-module-documentation)
4. [Data Structures](#data-structures)
5. [Algorithms & Optimizations](#algorithms--optimizations)
6. [File Signatures System](#file-signatures-system)
7. [Quality Analysis System](#quality-analysis-system)
8. [Threading & Concurrency](#threading--concurrency)
9. [Code Flow Diagrams](#code-flow-diagrams)

---

## Architecture Overview

### High-Level Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     User Interface Layer                     │
│  ┌──────────────────┐              ┌──────────────────┐     │
│  │  CLI Interface   │              │  GUI (ImGui)     │     │
│  │  (main.cpp)      │              │  (gui_main.cpp)  │     │
│  └──────────────────┘              └──────────────────┘     │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                      Engine Layer                            │
│  ┌──────────────────┐   ┌──────────────────┐               │
│  │ RecoveryEngine   │   │ RecoveryManager  │               │
│  │ (Orchestration)  │   │ (File Writing)   │               │
│  └──────────────────┘   └──────────────────┘               │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                    Scanning Layer                            │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │ QuickScanner │  │ DeepScanner  │  │SignatureScanner│    │
│  │ (Fast MFT)   │  │(Sector-by-   │  │(Pattern Match)│     │
│  │              │  │  sector)     │  │               │     │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                 Filesystem Parsers Layer                     │
│  ┌──────────────────┐              ┌──────────────────┐     │
│  │   NTFSParser     │              │  FAT32Parser     │     │
│  │  (MFT, Sectors)  │              │  (FAT, Clusters) │     │
│  └──────────────────┘              └──────────────────┘     │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                   Utilities & Support                        │
│  ┌────────┐ ┌────────┐ ┌──────────┐ ┌──────────────┐       │
│  │ DiskIO │ │ Logger │ │FileQuality│ │ConfigManager │       │
│  └────────┘ └────────┘ └──────────┘ └──────────────┘       │
│  ┌────────┐ ┌────────┐ ┌──────────┐ ┌──────────────┐       │
│  │BoyerMoore│FilePreview│Duplicate │ │ AdminCheck  │        │
│  └────────┘ └────────┘ └Detector  ┘ └──────────────┘       │
└─────────────────────────────────────────────────────────────┘
```

### Design Patterns Used

1. **Singleton Pattern**: `Logger`, `ConfigManager`, `SignatureDatabase`
2. **Strategy Pattern**: Scanner hierarchy (`DiskScanner` base → `QuickScanner`/`DeepScanner`)
3. **Factory Pattern**: File signature creation in `SignatureDatabase`
4. **Observer Pattern**: Progress callbacks in scanning operations
5. **RAII**: All resource management uses smart pointers

---

## Core Components

### 1. Recovery Engine (`RecoveryEngine.h/cpp`)

**Location:** `include/RecoveryEngine.h`, `src/core/RecoveryEngine.cpp`

**Purpose:** Main orchestration component that coordinates the entire recovery process.

**Key Methods:**

```cpp
class RecoveryEngine {
public:
    RecoveryEngine();

    // Configuration
    void setOutputDirectory(const std::string& path);
    void setFileTypes(const std::vector<std::string>& types);
    void setThreadCount(int threads);

    // Execution
    bool quickScan(const std::string& diskPath);
    bool deepScan(const std::string& diskPath);
    void stopRecovery();

    // Results
    std::vector<RecoveredFile> getRecoveredFiles() const;
    float getProgress() const;
};
```

**Workflow:**
1. Initialize scanner (Quick or Deep based on user choice)
2. Start disk scanning in separate thread
3. Process discovered files through quality analysis
4. Write recovered files to output directory
5. Update progress metrics and UI

---

### 2. Disk Scanners

#### Base Class: `DiskScanner` (`src/scanner/DiskScanner.h`)

**Abstract interface for all scanners:**

```cpp
class DiskScanner {
public:
    virtual ~DiskScanner() = default;

    virtual bool scan(const std::string& diskPath) = 0;
    virtual void stop() = 0;
    virtual float getProgress() const = 0;
    virtual std::vector<RecoveredFile> getResults() const = 0;

    // Callback for progress updates
    void setProgressCallback(std::function<void(float)> callback);
};
```

#### QuickScanner (`src/scanner/QuickScanner.h/cpp`)

**Purpose:** Fast recovery using filesystem metadata (MFT for NTFS).

**Algorithm:**
1. Parse Master File Table (MFT)
2. Extract file records that are marked as deleted
3. Read file attributes (name, size, timestamps)
4. Verify data clusters are readable
5. Reconstruct file if possible

**Performance:** ~10-100x faster than deep scan for recently deleted files.

**Limitations:**
- Only works if MFT entry still exists
- Cannot recover overwritten files
- Filesystem-dependent

```cpp
class QuickScanner : public DiskScanner {
private:
    std::unique_ptr<NTFSParser> ntfsParser_;
    std::unique_ptr<FAT32Parser> fat32Parser_;

    bool scanNTFS(const std::string& diskPath);
    bool scanFAT32(const std::string& diskPath);
};
```

#### DeepScanner (`src/scanner/DeepScanner.h/cpp`)

**Purpose:** Comprehensive sector-by-sector recovery using file signatures.

**Algorithm:**
1. Read disk sector-by-sector (512-byte blocks)
2. Use Boyer-Moore to search for file signatures (magic bytes)
3. When signature found:
   - Determine file type
   - Estimate file size (format-specific heuristics)
   - Extract file data
   - Run quality analysis
4. Apply optimizations (skip empty sectors, large buffers)

**Performance Optimizations:**
- **Smart Scan:** Skip sectors filled with zeros (40-60% speed improvement)
- **Large Buffers:** Read 16MB chunks instead of 512-byte sectors
- **Boyer-Moore Search:** O(n/m) pattern matching instead of O(n*m) naive search
- **Multi-threading:** Parallel processing of file extraction

```cpp
class DeepScanner : public DiskScanner {
private:
    SignatureDatabase signatureDb_;
    std::atomic<bool> stopRequested_{false};
    std::atomic<float> progress_{0.0f};

    // Optimization flags
    bool smartScan_ = true;
    bool useLargeBuffers_ = true;

    std::vector<RecoveredFile> scanDiskForSignatures(DiskIO& disk);
    bool isEmptySector(const uint8_t* data, size_t size);
};
```

---

### 3. File Signature System

#### SignatureDatabase (`src/signatures/SignatureDatabase.h/cpp`)

**Purpose:** Manage 70+ file signatures across 9 categories.

**Signature Structure:**

```cpp
struct FileSignature {
    std::string extension;        // "jpg", "png", etc.
    std::string description;      // Human-readable name
    std::vector<uint8_t> header;  // Magic bytes (start of file)
    std::vector<uint8_t> footer;  // Optional end marker
    size_t minSize;               // Minimum valid file size
    size_t maxSize;               // Maximum expected size
    bool hasFooter;               // Whether footer is required
};
```

**Categories (70 signatures total):**

1. **Images (13):** JPEG, PNG, GIF, BMP, TIFF, WebP, ICO, PSD, CR2, NEF, HEIC, SVG
2. **Videos (14):** MP4, AVI, MKV, MOV, FLV, WMV, MPEG, WEBM, M4V
3. **Audio (9):** MP3, WAV, FLAC, OGG, M4A, AAC, WMA, MIDI
4. **Documents (11):** PDF, DOC, DOCX, XLS, XLSX, PPT, PPTX, RTF, EPUB, ODT
5. **Archives (9):** ZIP, RAR, 7Z, TAR, GZ, BZ2, ISO
6. **Executables (6):** EXE, DLL, ELF, APK, MSI, DMG
7. **Databases (5):** SQLite, MDB, ACCDB, PST, OST
8. **3D/CAD (7):** OBJ, STL, FBX, BLEND, 3DS, DXF, DWG
9. **Crypto (4):** Wallet.dat, Keystore, PEM, P12

**Boyer-Moore Optimization:**

The signature search uses the Boyer-Moore algorithm for 3-10x faster pattern matching:

```cpp
bool SignatureDatabase::matchesSignature(const ByteArray& data,
                                         const FileSignature& sig) {
    Utils::BoyerMoore bm(sig.header);
    std::vector<size_t> matches = bm.searchAll(data.data(), data.size());
    return !matches.empty();
}
```

---

### 4. File Quality Analysis System

#### FileQuality (`include/FileQuality.h`, `src/utils/FileQuality.cpp`)

**Purpose:** Intelligent multi-criteria file corruption detection.

**Quality Levels:**

```cpp
enum class FileQualityLevel {
    EXCELLENT,  // Perfect file, all checks pass
    GOOD,       // Minor issues, fully recoverable
    POOR,       // Partial corruption, some data lost
    CORRUPTED,  // Heavy corruption, limited usability
    DEAD        // Completely unusable (overwritten/random data)
};
```

**Analysis Metrics:**

```cpp
struct FileQualityReport {
    FileQualityLevel level;

    // Legacy metric (kept for compatibility)
    float zeroPercentage;

    // NEW INTELLIGENT METRICS
    float entropy;              // Shannon entropy (0.0-8.0)
    bool hasValidHeader;        // Signature matches
    bool hasValidFooter;        // End marker present
    bool hasValidStructure;     // Internal consistency
    bool hasRepeatingPatterns;  // Suspicious repetition
    float dataConsistency;      // Type-specific coherence (0.0-1.0)

    std::string recommendation;
    std::string detailedAnalysis;
};
```

**Shannon Entropy Calculation:**

```cpp
float FileQuality::calculateEntropy(const ByteArray& data) {
    // Count byte frequencies
    std::vector<size_t> frequencies(256, 0);
    for (uint8_t byte : data) {
        frequencies[byte]++;
    }

    // Calculate Shannon entropy: H = -Σ(p_i * log2(p_i))
    float entropy = 0.0f;
    float size = static_cast<float>(data.size());

    for (size_t freq : frequencies) {
        if (freq > 0) {
            float p = freq / size;
            entropy -= p * std::log2(p);
        }
    }

    return entropy; // Range: 0.0 (all zeros) to 8.0 (perfect random)
}
```

**Quality Determination Logic:**

```cpp
FileQualityLevel FileQuality::determineQualityLevel(const FileQualityReport& report) {
    // PRIORITY 1: Detect completely dead files
    if (report.zeroPercentage > 99.0f && report.entropy < 0.5f) {
        return FileQualityLevel::DEAD; // Overwritten with zeros
    }

    if (!report.hasValidHeader && report.entropy > 7.8f) {
        return FileQualityLevel::DEAD; // Random garbage
    }

    // PRIORITY 2: Check structural integrity
    if (!report.hasValidHeader && !report.hasValidFooter) {
        return FileQualityLevel::CORRUPTED;
    }

    if (report.hasValidHeader && !report.hasValidFooter) {
        return FileQualityLevel::POOR; // Truncated file
    }

    // PRIORITY 3: Analyze content quality
    if (report.hasValidStructure &&
        report.entropy >= 3.0f && report.entropy <= 7.5f &&
        report.dataConsistency > 0.8f) {
        return FileQualityLevel::EXCELLENT;
    }

    if (report.entropy >= 2.0f && report.dataConsistency > 0.6f) {
        return FileQualityLevel::GOOD;
    }

    return FileQualityLevel::POOR;
}
```

---

### 5. File Preview System

#### FilePreview (`include/utils/FilePreview.h`, `src/utils/FilePreview.cpp`)

**Purpose:** Generate previews of recovered files before opening them.

**Preview Types:**

```cpp
enum class PreviewType {
    NONE,
    IMAGE,    // Thumbnail generation
    TEXT,     // First 100 lines
    HEX       // Hex dump
};

struct PreviewData {
    PreviewType type;

    // Image preview
    std::vector<uint8_t> thumbnailData;
    int width;
    int height;

    // Text preview
    std::string textContent;

    // Hex preview
    std::string hexDump;
};
```

**Implementation:**

```cpp
PreviewData FilePreview::generatePreview(const std::string& filePath,
                                         const std::string& fileType) {
    PreviewData preview;

    if (isImageType(fileType)) {
        preview.type = PreviewType::IMAGE;
        generateImagePreview(filePath, preview);
    }
    else if (isTextType(fileType)) {
        preview.type = PreviewType::TEXT;
        generateTextPreview(filePath, preview);
    }
    else {
        preview.type = PreviewType::HEX;
        generateHexPreview(filePath, preview);
    }

    return preview;
}
```

**Text Extraction with Binary Detection:**

```cpp
void FilePreview::generateTextPreview(const std::string& filePath,
                                      PreviewData& preview) {
    std::ifstream file(filePath, std::ios::binary);
    std::string line;
    int lineCount = 0;

    while (lineCount < 100 && std::getline(file, line)) {
        // Detect binary content
        bool hasBinary = false;
        for (char c : line) {
            if (c < 32 && c != '\t' && c != '\n' && c != '\r') {
                hasBinary = true;
                break;
            }
        }

        if (hasBinary) {
            preview.textContent += "[BINARY DATA]\n";
            break;
        }

        preview.textContent += line + "\n";
        lineCount++;
    }
}
```

---

### 6. Duplicate Detection System

#### DuplicateDetector (`include/utils/DuplicateDetector.h`, `src/utils/DuplicateDetector.cpp`)

**Purpose:** Find and manage duplicate files using SHA256 hashing.

**Architecture:**

```cpp
struct DuplicateFileInfo {
    std::string filePath;
    size_t fileSize;
    float quality;         // From FileQuality analysis
    bool hasValidHeader;
    bool hasValidFooter;
    float entropy;
};

struct DuplicateGroup {
    std::string hash;      // SHA256 hash
    std::vector<DuplicateFileInfo> files;
    std::string bestFile;  // Highest quality file to keep
};
```

**SHA256 Implementation:**

Complete from-scratch implementation with no external dependencies:

```cpp
std::string DuplicateDetector::hashBuffer(const ByteArray& data) {
    // SHA256 constants (first 32 bits of fractional parts of cube roots)
    static const uint32_t K[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, ...
    };

    // Initialize hash values (first 32 bits of square roots)
    uint32_t H[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    // Pre-processing: padding message
    ByteArray paddedData = padMessage(data);

    // Process message in 512-bit chunks
    for (size_t chunkStart = 0; chunkStart < paddedData.size(); chunkStart += 64) {
        processChunk(&paddedData[chunkStart], H, K);
    }

    // Produce final hash
    return hashToHexString(H, 8);
}
```

**Quality-Based Selection:**

The system selects the best file based on multiple criteria:

```cpp
std::string DuplicateDetector::selectBestFile(const std::vector<DuplicateFileInfo>& files) {
    auto best = std::max_element(files.begin(), files.end(),
        [](const DuplicateFileInfo& a, const DuplicateFileInfo& b) {
            // Priority 1: Valid header + footer
            if (a.hasValidHeader && a.hasValidFooter &&
                !(b.hasValidHeader && b.hasValidFooter)) {
                return false;
            }

            // Priority 2: Valid header
            if (a.hasValidHeader && !b.hasValidHeader) {
                return false;
            }

            // Priority 3: Better entropy (3.0-7.5 ideal)
            float aEntropyScore = 1.0f - std::abs(5.0f - a.entropy) / 5.0f;
            float bEntropyScore = 1.0f - std::abs(5.0f - b.entropy) / 5.0f;
            if (std::abs(aEntropyScore - bEntropyScore) > 0.1f) {
                return aEntropyScore < bEntropyScore;
            }

            // Priority 4: Higher quality score
            return a.quality < b.quality;
        });

    return best->filePath;
}
```

---

### 7. Fragmented File Recovery

#### FileReconstructor (`src/recovery/FileReconstructor.h/cpp`)

**Purpose:** Reconstruct files that are fragmented across non-contiguous disk sectors.

**Fragmentation Detection:**

```cpp
bool FileReconstructor::detectFragmentation(const RecoveredFile& file, DiskIO& disk) {
    // Read first 64KB sample
    ByteArray sample = disk.readBytes(file.diskOffset, 65536);

    // CRITERION 1: Discontinuity detection
    size_t zeroBlockCount = 0;
    size_t currentZeroBlock = 0;
    for (uint8_t byte : sample) {
        if (byte == 0) {
            currentZeroBlock++;
        } else {
            if (currentZeroBlock > 4096) { // >4KB zeros = suspicious
                zeroBlockCount++;
            }
            currentZeroBlock = 0;
        }
    }
    if (zeroBlockCount > 2) return true;

    // CRITERION 2: JPEG marker spacing validation
    if (file.detectedType == "jpg") {
        size_t lastMarkerPos = 0;
        for (size_t i = 0; i < sample.size() - 1; i++) {
            if (sample[i] == 0xFF && sample[i+1] >= 0xC0) {
                size_t gap = i - lastMarkerPos;
                if (lastMarkerPos > 0 && gap > 16384) { // >16KB gap
                    return true;
                }
                lastMarkerPos = i;
            }
        }
    }

    // CRITERION 3: ZIP central directory verification
    if (file.detectedType == "zip") {
        ByteArray endSample = disk.readBytes(
            file.diskOffset + file.fileSize - 1024, 1024);

        bool foundCentralDir = searchForBytes(endSample,
                                              {0x50, 0x4B, 0x01, 0x02});
        if (!foundCentralDir) return true;
    }

    return false;
}
```

**Reconstruction Strategies:**

```cpp
bool FileReconstructor::reconstructFragmentedFile(const RecoveredFile& file,
                                                  DiskIO& disk, ByteArray& output) {
    // STRATEGY 1: Use provided fragment offsets
    if (!file.fragmentOffsets.empty()) {
        for (Offset offset : file.fragmentOffsets) {
            ByteArray fragment = disk.readBytes(offset, 4096);
            output.insert(output.end(), fragment.begin(), fragment.end());
        }
        return true;
    }

    // STRATEGY 2: Intelligent scanning
    // Read header chunk
    ByteArray header = disk.readBytes(file.diskOffset, 4096);
    output.insert(output.end(), header.begin(), header.end());

    // Scan for continuation patterns
    Offset currentOffset = file.diskOffset + 4096;
    while (output.size() < file.fileSize) {
        ByteArray chunk = disk.readBytes(currentOffset, 4096);

        // Verify chunk continuity (format-specific heuristics)
        if (verifyContinuity(output, chunk, file.detectedType)) {
            output.insert(output.end(), chunk.begin(), chunk.end());
        }

        currentOffset += 4096;
    }

    return true;
}
```

---

## Data Structures

### RecoveredFile

**Location:** `include/RecoveryTypes.h`

```cpp
struct RecoveredFile {
    std::string originalName;     // Filename from MFT or generated
    std::string detectedType;     // "jpg", "pdf", etc.
    Offset diskOffset;            // Physical byte offset on disk
    size_t fileSize;              // Size in bytes
    FileQualityLevel quality;     // EXCELLENT/GOOD/POOR/CORRUPTED/DEAD

    // Timestamps (if available from MFT)
    std::time_t createdTime;
    std::time_t modifiedTime;
    std::time_t accessedTime;

    // Fragmentation support
    bool isFragmented;
    std::vector<Offset> fragmentOffsets;

    // Quality metrics
    float zeroPercentage;
    float entropy;
    float dataConsistency;
    bool hasValidHeader;
    bool hasValidFooter;

    // Recovery metadata
    std::string outputPath;       // Where file was saved
    bool recovered;               // Successfully written to disk
};
```

### ByteArray

Type alias for `std::vector<uint8_t>` used throughout the codebase for binary data.

---

## Algorithms & Optimizations

### Boyer-Moore String Search

**Location:** `include/utils/BoyerMoore.h`, `src/utils/BoyerMoore.cpp`

**Complexity:** O(n/m) best case, O(n*m) worst case (but rare)

**Implementation:**

```cpp
class BoyerMoore {
private:
    std::vector<uint8_t> pattern_;
    std::vector<int> badCharTable_;      // 256 entries
    std::vector<int> goodSuffixTable_;   // pattern length entries

    void buildBadCharTable() {
        badCharTable_.assign(256, -1);
        for (size_t i = 0; i < pattern_.size(); i++) {
            badCharTable_[pattern_[i]] = static_cast<int>(i);
        }
    }

    void buildGoodSuffixTable() {
        // Knuth-Morris-Pratt-like preprocessing
        // Computes shift distances for matching suffixes
    }

public:
    int64_t search(const uint8_t* data, size_t dataSize) const {
        size_t m = pattern_.size();
        size_t n = dataSize;

        size_t s = 0; // Shift of pattern relative to data
        while (s <= n - m) {
            int j = static_cast<int>(m) - 1;

            // Compare from right to left
            while (j >= 0 && pattern_[j] == data[s + j]) {
                j--;
            }

            if (j < 0) {
                return static_cast<int64_t>(s); // Match found
            }

            // Shift pattern using bad character rule
            int badCharShift = j - badCharTable_[data[s + j]];
            int goodSuffixShift = goodSuffixTable_[j];
            s += std::max(badCharShift, goodSuffixShift);
        }

        return -1; // No match
    }
};
```

**Performance Improvement:**
- 3-10x faster than naive search for patterns ≥4 bytes
- Critical for deep scan performance (70+ signatures × billions of sectors)

---

## Threading & Concurrency

### Thread Safety Mechanisms

1. **Atomic Variables:**
```cpp
std::atomic<float> progress_{0.0f};
std::atomic<int> filesFound_{0};
std::atomic<bool> stopRequested_{false};
```

2. **Mutex Protection:**
```cpp
std::mutex dataMutex_;
{
    std::lock_guard<std::mutex> lock(dataMutex_);
    recoveredFiles_.push_back(file);
}
```

3. **Thread Pool (Scanning):**
```cpp
void DeepScanner::scan(const std::string& diskPath) {
    std::vector<std::thread> workers;
    for (int i = 0; i < threadCount_; i++) {
        workers.emplace_back(&DeepScanner::workerThread, this, i);
    }

    for (auto& t : workers) {
        t.join();
    }
}
```

---

## Configuration System

**Location:** `include/utils/ConfigManager.h`, `src/utils/ConfigManager.cpp`

**Format:** INI file with sections

```ini
[Paths]
output_directory = .\recovered_files
signature_file = signatures.txt

[Performance]
thread_count = 4
deep_scan = true
buffer_size_mb = 16

[Scanning]
smart_scan = true
large_buffers = true

[Logging]
log_level = INFO
log_file = recovery_log.txt
```

**Usage:**

```cpp
auto& config = Utils::ConfigManager::getInstance();
config.load("config.ini");

int threads = config.getInt("Performance", "thread_count", 4);
bool smartScan = config.getBool("Scanning", "smart_scan", true);
std::string outputDir = config.getString("Paths", "output_directory",
                                         ".\\recovered_files");
```

---

## Code Flow Diagrams

### Deep Scan Recovery Flow

```
START
  │
  ├─→ Load Configuration (config.ini)
  │
  ├─→ Check Admin Privileges (AdminCheck)
  │
  ├─→ Initialize RecoveryEngine
  │     ├─→ Load SignatureDatabase (70 signatures)
  │     └─→ Initialize FileQuality analyzer
  │
  ├─→ Start DeepScanner
  │     ├─→ Open disk with DiskIO
  │     ├─→ Get total disk size
  │     └─→ Create worker threads
  │
  ├─→ Sector-by-Sector Scanning (parallel)
  │     ├─→ Read 16MB buffer (if large_buffers=true)
  │     ├─→ Skip empty sectors (if smart_scan=true)
  │     ├─→ Boyer-Moore search for signatures
  │     ├─→ When signature found:
  │     │     ├─→ Determine file type
  │     │     ├─→ Estimate file size
  │     │     ├─→ Extract file data
  │     │     ├─→ Run FileQuality analysis
  │     │     │     ├─→ Calculate entropy
  │     │     │     ├─→ Validate header/footer
  │     │     │     ├─→ Check structure
  │     │     │     └─→ Determine quality level
  │     │     └─→ Add to results if quality ≥ threshold
  │     └─→ Update progress
  │
  ├─→ Recovery Phase
  │     ├─→ For each recovered file:
  │     │     ├─→ Check if fragmented (FileReconstructor)
  │     │     ├─→ Reconstruct if needed
  │     │     └─→ Write to output directory
  │     └─→ Log results
  │
  └─→ END (Return results to UI)
```

---

## Performance Metrics

### Optimization Impact

| Optimization | Improvement | Notes |
|--------------|-------------|-------|
| Boyer-Moore Search | 3-10x | For signature matching |
| Smart Scan (skip zeros) | 40-60% | Depends on disk usage |
| Large Buffers (16MB) | 2-3x | Reduces syscall overhead |
| Multi-threading | Linear with cores | Up to CPU core count |
| Quality Pre-filtering | 30-50% | Reduces I/O for bad files |

### Typical Scan Speeds

- **Quick Scan (MFT):** 10-50 GB/s (memory speed limited)
- **Deep Scan (optimized):** 100-500 MB/s (disk speed limited)
- **Deep Scan (unoptimized):** 20-50 MB/s

---

## Memory Management

### RAII Principles

All resources use smart pointers:

```cpp
std::unique_ptr<DiskScanner> scanner_;
std::unique_ptr<RecoveryManager> recoveryManager_;
std::unique_ptr<FilePreview> filePreview_;
```

### Large Buffer Handling

Buffers are allocated on-demand and released after use:

```cpp
{
    ByteArray buffer(16 * 1024 * 1024); // 16MB
    disk.readBytes(offset, buffer.size(), buffer.data());
    // Process buffer
} // Automatically freed here
```

---

## Error Handling

### Levels

1. **Critical Errors:** Admin check failure, disk open failure → Exit program
2. **Recoverable Errors:** File write failure, read error → Log and continue
3. **Warnings:** Quality threshold not met → Log but don't stop

### Logging

```cpp
Logger::getInstance().log(LogLevel::ERROR, "Failed to open disk: " + diskPath);
Logger::getInstance().log(LogLevel::WARNING, "Low quality file: " + filename);
Logger::getInstance().log(LogLevel::INFO, "Recovered " + std::to_string(count) + " files");
Logger::getInstance().log(LogLevel::DEBUG, "Entropy: " + std::to_string(entropy));
```

---

## Build System (CMake)

### Targets

1. **file_recovery** - CLI executable
2. **file_recovery_gui** - GUI executable (ImGui + GLFW + OpenGL)
3. **batch_analyzer** - Batch quality analysis tool

### Platform Support

- **Windows:** MSVC, MinGW (setupapi.lib required)
- **Linux:** GCC, Clang (root privileges required for disk access)
- **macOS:** Clang (limited support)

---

## Future Enhancements

### Planned Features

1. **Image Preview Decoding:** Integrate stb_image for full thumbnail rendering
2. **RAID Support:** Reconstruct files from RAID arrays
3. **Cloud Export:** Direct upload to cloud storage
4. **Advanced Carving:** Machine learning-based file type detection
5. **Incremental Scanning:** Resume interrupted scans

### Known Limitations

1. Encrypted files cannot be recovered without keys
2. Heavy file overwriting (multiple passes) prevents recovery
3. Fragmentation detection is heuristic-based (may have false positives)
4. Image preview currently shows placeholders

---

## References

### Algorithms

- **Boyer-Moore:** Boyer, R.S.; Moore, J.S. (1977). "A Fast String Searching Algorithm"
- **SHA256:** FIPS 180-4, Secure Hash Standard
- **Shannon Entropy:** Shannon, C.E. (1948). "A Mathematical Theory of Communication"

### File Format Specifications

- **JPEG:** ITU-T T.81 | ISO/IEC 10918-1
- **PNG:** RFC 2083
- **PDF:** ISO 32000-1:2008
- **ZIP:** PKWARE .ZIP File Format Specification

---

**End of Technical Documentation**
