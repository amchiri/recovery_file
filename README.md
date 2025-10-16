# 🔄 Advanced File Recovery Tool# Advanced File Recovery Tool



> Professional file recovery software with Deep Scan, Quick Scan, and modern GUI interfaceUn outil de récupération de fichiers supprimés robuste et performant, conçu pour être aussi puissant que Disk Drill.



[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)## 🎮 NOUVEAU : Support des Sauvegardes de Jeux Vidéo !

[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://en.cppreference.com/w/cpp/17)

[![Windows](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)](https://www.microsoft.com/windows)**Signature GVAS ajoutée !** Récupérez vos sauvegardes de :

- ✅ **Octopath Traveler 2**

---- ✅ **Satisfactory**, **Deep Rock Galactic**, **Palworld**, **ARK**

- ✅ **Tous les jeux Unreal Engine** utilisant le format GVAS

## ✨ Features

Voir `docs/GAME_SAVES_RECOVERY.md` pour plus de détails.

### 🚀 Dual Scanning Technology

- **🔍 Deep Scan**: Sector-by-sector signature-based recovery---

  - Scans entire disk for file signatures

  - Finds files even after filesystem corruption## Fonctionnalités

  - Smart scan with large buffers for performance

  - No metadata required### Core Features

- ✅ Scan profond des systèmes de fichiers (NTFS, FAT32, exFAT, ext4)

- **⚡ Quick Scan**: Filesystem metadata parsing- ✅ Détection de signatures de fichiers (file carving)

  - Fast MFT (NTFS) and FAT table parsing- ✅ **Récupération de sauvegardes de jeux (GVAS)** 🎮

  - Recovers recently deleted files with full metadata- ✅ Récupération par MFT (Master File Table) pour NTFS

  - Supports folder filtering- ✅ Support multi-threading pour performances optimales

  - Preserves original filenames and paths- ✅ Filtrage avancé par type de fichier, taille, date

- ✅ Preview de fichiers avant récupération

### 🎨 Modern GUI Interface- ✅ Support des disques RAW et partitions corrompues

- Beautiful Dear ImGui interface

- Real-time scan progress with statistics### Advanced Features

- **Pagination**: Navigate large result sets (100 files per page)- 🔍 Analyse de clusters et reconstruction intelligente

- **Batch Operations**: Select multiple files for bulk actions- 🔍 Support des fichiers fragmentés

  - Select All / Deselect All- 🔍 **Détection de 15+ types de fichiers** (extensible à 100+)

  - Batch Delete- 🔍 Mode recovery rapide et profond

  - Batch Copy- 🔍 Interface CLI et potentiel GUI

  - Batch Open- 🔍 Logs détaillés et rapports

- File quality indicators (Good/Fair/Poor/Bad)- 🔍 **Chargement de signatures depuis fichier externe**

- Advanced filtering by type, size, quality

- Dark theme with smooth animations## Architecture



### 🎮 Game Save Recovery```

**GVAS Signature Support** - Recover save files from:recovery_file/

- ✅ Octopath Traveler 2├── src/

- ✅ Satisfactory, Deep Rock Galactic, Palworld, ARK│   ├── core/              # Moteur principal (RecoveryEngine, DiskScanner)

- ✅ All Unreal Engine games using GVAS format│   ├── filesystem/        # Parsers de systèmes de fichiers (NTFS, FAT32)

│   ├── scanner/           # Moteurs de scan (Quick, Deep, Signature)

See `docs/GAME_SAVES_RECOVERY.md` for details.│   ├── recovery/          # Logique de récupération (RecoveryManager)

│   ├── formats/           # Parsers de formats spéciaux (GVAS, etc.)

### 📁 Supported Filesystems│   ├── gui/               # Interface graphique moderne (Dear ImGui)

- ✅ NTFS (Master File Table parsing)│   └── utils/             # Utilitaires (DiskIO, Logger, FileQuality)

- ✅ FAT32 / exFAT (FAT table parsing)├── include/               # Headers publics

- ✅ RAW / Corrupted partitions (signature scanning)├── tests/                 # Tests unitaires

├── docs/                  # 📚 Documentation complète

### 🔎 File Type Detection├── gvas_tools/            # 🎮 Scripts Python pour analyse GVAS

**15+ file signatures** including:├── build/                 # Fichiers de build

- **Images**: JPG, PNG, GIF, BMP, WebP└── signatures.txt         # Base de données de signatures (44+ types)

- **Documents**: PDF, DOCX, XLSX, PPTX```

- **Archives**: ZIP, RAR, 7Z

- **Media**: MP4, AVI, MP3## 📚 Documentation

- **Game Saves**: GVAS (Unreal Engine)

Toute la documentation est maintenant dans le dossier **`docs/`** :

Extensible to 100+ signatures via `data/signatures.json`

- **[docs/QUICKSTART.md](docs/QUICKSTART.md)** - Guide de démarrage rapide

---- **[docs/GUI_README.md](docs/GUI_README.md)** - Guide complet de l'interface graphique

- **[docs/ROADMAP.md](docs/ROADMAP.md)** - Feuille de route du projet

## 🖼️ Screenshots- **[docs/ADMIN_REQUIRED.md](docs/ADMIN_REQUIRED.md)** - Pourquoi les droits admin sont nécessaires

- **[docs/GAME_SAVES_RECOVERY.md](docs/GAME_SAVES_RECOVERY.md)** - Récupération de sauvegardes de jeux

> **Note**: Add screenshots here after capturing the GUI:- **[docs/PHASE6_AND_7_COMPLETE.md](docs/PHASE6_AND_7_COMPLETE.md)** - Dernières fonctionnalités implémentées

> - Main interface with file list

> - Scan configuration panel## 🎮 GVAS Tools

> - Results with pagination

> - Batch operations in actionScripts Python pour analyser les fichiers de sauvegarde Unreal Engine :



---- **[gvas_tools/analyze_gvas.py](gvas_tools/analyze_gvas.py)** - Analyse complète de structure GVAS

- **[gvas_tools/find_gvas_end.py](gvas_tools/find_gvas_end.py)** - Détection précise de la fin de fichier

## 🏗️ Architecture- **[gvas_tools/check_gvas_size.py](gvas_tools/check_gvas_size.py)** - Validation de taille

- Plus d'infos : **[gvas_tools/README.md](gvas_tools/README.md)**

```

recovery_file/```

├── src/

│   ├── core/              # Core engine (RecoveryEngine)## Compilation

│   ├── scanner/           # Scanning engines

│   │   ├── DeepScanner    # Sector-by-sector signature scan```bash

│   │   └── QuickScanner   # MFT/FAT metadata scanmkdir build && cd build

│   ├── filesystem/        # Filesystem parserscmake ..

│   │   ├── NTFSParser     # NTFS MFT parsingcmake --build .

│   │   └── FAT32Parser    # FAT32 table parsing```

│   ├── signatures/        # File signature detection

│   ├── recovery/          # Recovery logic## Utilisation

│   ├── gui/               # Dear ImGui interface

│   │   ├── RecoveryGUI    # Main window```bash

│   │   ├── ResultsPagination  # Page navigation# Scan rapide

│   │   └── BatchOperations    # Multi-file selection./file_recovery --scan C: --output recovered/

│   └── utils/             # Utilities (Logger, DiskIO, FileQuality)

├── include/               # Public headers# Scan profond avec filtres

├── data/                  # Signature database./file_recovery --deep-scan D: --types jpg,png,pdf --output recovered/ --threads 8

└── docs/                  # Documentation

```# Récupération spécifique

./file_recovery --recover "path/to/file" --output recovered/

### 🧠 Key Design Decisions```



**Deep vs Quick Scan**:## Technologies

- Deep Scanner scans raw sectors, no filesystem metadata

- Quick Scanner parses filesystem structures (MFT/FAT)- C++17/20

- UI automatically disables folder filtering for Deep Scan (no paths available)- CMake pour le build system

- Multi-threading (std::thread, std::async)

**Smart Scan**:- Accès bas niveau au disque (Windows API / POSIX)

- Phase 8 optimizations: Large buffers (16MB), parallel validation- Bibliothèques modernes STL

- Skips processed sectors to avoid duplicates

- Quality scoring for recovered files## Roadmap



---### ✅ Version 1.0 - Complétée (Octobre 2025)

- [x] Architecture de base complète

## 📦 Installation- [x] Parser NTFS (lecture MFT, détection fichiers supprimés)

- [x] Parser FAT32 (structure de base)

### Prerequisites- [x] Détection de signatures (15+ types de fichiers)

- **Windows 10/11** (64-bit)- [x] Moteur de scan multi-thread (Quick, Deep, Signature)

- **Visual Studio 2022** (or VS Build Tools)- [x] Interface CLI complète avec options avancées

- **CMake 3.15+**- [x] Système de logging et progression

- [x] Support Windows (accès disque bas niveau)

### Build Instructions- [x] Énumération et détection automatique des disques

- [x] RecoveryManager pour extraction de fichiers

```powershell

# Clone repository### 🚧 Version 1.5 - En cours

git clone https://github.com/amchiri/recovery_file.git- [x] **Filtre qualité fichiers** (détection fichiers corrompus/vides) ⭐ NOUVEAU

cd recovery_file- [x] **Détection précise fin GVAS** (pattern "None" + footer) 🎮

- [ ] Parser NTFS avancé (attributs complets, dates, permissions)

# Configure with CMake- [ ] Parser FAT32 complet (récursion répertoires)

cmake -B build -S . -G "Visual Studio 17 2022" -A x64- [ ] Support exFAT complet

- [ ] Ajout de 50+ signatures supplémentaires

# Build Release version- [ ] Tests unitaires complets

cmake --build build --config Release- [ ] Support Linux/macOS (accès disque POSIX)

- [ ] Reconstruction de fichiers fragmentés

# Run the GUI (requires Administrator privileges)- [ ] Documentation API complète

.\build\Release\file_recovery_gui.exe

```### 🔮 Version 2.0 - Futur

- [ ] Interface GUI (Qt/wxWidgets)

Or use the batch script:- [ ] Support ext4/XFS (Linux)

```powershell- [ ] Support HFS+/APFS (macOS)

.\build.bat- [ ] Preview de fichiers avant récupération

```- [ ] Support RAID

- [ ] Images disque (DD, E01, AFF)

### ⚠️ Administrator Privileges Required- [ ] Récupération réseau

Direct disk access requires running as Administrator:- [ ] Mode forensique avancé

- Right-click `file_recovery_gui.exe` → "Run as administrator"

- Or use `launch_gui_admin.bat`## État actuel



---**Fonctionnel ✅**

- Détection automatique NTFS/FAT32

## 🚀 Quick Start- Scan rapide via MFT/FAT

- Scan profond secteur par secteur

### 1. Launch the Application- 15+ signatures de fichiers

```powershell- Multi-threading (4-16 threads)

.\launch_gui_admin.bat- Récupération de fichiers

```- CLI complète



### 2. Select Scan Type**En développement 🚧**

- **Quick Scan**: Fast, recovers recently deleted files with metadata- Parsers complets des filesystems

  - ✅ Enable folder filtering- Plus de signatures

  - ✅ Preserves original filenames- Support cross-platform complet

  - ⚡ Faster (seconds to minutes)- Fichiers fragmentés



- **Deep Scan**: Complete disk scan with signature detection## License

  - ✅ Finds all recoverable files

  - ✅ Works on corrupted filesystemsMIT License

  - 🐌 Slower (minutes to hours depending on disk size)

### 3. Configure Scan
- **Drive**: Select target drive (e.g., `D:`)
- **File Types**: Choose types to recover (All, Images, Documents, etc.)
- **Folder Filter** (Quick Scan only): Specify folder path (e.g., `C:\Users\YourName\Documents`)

### 4. Start Scan
- Click **"Start Scan"**
- Monitor progress: Files found, sectors scanned, speed
- Wait for completion

### 5. Review Results
- **Pagination**: Navigate through results (100 per page)
- **Sorting**: Click column headers (Name, Size, Quality, Type)
- **Quality Filter**: Filter by Good/Fair/Poor/Bad files
- **Batch Selection**: Use checkboxes to select multiple files

### 6. Recover Files
- **Single File**: Click "Recover" button
- **Batch Recovery**: 
  1. Select multiple files with checkboxes
  2. Click "Copy Selected To..."
  3. Choose destination folder

---

## 🔧 Configuration

### Signature Database
Edit `data/signatures.json` to add custom file signatures:
```json
{
  "name": "MyFormat",
  "extension": ".myf",
  "signature": "4D594621",
  "offset": 0,
  "description": "My Custom Format"
}
```

See `docs/SIGNATURES_REFERENCE.md` for details.

### Settings
- `config.ini`: User preferences (theme, default paths)
- `imgui.ini`: ImGui window layout (auto-saved)

---

## 📊 Performance

Optimizations (Phase 8):
- **Large Buffers**: 16MB reads for sequential disk access
- **Smart Scan**: Skip already-processed sectors
- **Parallel Validation**: Multi-threaded file quality checks
- **Memory Efficiency**: Streaming validation, no full-file loading

Typical Performance:
- **Quick Scan**: 1-10 seconds (MFT/FAT parsing)
- **Deep Scan**: 50-200 MB/s (depends on disk speed)
- **1TB HDD**: ~2-3 hours deep scan

---

## 🗺️ Roadmap

### ✅ Completed (Phase 9 Sprint 1)
- [x] Pagination (100 items per page)
- [x] Batch Operations (multi-select, bulk actions)
- [x] Quick Scan (MFT/FAT parsing)
- [x] Filesystem detection (NTFS/FAT32/exFAT)
- [x] Folder filtering (Quick Scan)

### 🚧 Upcoming (Phase 9 Sprint 2-5)
- [ ] **Sprint 2**: Quick Preview (images, text, hex dump) - 6-8h
- [ ] **Sprint 3**: Auto-Clean (remove corrupted/duplicates) - 2-3h
- [ ] **Sprint 4**: Duplicate Finder - 6-8h
- [ ] **Sprint 5**: Polish & Integration - 2-3h

See `docs/ROADMAP.md` for detailed plan.

---

## 📚 Documentation

- **[Quick Start Guide](docs/QUICKSTART.md)** - Get started in 5 minutes
- **[Architecture Overview](docs/DEVELOPMENT.md)** - Technical deep dive
- **[Game Saves Recovery](docs/GAME_SAVES_RECOVERY.md)** - GVAS signature guide
- **[Signatures Reference](docs/SIGNATURES_REFERENCE.md)** - Adding custom signatures
- **[Phase 9 Summary](docs/PHASE9_SUMMARY.md)** - Latest features
- **[Navigation](docs/NAVIGATION.md)** - Full documentation index

---

## 🐛 Troubleshooting

### "Access Denied" Error
- **Solution**: Run as Administrator (required for disk access)
- Use `launch_gui_admin.bat` or right-click → "Run as administrator"

### "Unsupported Filesystem" Error
- **Cause**: Drive has unknown filesystem type
- **Solution**: Use Deep Scan (works on all filesystems/corrupted drives)

### No Files Found
- **Quick Scan**: Only finds recently deleted files (before MFT reuse)
  - Try Deep Scan for older files
- **Folder Filter**: Check path is correct and on selected drive
- **File Types**: Ensure correct types are selected

### Program Crashes
- Check `recovery_log.txt` for error details
- Report issues on GitHub with log file

---

## 🤝 Contributing

Contributions welcome! Please:
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit changes (`git commit -m 'Add AmazingFeature'`)
4. Push to branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

See `docs/DEVELOPMENT.md` for build instructions and architecture.

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- **Dear ImGui** - UI framework
- **GLFW** - Window/input management
- **stb_image** - Image loading
- Inspired by **Disk Drill**, **Recuva**, and **PhotoRec**

---

## 📧 Contact

**Project Repository**: [https://github.com/amchiri/recovery_file](https://github.com/amchiri/recovery_file)

**Issues**: [GitHub Issues](https://github.com/amchiri/recovery_file/issues)

---

<p align="center">Made with ❤️ for data recovery</p>
