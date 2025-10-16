# Guide de développement

## Architecture du projet

### Structure des modules

1. **Core** (`src/core/`)
   - `RecoveryEngine`: Moteur principal orchestrant toutes les opérations
   - `DiskScanner`: Interface abstraite pour les différents types de scan

2. **Filesystem** (`src/filesystem/`)
   - `NTFSParser`: Parse la MFT et récupère les fichiers supprimés sur NTFS
   - `FAT32Parser`: Parse la FAT pour systèmes FAT32
   - `FilesystemDetector`: Détecte automatiquement le type de système de fichiers

3. **Scanner** (`src/scanner/`)
   - `QuickScanner`: Scan rapide utilisant les structures du FS
   - `DeepScanner`: Scan profond secteur par secteur
   - `SignatureScanner`: Scan basé uniquement sur les signatures

4. **Signatures** (`src/signatures/`)
   - `FileSignatures`: Base de données de signatures de fichiers
   - `SignatureDatabase`: Gestion et recherche de signatures

5. **Recovery** (`src/recovery/`)
   - `RecoveryManager`: Gestion de la récupération et sauvegarde
   - `FileReconstructor`: Reconstruction de fichiers fragmentés

6. **Utils** (`src/utils/`)
   - `DiskIO`: Accès bas niveau au disque
   - `Logger`: Système de logging
   - `ProgressBar`: Affichage de la progression

## Compilation

### Windows (Visual Studio)

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

### Windows (MinGW)

```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

### Linux

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

## Amélirations futures

### Priorité haute
- [ ] Implémenter complètement le parser NTFS avec lecture des attributs
- [ ] Ajouter support exFAT
- [ ] Optimiser le scan profond avec algorithmes de recherche de motifs efficaces
- [ ] Ajouter plus de signatures de fichiers (actuellement ~15, objectif: 100+)

### Priorité moyenne
- [ ] Support des systèmes de fichiers Linux (ext4, XFS)
- [ ] Interface graphique (Qt ou wxWidgets)
- [ ] Support RAID
- [ ] Détection et reconstruction de fichiers chiffrés

### Priorité basse
- [ ] Support macOS (HFS+, APFS)
- [ ] API REST pour intégration dans d'autres outils
- [ ] Support des images disque (DD, E01, etc.)

## Performance

### Optimisations implémentées
- Multi-threading pour le scan profond
- Lecture par blocs pour minimiser les I/O
- Cache de signatures
- Accès direct au disque (NO_BUFFERING)

### Benchmarks ciblés
- Scan NTFS: > 1 GB/s sur SSD
- Scan profond: > 100 MB/s
- Détection de signatures: > 500 MB/s

## Tests

Pour ajouter des tests:

```bash
cmake .. -DBUILD_TESTS=ON
cmake --build .
ctest
```

## Contribution

1. Fork le projet
2. Créer une branche (`git checkout -b feature/AmazingFeature`)
3. Commit les changements (`git commit -m 'Add AmazingFeature'`)
4. Push vers la branche (`git push origin feature/AmazingFeature`)
5. Ouvrir une Pull Request

## Sécurité

⚠️ **ATTENTION**: Ce logiciel nécessite des privilèges administrateur pour accéder aux disques bruts.

- Toujours tester sur des disques de test
- Ne JAMAIS écrire sur le disque source
- Vérifier deux fois le chemin du périphérique
- Faire des backups avant toute opération

## License

MIT License - Voir LICENSE pour plus de détails
