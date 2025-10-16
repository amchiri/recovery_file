# Advanced File Recovery Tool

Un outil de récupération de fichiers supprimés robuste et performant, conçu pour être aussi puissant que Disk Drill.

## 🎮 NOUVEAU : Support des Sauvegardes de Jeux Vidéo !

**Signature GVAS ajoutée !** Récupérez vos sauvegardes de :
- ✅ **Octopath Traveler 2**
- ✅ **Satisfactory**, **Deep Rock Galactic**, **Palworld**, **ARK**
- ✅ **Tous les jeux Unreal Engine** utilisant le format GVAS

Voir `docs/GAME_SAVES_RECOVERY.md` pour plus de détails.

---

## Fonctionnalités

### Core Features
- ✅ Scan profond des systèmes de fichiers (NTFS, FAT32, exFAT, ext4)
- ✅ Détection de signatures de fichiers (file carving)
- ✅ **Récupération de sauvegardes de jeux (GVAS)** 🎮
- ✅ Récupération par MFT (Master File Table) pour NTFS
- ✅ Support multi-threading pour performances optimales
- ✅ Filtrage avancé par type de fichier, taille, date
- ✅ Preview de fichiers avant récupération
- ✅ Support des disques RAW et partitions corrompues

### Advanced Features
- 🔍 Analyse de clusters et reconstruction intelligente
- 🔍 Support des fichiers fragmentés
- 🔍 **Détection de 15+ types de fichiers** (extensible à 100+)
- 🔍 Mode recovery rapide et profond
- 🔍 Interface CLI et potentiel GUI
- 🔍 Logs détaillés et rapports
- 🔍 **Chargement de signatures depuis fichier externe**

## Architecture

```
recovery_file/
├── src/
│   ├── core/              # Moteur principal (RecoveryEngine, DiskScanner)
│   ├── filesystem/        # Parsers de systèmes de fichiers (NTFS, FAT32)
│   ├── scanner/           # Moteurs de scan (Quick, Deep, Signature)
│   ├── recovery/          # Logique de récupération (RecoveryManager)
│   ├── formats/           # Parsers de formats spéciaux (GVAS, etc.)
│   ├── gui/               # Interface graphique moderne (Dear ImGui)
│   └── utils/             # Utilitaires (DiskIO, Logger, FileQuality)
├── include/               # Headers publics
├── tests/                 # Tests unitaires
├── docs/                  # 📚 Documentation complète
├── gvas_tools/            # 🎮 Scripts Python pour analyse GVAS
├── build/                 # Fichiers de build
└── signatures.txt         # Base de données de signatures (44+ types)
```

## 📚 Documentation

Toute la documentation est maintenant dans le dossier **`docs/`** :

- **[docs/QUICKSTART.md](docs/QUICKSTART.md)** - Guide de démarrage rapide
- **[docs/GUI_README.md](docs/GUI_README.md)** - Guide complet de l'interface graphique
- **[docs/ROADMAP.md](docs/ROADMAP.md)** - Feuille de route du projet
- **[docs/ADMIN_REQUIRED.md](docs/ADMIN_REQUIRED.md)** - Pourquoi les droits admin sont nécessaires
- **[docs/GAME_SAVES_RECOVERY.md](docs/GAME_SAVES_RECOVERY.md)** - Récupération de sauvegardes de jeux
- **[docs/PHASE6_AND_7_COMPLETE.md](docs/PHASE6_AND_7_COMPLETE.md)** - Dernières fonctionnalités implémentées

## 🎮 GVAS Tools

Scripts Python pour analyser les fichiers de sauvegarde Unreal Engine :

- **[gvas_tools/analyze_gvas.py](gvas_tools/analyze_gvas.py)** - Analyse complète de structure GVAS
- **[gvas_tools/find_gvas_end.py](gvas_tools/find_gvas_end.py)** - Détection précise de la fin de fichier
- **[gvas_tools/check_gvas_size.py](gvas_tools/check_gvas_size.py)** - Validation de taille
- Plus d'infos : **[gvas_tools/README.md](gvas_tools/README.md)**

```

## Compilation

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Utilisation

```bash
# Scan rapide
./file_recovery --scan C: --output recovered/

# Scan profond avec filtres
./file_recovery --deep-scan D: --types jpg,png,pdf --output recovered/ --threads 8

# Récupération spécifique
./file_recovery --recover "path/to/file" --output recovered/
```

## Technologies

- C++17/20
- CMake pour le build system
- Multi-threading (std::thread, std::async)
- Accès bas niveau au disque (Windows API / POSIX)
- Bibliothèques modernes STL

## Roadmap

### ✅ Version 1.0 - Complétée (Octobre 2025)
- [x] Architecture de base complète
- [x] Parser NTFS (lecture MFT, détection fichiers supprimés)
- [x] Parser FAT32 (structure de base)
- [x] Détection de signatures (15+ types de fichiers)
- [x] Moteur de scan multi-thread (Quick, Deep, Signature)
- [x] Interface CLI complète avec options avancées
- [x] Système de logging et progression
- [x] Support Windows (accès disque bas niveau)
- [x] Énumération et détection automatique des disques
- [x] RecoveryManager pour extraction de fichiers

### 🚧 Version 1.5 - En cours
- [x] **Filtre qualité fichiers** (détection fichiers corrompus/vides) ⭐ NOUVEAU
- [x] **Détection précise fin GVAS** (pattern "None" + footer) 🎮
- [ ] Parser NTFS avancé (attributs complets, dates, permissions)
- [ ] Parser FAT32 complet (récursion répertoires)
- [ ] Support exFAT complet
- [ ] Ajout de 50+ signatures supplémentaires
- [ ] Tests unitaires complets
- [ ] Support Linux/macOS (accès disque POSIX)
- [ ] Reconstruction de fichiers fragmentés
- [ ] Documentation API complète

### 🔮 Version 2.0 - Futur
- [ ] Interface GUI (Qt/wxWidgets)
- [ ] Support ext4/XFS (Linux)
- [ ] Support HFS+/APFS (macOS)
- [ ] Preview de fichiers avant récupération
- [ ] Support RAID
- [ ] Images disque (DD, E01, AFF)
- [ ] Récupération réseau
- [ ] Mode forensique avancé

## État actuel

**Fonctionnel ✅**
- Détection automatique NTFS/FAT32
- Scan rapide via MFT/FAT
- Scan profond secteur par secteur
- 15+ signatures de fichiers
- Multi-threading (4-16 threads)
- Récupération de fichiers
- CLI complète

**En développement 🚧**
- Parsers complets des filesystems
- Plus de signatures
- Support cross-platform complet
- Fichiers fragmentés

## License

MIT License
