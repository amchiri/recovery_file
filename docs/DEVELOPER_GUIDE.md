# File Recovery Tool - Developer Guide

**Version:** 1.0.0
**Target Audience:** Software developers contributing to the project
**Prerequisites:** C++17, CMake 3.15+, Git

---

## Table of Contents

1. [Getting Started](#getting-started)
2. [Development Environment Setup](#development-environment-setup)
3. [Building the Project](#building-the-project)
4. [Project Structure](#project-structure)
5. [Coding Standards](#coding-standards)
6. [Contributing Guidelines](#contributing-guidelines)
7. [Testing](#testing)
8. [Debugging](#debugging)
9. [Adding New Features](#adding-new-features)
10. [Performance Profiling](#performance-profiling)
11. [Common Development Tasks](#common-development-tasks)

---

## Getting Started

### Quick Start

```bash
# Clone the repository
git clone https://github.com/yourusername/recovery_file.git
cd recovery_file

# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
cmake --build .

# Run the CLI tool (requires admin privileges)
sudo ./file_recovery

# Or run the GUI
sudo ./file_recovery_gui
```

### Repository Structure

```
recovery_file/
├── include/              # Public headers
│   ├── RecoveryEngine.h
│   ├── RecoveryTypes.h
│   ├── FileQuality.h
│   ├── FileSignatures.h
│   └── utils/           # Utility headers
│       ├── AdminCheck.h
│       ├── BoyerMoore.h
│       ├── ConfigManager.h
│       ├── DuplicateDetector.h
│       └── FilePreview.h
├── src/                 # Implementation files
│   ├── main.cpp         # CLI entry point
│   ├── gui_main.cpp     # GUI entry point
│   ├── core/            # Core engine
│   ├── scanner/         # Scanning algorithms
│   ├── filesystem/      # Filesystem parsers
│   ├── recovery/        # File reconstruction
│   ├── signatures/      # Signature database
│   ├── utils/           # Utilities
│   └── gui/             # GUI components
├── cmake/               # CMake modules
│   └── ImGuiConfig.cmake
├── docs/                # Documentation
├── tests/               # Unit tests (optional)
├── config.ini           # Configuration file
├── signatures.txt       # External signature database
├── CMakeLists.txt       # Build configuration
└── README.md            # Project overview
```

---

## Development Environment Setup

### Windows (Visual Studio 2019/2022)

#### Prerequisites

1. **Visual Studio 2019/2022** with C++ Desktop Development workload
2. **CMake** 3.15+ (included with VS or standalone)
3. **Git** for version control

#### Setup Steps

```powershell
# Install vcpkg (for dependencies)
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Install GLFW and OpenGL (for GUI)
.\vcpkg install glfw3:x64-windows
.\vcpkg integrate install

# Clone project
git clone <repository-url>
cd recovery_file

# Generate Visual Studio solution
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]\scripts\buildsystems\vcpkg.cmake

# Open solution in Visual Studio
start file_recovery.sln
```

### Linux (Ubuntu/Debian)

#### Prerequisites

```bash
# Update package list
sudo apt update

# Install build essentials
sudo apt install build-essential cmake git

# Install GUI dependencies
sudo apt install libglfw3-dev libgl1-mesa-dev libglu1-mesa-dev

# Install optional tools
sudo apt install clang-format clang-tidy valgrind gdb
```

#### Setup Steps

```bash
# Clone repository
git clone <repository-url>
cd recovery_file

# Build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)

# Run (requires root for disk access)
sudo ./file_recovery
```

### macOS (Xcode)

#### Prerequisites

```bash
# Install Homebrew
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake glfw
```

#### Setup Steps

```bash
# Clone repository
git clone <repository-url>
cd recovery_file

# Generate Xcode project
mkdir build && cd build
cmake -G Xcode ..

# Open in Xcode
open file_recovery.xcodeproj
```

---

## Building the Project

### CMake Configuration Options

```bash
# Build type
cmake -DCMAKE_BUILD_TYPE=Release ..      # Optimized build
cmake -DCMAKE_BUILD_TYPE=Debug ..        # Debug symbols
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo  # Optimized + debug info

# Optional features
cmake -DBUILD_GUI=ON ..                  # Build GUI (default: ON)
cmake -DBUILD_TESTS=ON ..                # Build unit tests (default: OFF)

# Compiler selection
cmake -DCMAKE_CXX_COMPILER=clang++ ..    # Use Clang
cmake -DCMAKE_CXX_COMPILER=g++ ..        # Use GCC
```

### Build Targets

```bash
# Build everything
cmake --build . --target all

# Build specific target
cmake --build . --target file_recovery
cmake --build . --target file_recovery_gui
cmake --build . --target batch_analyzer

# Install
cmake --build . --target install
```

### Incremental Builds

```bash
# After modifying a .cpp file
make                  # Rebuilds only affected files

# After modifying a .h file
make clean && make    # Full rebuild (headers affect many files)

# Parallel build (faster)
make -j8              # Use 8 cores
```

---

## Coding Standards

### C++ Style Guide

#### Naming Conventions

```cpp
// Classes: PascalCase
class FileRecoveryEngine { };

// Functions: camelCase
void recoverDeletedFiles();

// Variables: camelCase
int fileCount = 0;

// Constants: UPPER_SNAKE_CASE
const int MAX_FILE_SIZE = 1024 * 1024 * 100;

// Member variables: camelCase_ (trailing underscore)
class MyClass {
private:
    int myMember_;
    std::string fileName_;
};

// Namespaces: PascalCase
namespace FileRecovery { }
namespace Utils { }
```

#### Code Formatting

Use **4 spaces** for indentation (no tabs):

```cpp
// GOOD
void myFunction() {
    if (condition) {
        doSomething();
    }
}

// BAD (tabs)
void myFunction() {
→   if (condition) {
→   →   doSomething();
→   }
}
```

#### Header Guards

Always use `#pragma once`:

```cpp
// GOOD
#pragma once

#include <string>

class MyClass { };

// AVOID (old-style guards)
#ifndef MY_CLASS_H
#define MY_CLASS_H
// ...
#endif
```

#### Include Order

```cpp
// 1. Corresponding header (for .cpp files)
#include "MyClass.h"

// 2. Project headers
#include "RecoveryEngine.h"
#include "utils/Logger.h"

// 3. Third-party headers
#include <imgui.h>

// 4. Standard library headers
#include <iostream>
#include <vector>
#include <memory>
```

### Modern C++ Best Practices

#### Use Smart Pointers

```cpp
// GOOD
std::unique_ptr<DiskScanner> scanner_;
std::shared_ptr<FileQuality> quality_;

// BAD
DiskScanner* scanner_;  // Manual memory management
```

#### Use RAII

```cpp
// GOOD
{
    std::lock_guard<std::mutex> lock(mutex_);
    // Critical section
} // Automatically unlocked

// BAD
mutex_.lock();
// Critical section
mutex_.unlock();  // Easy to forget!
```

#### Use Range-Based For Loops

```cpp
// GOOD
for (const auto& file : recoveredFiles_) {
    processFile(file);
}

// BAD
for (size_t i = 0; i < recoveredFiles_.size(); i++) {
    processFile(recoveredFiles_[i]);
}
```

#### Prefer `const` References

```cpp
// GOOD (avoid copies)
void processFile(const RecoveredFile& file);

// BAD (copies entire struct)
void processFile(RecoveredFile file);
```

### Comments and Documentation

```cpp
/**
 * @brief Recovers deleted files from a disk using deep scanning
 *
 * This function performs a sector-by-sector scan of the specified disk,
 * searching for file signatures and reconstructing files.
 *
 * @param diskPath Path to the disk device (e.g., "\\.\PhysicalDrive0")
 * @param outputDir Directory to save recovered files
 * @return Number of files successfully recovered
 *
 * @throws std::runtime_error if disk cannot be opened
 * @note Requires administrator privileges
 */
int recoverFiles(const std::string& diskPath, const std::string& outputDir);
```

---

## Contributing Guidelines

### Git Workflow

#### Branch Naming

```
feature/add-raid-support
bugfix/fix-jpeg-corruption-detection
hotfix/critical-memory-leak
refactor/optimize-boyer-moore
docs/update-api-reference
```

#### Commit Messages

Follow the **Conventional Commits** format:

```
type(scope): subject

body (optional)

footer (optional)
```

**Examples:**

```
feat(scanner): add RAID 5 support

Implement RAID 5 parity calculation and stripe reconstruction
for multi-disk recovery scenarios.

Closes #42

---

fix(quality): correct entropy calculation for small files

Files < 1KB were incorrectly flagged as DEAD due to entropy
underestimation. Added minimum sample size check.

Fixes #38

---

perf(search): optimize Boyer-Moore with SIMD instructions

Use AVX2 instructions for 4x faster pattern matching on
compatible CPUs.

Benchmarks:
- Before: 250 MB/s
- After: 1000 MB/s

---

docs(api): add developer guide and code documentation

Complete API reference with examples for all public functions.
```

### Pull Request Process

1. **Create feature branch**
   ```bash
   git checkout -b feature/my-feature
   ```

2. **Make changes with frequent commits**
   ```bash
   git add src/scanner/MyNewScanner.cpp
   git commit -m "feat(scanner): add MyNewScanner implementation"
   ```

3. **Ensure all tests pass**
   ```bash
   cmake -DBUILD_TESTS=ON ..
   make test
   ```

4. **Run code formatting**
   ```bash
   clang-format -i src/**/*.cpp include/**/*.h
   ```

5. **Push and create PR**
   ```bash
   git push origin feature/my-feature
   # Create PR on GitHub
   ```

6. **Address review comments**

7. **Squash commits before merge** (optional)
   ```bash
   git rebase -i main
   ```

### Code Review Checklist

- [ ] Code follows project style guide
- [ ] No memory leaks (verified with Valgrind)
- [ ] Thread-safe (if applicable)
- [ ] Error handling for all edge cases
- [ ] Unit tests added for new features
- [ ] Documentation updated (README, API docs)
- [ ] Performance impact measured (for core components)
- [ ] Backward compatibility maintained

---

## Testing

### Unit Tests (Optional)

#### Adding Tests

Create test file in `tests/`:

```cpp
// tests/test_boyer_moore.cpp
#include <gtest/gtest.h>
#include "utils/BoyerMoore.h"

TEST(BoyerMooreTest, FindsSimplePattern) {
    std::vector<uint8_t> pattern = {0xFF, 0xD8};  // JPEG header
    std::vector<uint8_t> data = {0x00, 0xFF, 0xD8, 0xFF};

    FileRecovery::Utils::BoyerMoore bm(pattern);
    int64_t pos = bm.search(data.data(), data.size());

    EXPECT_EQ(pos, 1);
}

TEST(BoyerMooreTest, HandlesNotFound) {
    std::vector<uint8_t> pattern = {0xFF, 0xD8};
    std::vector<uint8_t> data = {0x00, 0x00, 0x00};

    FileRecovery::Utils::BoyerMoore bm(pattern);
    int64_t pos = bm.search(data.data(), data.size());

    EXPECT_EQ(pos, -1);
}
```

#### Running Tests

```bash
cmake -DBUILD_TESTS=ON ..
make
ctest --verbose
```

### Manual Testing

#### Test with Sample Disk Image

```bash
# Create test disk image (100MB)
dd if=/dev/zero of=test_disk.img bs=1M count=100

# Copy test files to image
# (Mount as loop device, copy files, unmount, "delete" files)

# Run recovery on test image
./file_recovery --disk test_disk.img --output ./test_output
```

#### Verify Results

```bash
# Check recovered files
ls -lh test_output/

# Verify file integrity
md5sum original_file.jpg
md5sum test_output/recovered_file_00001.jpg
```

---

## Debugging

### Debug Builds

```bash
# Build with debug symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Run with GDB
gdb ./file_recovery
(gdb) run
(gdb) backtrace       # On crash
(gdb) print variable  # Inspect variables
```

### Logging Levels

Configure in `config.ini`:

```ini
[Logging]
log_level = DEBUG   # DEBUG, INFO, WARNING, ERROR
log_file = debug_log.txt
console_output = true
```

Use in code:

```cpp
LOG_DEBUG("Scanning sector " + std::to_string(sectorNum));
LOG_INFO("Found signature at offset " + std::to_string(offset));
LOG_WARNING("Quality below threshold: " + std::to_string(quality));
LOG_ERROR("Failed to read disk: " + errorMsg);
```

### Memory Debugging (Valgrind)

```bash
# Check for memory leaks
valgrind --leak-check=full --show-leak-kinds=all ./file_recovery

# Check for thread errors
valgrind --tool=helgrind ./file_recovery
```

### Performance Profiling (Linux)

```bash
# CPU profiling with perf
perf record -g ./file_recovery
perf report

# Flame graph visualization
perf script | ./FlameGraph/stackcollapse-perf.pl | ./FlameGraph/flamegraph.pl > flamegraph.svg
```

---

## Adding New Features

### Adding a New File Signature

**Step 1:** Define signature in `src/signatures/FileSignatures.cpp`

```cpp
void FileSignatures::addVideoSignatures() {
    // Existing signatures...

    // Add new signature
    addSignature({
        "webm",                               // Extension
        "WebM Video",                         // Description
        {0x1A, 0x45, 0xDF, 0xA3},            // Header magic bytes
        {},                                   // Footer (empty if none)
        1024,                                 // Min size (1KB)
        1024ULL * 1024 * 1024 * 100,         // Max size (100MB)
        false                                 // Has footer?
    });
}
```

**Step 2:** Add quality validation in `src/utils/FileQuality.cpp`

```cpp
bool FileQuality::validateStructure(const ByteArray& data,
                                    const std::string& fileType) {
    if (fileType == "webm") {
        // Check EBML header structure
        if (data.size() < 4) return false;
        // Verify EBML version, DocType, etc.
        return (data[0] == 0x1A && data[1] == 0x45);
    }
    // ...
}
```

**Step 3:** Test with sample file

```bash
# Create test case
mkdir tests/samples
cp sample.webm tests/samples/

# Run recovery
./file_recovery --deep-scan --types webm

# Verify recovered file matches original
md5sum sample.webm recovered_sample.webm
```

### Adding a New Scanner Type

**Step 1:** Create scanner class

```cpp
// src/scanner/MacOSHFSScanner.h
#pragma once

#include "DiskScanner.h"

namespace FileRecovery {

class MacOSHFSScanner : public DiskScanner {
public:
    bool scan(const std::string& diskPath) override;
    void stop() override;
    float getProgress() const override;
    std::vector<RecoveredFile> getResults() const override;

private:
    bool parseHFSPlus(DiskIO& disk);
    std::vector<RecoveredFile> results_;
    std::atomic<float> progress_{0.0f};
    std::atomic<bool> stopRequested_{false};
};

} // namespace FileRecovery
```

**Step 2:** Implement scanning logic

```cpp
// src/scanner/MacOSHFSScanner.cpp
#include "MacOSHFSScanner.h"
#include "../utils/DiskIO.h"
#include "../utils/Logger.h"

namespace FileRecovery {

bool MacOSHFSScanner::scan(const std::string& diskPath) {
    LOG_INFO("Starting HFS+ scan on: " + diskPath);

    DiskIO disk;
    if (!disk.openDisk(diskPath)) {
        LOG_ERROR("Failed to open disk");
        return false;
    }

    return parseHFSPlus(disk);
}

bool MacOSHFSScanner::parseHFSPlus(DiskIO& disk) {
    // Read HFS+ Volume Header (sector 2)
    ByteArray header(512);
    disk.readBytes(1024, 512, header.data());

    // Parse catalog file, extents overflow, etc.
    // ...

    return true;
}

} // namespace FileRecovery
```

**Step 3:** Integrate into RecoveryEngine

```cpp
// In RecoveryEngine.cpp
#include "scanner/MacOSHFSScanner.h"

bool RecoveryEngine::quickScan(const std::string& diskPath) {
    // Detect filesystem type
    std::string fsType = detectFilesystem(diskPath);

    if (fsType == "NTFS") {
        scanner_ = std::make_unique<QuickScanner>();
    } else if (fsType == "HFS+") {
        scanner_ = std::make_unique<MacOSHFSScanner>();  // NEW
    } else {
        LOG_ERROR("Unsupported filesystem: " + fsType);
        return false;
    }

    return scanner_->scan(diskPath);
}
```

### Adding GUI Features

**Step 1:** Add UI elements in `src/gui/RecoveryGUI.cpp`

```cpp
void RecoveryGUI::renderConfigPanel() {
    // Existing UI...

    // Add new checkbox
    ImGui::Checkbox("Enable RAID Recovery", &enableRAIDRecovery_);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Attempt to recover files from RAID arrays");
    }

    // Add new input field
    ImGui::InputText("RAID Configuration", raidConfig_, sizeof(raidConfig_));
}
```

**Step 2:** Add state variables in `src/gui/RecoveryGUI.h`

```cpp
class RecoveryGUI {
private:
    // Existing members...

    bool enableRAIDRecovery_ = false;
    char raidConfig_[256] = "";
};
```

**Step 3:** Wire up to backend

```cpp
void RecoveryGUI::startRecovery() {
    if (enableRAIDRecovery_) {
        engine_->setRAIDMode(true);
        engine_->setRAIDConfig(raidConfig_);
    }

    engine_->deepScan(diskPath);
}
```

---

## Common Development Tasks

### Adding Configuration Options

**1. Update `config.ini`:**

```ini
[Recovery]
max_file_size_gb = 10
skip_corrupted = true
```

**2. Load in ConfigManager:**

```cpp
auto& config = Utils::ConfigManager::getInstance();
config.load("config.ini");

size_t maxSize = config.getInt("Recovery", "max_file_size_gb", 10);
bool skipCorrupted = config.getBool("Recovery", "skip_corrupted", true);
```

### Optimizing Performance

**1. Profile first:**

```bash
perf record -g ./file_recovery
perf report
```

**2. Common optimizations:**

```cpp
// Reserve vector capacity
std::vector<RecoveredFile> files;
files.reserve(10000);  // Avoid reallocations

// Use move semantics
files.push_back(std::move(file));  // No copy

// Avoid string copies
void processFile(const std::string& path);  // const&, not value

// Reduce mutex contention
{
    RecoveredFile fileCopy;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        fileCopy = files_[i];  // Quick copy
    }
    // Process outside lock
    analyzeFile(fileCopy);
}
```

### Adding Logging

```cpp
#include "utils/Logger.h"

// Different severity levels
LOG_DEBUG("Detailed information for debugging");
LOG_INFO("General informational messages");
LOG_WARNING("Warning but program continues");
LOG_ERROR("Error occurred but recoverable");

// With variables
LOG_INFO("Recovered " + std::to_string(count) + " files");
LOG_ERROR("Failed to open " + filePath + ": " + strerror(errno));
```

---

## Troubleshooting

### Common Build Errors

#### Error: "Cannot find GLFW"

**Solution:**

```bash
# Windows
vcpkg install glfw3:x64-windows

# Linux
sudo apt install libglfw3-dev

# macOS
brew install glfw
```

#### Error: "CMake version too old"

**Solution:**

```bash
# Ubuntu
sudo snap install cmake --classic

# macOS
brew upgrade cmake

# Windows
# Download from https://cmake.org/download/
```

#### Error: "setupapi.lib not found" (Windows)

**Solution:** Install Windows SDK with Visual Studio Installer

### Common Runtime Errors

#### "Access denied" when opening disk

**Solution:** Run with administrator/root privileges

```bash
# Windows (PowerShell as Admin)
.\file_recovery.exe

# Linux/macOS
sudo ./file_recovery
```

#### "Configuration file not found"

**Solution:** Create `config.ini` from template

```bash
cp config.ini.example config.ini
```

---

## Resources

### Documentation

- [C++17 Standard](https://en.cppreference.com/w/cpp/17)
- [CMake Documentation](https://cmake.org/documentation/)
- [Dear ImGui Manual](https://github.com/ocornut/imgui)

### Tools

- **Compiler Explorer:** https://godbolt.org/ (view assembly output)
- **Quick Bench:** https://quick-bench.com/ (microbenchmarks)
- **Valgrind:** http://valgrind.org/ (memory debugging)
- **Perf:** https://perf.wiki.kernel.org/ (performance profiling)

### Learning Resources

- **Modern C++:** https://github.com/AnthonyCalandra/modern-cpp-features
- **CMake Tutorial:** https://cmake.org/cmake/help/latest/guide/tutorial/
- **File System Forensics:** "File System Forensic Analysis" by Brian Carrier

---

## Contact & Support

- **Issues:** https://github.com/yourusername/recovery_file/issues
- **Discussions:** https://github.com/yourusername/recovery_file/discussions
- **Email:** developer@example.com

---

**Happy Coding! 🚀**
