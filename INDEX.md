# 📂 FILE RECOVERY TOOL - Navigation Index

## 🚀 Quick Start

1. **[README.md](README.md)** - Main documentation
2. **[docs/QUICKSTART.md](docs/QUICKSTART.md)** - Quick start guide
3. **[docs/START_HERE.txt](docs/START_HERE.txt)** - Guide rapide (Français)
4. **[launch_gui_admin.bat](launch_gui_admin.bat)** - Launch GUI with admin rights

---

## 📚 Documentation (`docs/`)

### Getting Started
- [QUICKSTART.md](docs/QUICKSTART.md) - Quick start
- [GUI_README.md](docs/GUI_README.md) - Complete GUI guide
- [ADMIN_REQUIRED.md](docs/ADMIN_REQUIRED.md) - Why admin rights are needed
- [EXAMPLES.md](docs/EXAMPLES.md) - Usage examples

### Technical Documentation
- [ROADMAP.md](docs/ROADMAP.md) - Development roadmap (Phases 1-11)
- [PROJECT_SUMMARY.md](docs/PROJECT_SUMMARY.md) - Project overview
- [DEVELOPMENT.md](docs/DEVELOPMENT.md) - Development guidelines
- [CHANGELOG_GUI.md](docs/CHANGELOG_GUI.md) - GUI changelog

### Phase Completion Reports
- [PHASE5_COMPLETE.md](docs/PHASE5_COMPLETE.md) - Phase 5: GUI ↔ Engine Integration
- [PHASE6_AND_7_COMPLETE.md](docs/PHASE6_AND_7_COMPLETE.md) - Phases 6 & 7: File Recovery + Advanced Features

### Specialized Guides
- [GAME_SAVES_RECOVERY.md](docs/GAME_SAVES_RECOVERY.md) - Game saves recovery
- [GVAS_SIGNATURE_ADDED.md](docs/GVAS_SIGNATURE_ADDED.md) - GVAS format documentation
- [SIGNATURES_REFERENCE.md](docs/SIGNATURES_REFERENCE.md) - File signatures reference
- [DYNAMIC_SIGNATURES.md](docs/DYNAMIC_SIGNATURES.md) - Dynamic signature system

---

## 🎮 GVAS Tools (`gvas_tools/`)

Python scripts for analyzing Unreal Engine save files:

### Analysis Tools
- [analyze_gvas.py](gvas_tools/analyze_gvas.py) - Complete GVAS structure parser
- [find_gvas_end.py](gvas_tools/find_gvas_end.py) - Detect end of GVAS files
- [check_gvas_size.py](gvas_tools/check_gvas_size.py) - Validate file sizes
- [check_big_file.py](gvas_tools/check_big_file.py) - Analyze large files
- [test_quality_simple.py](gvas_tools/test_quality_simple.py) - Quick quality check

**See [gvas_tools/README.md](gvas_tools/README.md) for detailed usage**

---

## 💻 Source Code (`src/`)

### Main Components
```
src/
├── core/           # RecoveryEngine, DiskScanner
├── filesystem/     # NTFS, FAT32 parsers
├── scanner/        # QuickScanner, DeepScanner, SignatureScanner
├── recovery/       # RecoveryManager
├── formats/        # GVAS, JPEG, PNG parsers
├── gui/            # Dear ImGui interface
└── utils/          # DiskIO, Logger, FileQuality
```

---

## 🔧 Build & Run

### Compile
```bash
cmake --build build --config Release
```

### Launch GUI (with admin rights)
```bash
.\launch_gui_admin.bat
```

Or manually:
```bash
Start-Process ".\build\Release\file_recovery_gui.exe" -Verb RunAs
```

### Launch CLI
```bash
.\build\Release\file_recovery.exe --mode deep --disk C: --extension jpg --output recovered/
```

---

## 📊 Project Status

**Current Version**: v1.1 (October 2025)

### ✅ Completed Phases
- Phase 1: Core Engine (100%)
- Phase 2: Quality Analysis (100%)
- Phase 3: Modern GUI (100%)
- Phase 4: Documentation (100%)
- Phase 5: GUI ↔ Engine Integration (100%)
- Phase 6: Complete File Recovery (100%)
- Phase 7: Advanced GUI Features (30%)

### 🚧 In Progress
- Phase 7: Advanced GUI Features (70% remaining)
- Phase 8: Performance Optimizations
- Phase 9: GVAS Expert Mode

**Overall Progress**: ~60% complete

---

## 🛠️ Key Features

### File Recovery
- ✅ Real file saving with RecoveryManager
- ✅ Real-time quality analysis (EXCELLENT → DEAD)
- ✅ Color-coded results table
- ✅ 30+ file format support
- ✅ Multi-threaded scanning (1-16 threads)
- ✅ Specific folder scan option (limit to 10 GB)

### GUI Features
- ✅ Modern Dear ImGui interface
- ✅ Disk auto-detection (A-Z)
- ✅ Multi-extension checkboxes
- ✅ Real-time progress and speed (MB/s)
- ✅ Double-click to open files
- ✅ Right-click context menu
- ✅ Browse button for output folder

### Quality Analysis
- ✅ 5-level quality system
- ✅ Zero-percentage calculation
- ✅ Header/footer validation
- ✅ GVAS-specific detection

---

## 🎯 Quick Actions

| Action | Command/File |
|--------|--------------|
| Launch GUI | `launch_gui_admin.bat` |
| Read docs | `docs/` folder |
| Analyze GVAS | `python gvas_tools/analyze_gvas.py file.sav` |
| Build project | `cmake --build build --config Release` |
| View roadmap | `docs/ROADMAP.md` |
| GUI guide | `docs/GUI_README.md` |

---

## 📞 Support

- **Issues**: Check `docs/ADMIN_REQUIRED.md` for common problems
- **GUI Help**: See `docs/GUI_README.md`
- **Game Saves**: See `docs/GAME_SAVES_RECOVERY.md`
- **GVAS Tools**: See `gvas_tools/README.md`

---

**Last Updated**: October 15, 2025  
**Project Status**: Production-Ready ✅
