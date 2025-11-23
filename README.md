# 🔄 Advanced File Recovery Tool

> Professional file recovery software with Deep Scan, Quick Scan, and modern GUI interface

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Windows](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)](https://www.microsoft.com/windows)

---

## ✨ Features

### 🚀 Dual Scanning Technology
- **🔍 Deep Scan**: Sector-by-sector signature-based recovery
  - Scans entire disk for file signatures
  - Finds files even after filesystem corruption
  - Smart scan with large buffers for performance
  - No metadata required

- **⚡ Quick Scan**: Filesystem metadata parsing
  - Fast MFT (NTFS) and FAT table parsing
  - Recovers recently deleted files with full metadata
  - Supports folder filtering
  - Preserves original filenames and paths

### 🎨 Modern GUI Interface
- Beautiful Dear ImGui interface
- Real-time scan progress with statistics
- **Pagination**: Navigate large result sets (100 files per page)
- **Batch Operations**: Select multiple files for bulk actions
  - Select All / Deselect All
  - Batch Delete
  - Batch Copy
  - Batch Open
- File quality indicators (Good/Fair/Poor/Bad)
- Advanced filtering by type, size, quality
- Dark theme with smooth animations

### 🎮 Game Save Recovery
**GVAS Signature Support** - Recover save files from:
- ✅ Octopath Traveler 2
- ✅ Satisfactory, Deep Rock Galactic, Palworld, ARK
- ✅ All Unreal Engine games using GVAS format

See `docs/GAME_SAVES_RECOVERY.md` for details.

### 📁 Supported Filesystems
- ✅ NTFS (Master File Table parsing)
- ✅ FAT32 / exFAT (FAT table parsing)
- ✅ RAW / Corrupted partitions (signature scanning)

### 🔎 File Type Detection
**15+ file signatures** including:
- **Images**: JPG, PNG, GIF, BMP, WebP
- **Documents**: PDF, DOCX, XLSX, PPTX
- **Archives**: ZIP, RAR, 7Z
- **Media**: MP4, AVI, MP3
- **Game Saves**: GVAS (Unreal Engine)

Extensible to 100+ signatures via `data/signatures.json`

---

## 🏗️ Architecture

```
recovery_file/
├── src/
│   ├── core/              # Core engine (RecoveryEngine)
│   ├── scanner/           # Scanning engines
│   │   ├── DeepScanner    # Sector-by-sector signature scan
│   │   └── QuickScanner   # MFT/FAT metadata scan
│   ├── filesystem/        # Filesystem parsers
│   │   ├── NTFSParser     # NTFS MFT parsing
│   │   └── FAT32Parser    # FAT32 table parsing
│   ├── signatures/        # File signature detection
│   ├── recovery/          # Recovery logic
│   ├── gui/               # Dear ImGui interface
│   │   ├── RecoveryGUI    # Main window
│   │   ├── ResultsPagination  # Page navigation
│   │   └── BatchOperations    # Multi-file selection
│   └── utils/             # Utilities (Logger, DiskIO, FileQuality)
├── include/               # Public headers
├── data/                  # Signature database
└── docs/                  # Documentation
```

---

## 📦 Installation & Build

### Prerequisites
- **Windows 10/11** (64-bit)
- **Visual Studio 2022** (or VS Build Tools with C++ workload)
- **CMake 3.15+**
- **Git**

### Build Instructions

#### Option 1: Using Command Line

```powershell
# 1. Clone the repository
git clone https://github.com/amchiri/recovery_file.git
cd recovery_file

# 2. Create build directory
mkdir build
cd build

# 3. Configure with CMake (Visual Studio 2022)
cmake .. -G "Visual Studio 17 2022" -A x64

# 4. Build Release version
cmake --build . --config Release

# 5. The executables will be in build/Release/
# - file_recovery.exe (CLI version)
# - file_recovery_gui.exe (GUI version)
```

#### Option 2: Using Batch Script (Automated)

```powershell
# Simply run the build script
.\build.bat
```

The `build.bat` script will:
1. Create the build directory
2. Run CMake configuration
3. Build in Release mode
4. Display the output location

### Build Output

After successful build, you'll find:
- **`build/Release/file_recovery.exe`** - Command-line interface (~296 KB)
- **`build/Release/file_recovery_gui.exe`** - Graphical interface (~1.1 MB)

### ⚠️ Administrator Privileges Required

Direct disk access requires running as Administrator:
- Right-click `file_recovery_gui.exe` → "Run as administrator"
- Or use `launch_gui_admin.bat`

---

## 🚀 Quick Start

### GUI Version (Recommended)

```powershell
# Launch with admin privileges
.\launch_gui_admin.bat
```

### CLI Version

```powershell
# Quick scan of C: drive
.\build\Release\file_recovery.exe --scan C: --output recovered/

# Deep scan with filters
.\build\Release\file_recovery.exe --deep-scan D: --types jpg,png,pdf --output recovered/ --threads 8

# Recover specific file
.\build\Release\file_recovery.exe --recover "path/to/file" --output recovered/
```

### Using the GUI

1. **Select Scan Type**
   - **Quick Scan**: Fast, finds recently deleted files with metadata
   - **Deep Scan**: Complete disk scan, works on corrupted filesystems

2. **Configure**
   - Choose drive (e.g., `D:`)
   - Select file types (Images, Documents, Archives, etc.)
   - Set folder filter (Quick Scan only)

3. **Start Scan**
   - Monitor progress: files found, sectors scanned, speed
   - Wait for completion

4. **Review & Recover**
   - Browse results with pagination (100 per page)
   - Filter by quality (Good/Fair/Poor/Bad)
   - Use batch operations to select multiple files
   - Click "Recover" or "Copy Selected To..."

---

## 🔧 Troubleshooting

### Build Errors

**"CMake not found"**
```powershell
# Install CMake from: https://cmake.org/download/
# Or use Visual Studio Installer to add CMake
```

**"Visual Studio 2022 not found"**
```powershell
# Install Visual Studio 2022 Community Edition
# Make sure to select "Desktop development with C++"
```

**"Missing ImGui/GLFW"**
```powershell
# These are automatically downloaded by CMake via FetchContent
# If download fails, check your internet connection and retry
```

### Runtime Errors

**"Access Denied"**
- **Solution**: Run as Administrator (required for disk access)

**"Unsupported Filesystem"**
- **Solution**: Use Deep Scan (works on all filesystems)

**"No Files Found"**
- Quick Scan only finds recently deleted files
- Try Deep Scan for older files
- Check folder filter path is correct

---

## 📚 Documentation

Complete documentation available in `docs/`:

- **[QUICKSTART.md](docs/QUICKSTART.md)** - Get started in 5 minutes
- **[GUI_README.md](docs/GUI_README.md)** - Complete GUI guide
- **[GAME_SAVES_RECOVERY.md](docs/GAME_SAVES_RECOVERY.md)** - GVAS signature recovery
- **[ADMIN_REQUIRED.md](docs/ADMIN_REQUIRED.md)** - Why admin rights are needed
- **[ROADMAP.md](docs/ROADMAP.md)** - Project roadmap
- **[NAVIGATION.md](docs/NAVIGATION.md)** - Full documentation index

---

## 📊 Performance

**Optimizations:**
- Large buffers (16MB) for sequential disk access
- Smart scan skips processed sectors
- Multi-threaded validation
- Memory-efficient streaming

**Typical Speed:**
- **Quick Scan**: 1-10 seconds (MFT/FAT parsing)
- **Deep Scan**: 50-200 MB/s (depends on disk)
- **1TB HDD**: ~2-3 hours deep scan

---

## 🗺️ Roadmap

### ✅ Completed (Phase 9 Sprint 1)
- [x] Pagination (100 items per page)
- [x] Batch Operations (multi-select, bulk actions)
- [x] Quick Scan (MFT/FAT parsing)
- [x] Filesystem detection (NTFS/FAT32/exFAT)
- [x] Folder filtering (Quick Scan)
- [x] Modern GUI with ImGui

### 🚧 Upcoming (Phase 9 Sprint 2-5)
- [ ] **Sprint 2**: Quick Preview (images, text, hex dump)
- [ ] **Sprint 3**: Auto-Clean (remove corrupted/duplicates)
- [ ] **Sprint 4**: Duplicate Finder
- [ ] **Sprint 5**: Polish & Integration

See [ROADMAP.md](docs/ROADMAP.md) for detailed plan.

---

## 🤝 Contributing

Contributions welcome! Please:
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit changes (`git commit -m 'Add AmazingFeature'`)
4. Push to branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- **Dear ImGui** - UI framework
- **GLFW** - Window/input management
- **stb_image** - Image loading and resizing
- Inspired by **Disk Drill**, **Recuva**, and **PhotoRec**

---

## 📧 Contact

**Project Repository**: [https://github.com/amchiri/recovery_file](https://github.com/amchiri/recovery_file)

**Issues**: [GitHub Issues](https://github.com/amchiri/recovery_file/issues)

---

<p align="center">Made with ❤️ for data recovery</p>
