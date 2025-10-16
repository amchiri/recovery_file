# Quick Start Guide

## Installation rapide

### Windows

1. **Téléchargez et installez les prérequis:**
   - [Visual Studio 2022 Community](https://visualstudio.microsoft.com/fr/downloads/) (avec "Développement Desktop C++")
   - OU [MinGW-w64](https://www.mingw-w64.org/)
   - [CMake](https://cmake.org/download/) (3.15 ou supérieur)

2. **Compilez le projet:**
   ```cmd
   cd recovery_file
   build.bat
   ```

3. **Lancez avec privilèges administrateur:**
   ```cmd
   cd build\Release
   file_recovery.exe --list
   ```

### Linux

1. **Installez les dépendances:**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install build-essential cmake

   # Fedora
   sudo dnf install gcc-c++ cmake

   # Arch
   sudo pacman -S base-devel cmake
   ```

2. **Compilez:**
   ```bash
   chmod +x build.sh
   ./build.sh
   ```

3. **Lancez avec sudo:**
   ```bash
   sudo ./build/file_recovery --list
   ```

## Utilisation de base

### 1. Voir les disques disponibles
```bash
file_recovery --list
```

### 2. Récupérer des fichiers supprimés (scan rapide)
```bash
# Windows
file_recovery --scan C: --output recovered\

# Linux
sudo ./file_recovery --scan /dev/sda1 --output recovered/
```

### 3. Scan profond (après formatage)
```bash
file_recovery --scan D: --mode deep --types jpg,png,pdf,docx --threads 8 --output recovered/
```

## Fonctionnalités principales

### ✅ Ce qui fonctionne actuellement

1. **Détection de systèmes de fichiers**
   - NTFS (Windows)
   - FAT32 (USB, cartes SD)
   - Détection automatique

2. **Modes de scan**
   - **Quick**: Scan rapide via MFT/FAT (recommandé pour suppressions récentes)
   - **Deep**: Scan secteur par secteur (pour formatages)
   - **Signature**: Basé sur les signatures de fichiers (disques corrompus)

3. **Types de fichiers supportés**
   - Images: JPG, PNG, GIF, BMP
   - Documents: PDF, DOC, DOCX, XLS, XLSX
   - Archives: ZIP, RAR, 7Z
   - Multimédia: MP3, MP4, AVI
   - Et plus...

4. **Performance**
   - Multi-threading (4-16 threads selon CPU)
   - Accès direct au disque
   - Buffering optimisé

### 🚧 En développement

- Support complet exFAT
- Systèmes de fichiers Linux (ext4)
- Interface graphique (GUI)
- Reconstruction de fichiers fragmentés
- Plus de signatures (objectif: 100+)

## Architecture technique

```
RecoveryEngine (Orchestration)
    │
    ├─── DiskIO (Accès bas niveau)
    │       └─── Lecture secteurs, énumération disques
    │
    ├─── FilesystemDetector (Détection FS)
    │       └─── NTFS, FAT32, exFAT, EXT4
    │
    ├─── Scanners (Modes de scan)
    │       ├─── QuickScanner (MFT/FAT)
    │       ├─── DeepScanner (Secteur par secteur)
    │       └─── SignatureScanner (Signatures)
    │
    ├─── SignatureDatabase (Détection types)
    │       └─── 15+ signatures de fichiers
    │
    └─── RecoveryManager (Récupération)
            └─── Extraction, sauvegarde, vérification
```

## Comparaison avec Disk Drill

| Fonctionnalité | File Recovery Tool | Disk Drill |
|----------------|-------------------|------------|
| Scan NTFS | ✅ Oui | ✅ Oui |
| Scan FAT32 | ✅ Oui | ✅ Oui |
| Scan profond | ✅ Oui | ✅ Oui |
| Multi-threading | ✅ Oui (4-16) | ✅ Oui |
| Signatures | ✅ 15+ types | ✅ 100+ types |
| GUI | ❌ CLI seulement | ✅ Oui |
| Open Source | ✅ MIT License | ❌ Propriétaire |
| Cross-platform | ✅ Win/Linux/Mac | ✅ Win/Mac |
| Prix | 🆓 Gratuit | 💰 $89 |

**Notre avantage**: 
- Code source ouvert et auditable
- Personnalisable et extensible
- Gratuit sans limitations
- Architecture moderne C++17

**À améliorer**:
- Plus de types de fichiers
- Interface graphique
- Détection de fragmentation avancée

## Performance attendue

Sur un SSD moderne:
- **Scan rapide (NTFS)**: ~2-5 GB/s
- **Scan profond**: ~200-500 MB/s
- **Détection signatures**: ~500 MB/s

Sur un HDD:
- **Scan rapide**: ~500 MB/s
- **Scan profond**: ~80-120 MB/s

## Sécurité et précautions

### ⚠️ IMPORTANT

1. **Ce logiciel accède directement au disque**
   - Nécessite privilèges admin/root
   - Peut lire des données sensibles
   - Utilisez avec prudence

2. **Ne jamais récupérer sur le même disque**
   - Toujours utiliser un disque différent pour `--output`
   - Risque d'écrasement des données récupérables

3. **Tester d'abord**
   - Utilisez `--list` pour vérifier les disques
   - Testez sur une petite partition
   - Vérifiez le chemin du périphérique

4. **Faire des backups**
   - Sauvegardez avant toute opération de récupération
   - Les outils de récupération ne sont pas infaillibles

## Support et aide

### Documentation
- `README.md` - Vue d'ensemble et features
- `EXAMPLES.md` - Exemples d'utilisation détaillés
- `DEVELOPMENT.md` - Guide de développement

### Problèmes courants

**"Access denied"**
→ Lancez en tant qu'administrateur/root

**"No files found"**
→ Essayez `--mode deep` ou vérifiez que les données n'ont pas été écrasées

**Scan très lent**
→ Réduisez `--threads` ou vérifiez l'état du disque

**Fichiers corrompus**
→ Normal pour les fichiers partiellement écrasés, utilisez `--verify`

### Obtenir de l'aide

1. Vérifiez la documentation
2. Lancez avec `--help`
3. Consultez EXAMPLES.md
4. Ouvrez une issue sur GitHub

## Contribution

Votre aide est bienvenue ! Le projet est open source (MIT License).

**Domaines d'amélioration prioritaires:**
1. Ajouter plus de signatures de fichiers
2. Implémenter support exFAT complet
3. Améliorer le parser NTFS
4. Créer une GUI (Qt/wxWidgets)
5. Tests et validation

Voir `DEVELOPMENT.md` pour les détails.

## Roadmap

### Version 1.0 (Actuelle)
- ✅ Architecture de base
- ✅ Scan NTFS/FAT32
- ✅ Signatures basiques
- ✅ CLI fonctionnelle
- ✅ Multi-threading

### Version 1.5 (Prochaine)
- [ ] Support exFAT complet
- [ ] 50+ signatures
- [ ] Amélioration NTFS
- [ ] Tests unitaires
- [ ] CI/CD

### Version 2.0 (Future)
- [ ] Interface graphique
- [ ] Support ext4/XFS
- [ ] Reconstruction fragmentée
- [ ] Support RAID
- [ ] 100+ signatures

## Licence

MIT License - Libre d'utilisation, modification et distribution.

Voir `LICENSE` pour les détails complets.

---

**Prêt à commencer ?**

```bash
# Windows
build.bat
cd build\Release
file_recovery.exe --list

# Linux
./build.sh
sudo ./build/file_recovery --list
```

🎉 **Bonne récupération !**
