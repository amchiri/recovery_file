# Base de données complète des signatures de fichiers

## Comment ajouter une nouvelle signature

1. Trouvez la signature hexadécimale du type de fichier
2. Ajoutez-la dans `src/signatures/FileSignatures.cpp`
3. Recompilez le projet

### Exemple pratique

```cpp
// Dans addImageSignatures() ou créer addMoreImageSignatures()
FileSignature tiff;
tiff.extension = "tiff";
tiff.mimeType = "image/tiff";
tiff.description = "TIFF Image";
tiff.header = {0x49, 0x49, 0x2A, 0x00}; // II*. (little-endian)
// ou {0x4D, 0x4D, 0x00, 0x2A} pour big-endian
addSignature(tiff);
```

## 📚 Signatures à ajouter (prioritaires)

### Images supplémentaires

| Type | Extension | Signature (hex) | Offset |
|------|-----------|-----------------|--------|
| TIFF | tiff, tif | `49 49 2A 00` ou `4D 4D 00 2A` | 0 |
| WebP | webp | `52 49 46 46 ?? ?? ?? ?? 57 45 42 50` | 0 |
| ICO | ico | `00 00 01 00` | 0 |
| PSD | psd | `38 42 50 53` | 0 |
| CR2 (Canon RAW) | cr2 | `49 49 2A 00 10 00 00 00 43 52` | 0 |
| NEF (Nikon RAW) | nef | `4D 4D 00 2A` | 0 |
| ARW (Sony RAW) | arw | `49 49 2A 00` | 0 |
| HEIC | heic | `66 74 79 70 68 65 69 63` | 4 |

### Vidéos supplémentaires

| Type | Extension | Signature (hex) | Offset |
|------|-----------|-----------------|--------|
| MKV | mkv | `1A 45 DF A3` | 0 |
| MOV | mov | `66 74 79 70 71 74` | 4 |
| WMV | wmv | `30 26 B2 75 8E 66 CF 11` | 0 |
| FLV | flv | `46 4C 56 01` | 0 |
| MPEG | mpg, mpeg | `00 00 01 BA` ou `00 00 01 B3` | 0 |
| 3GP | 3gp | `66 74 79 70 33 67` | 4 |
| VOB | vob | `00 00 01 BA` | 0 |

### Audio supplémentaires

| Type | Extension | Signature (hex) | Offset |
|------|-----------|-----------------|--------|
| FLAC | flac | `66 4C 61 43` | 0 |
| M4A | m4a | `66 74 79 70 4D 34 41` | 4 |
| OGG | ogg | `4F 67 67 53` | 0 |
| WMA | wma | `30 26 B2 75 8E 66 CF 11` | 0 |
| AAC | aac | `FF F1` ou `FF F9` | 0 |
| MIDI | mid, midi | `4D 54 68 64` | 0 |

### Documents supplémentaires

| Type | Extension | Signature (hex) | Offset |
|------|-----------|-----------------|--------|
| RTF | rtf | `7B 5C 72 74 66 31` | 0 |
| XLS | xls | `D0 CF 11 E0 A1 B1 1A E1` | 0 |
| XLSX | xlsx | `50 4B 03 04` | 0 |
| PPTX | pptx | `50 4B 03 04` | 0 |
| ODT | odt | `50 4B 03 04` | 0 |
| ODS | ods | `50 4B 03 04` | 0 |
| EPUB | epub | `50 4B 03 04` | 0 |

### Archives supplémentaires

| Type | Extension | Signature (hex) | Offset |
|------|-----------|-----------------|--------|
| TAR | tar | `75 73 74 61 72` | 257 |
| GZ | gz | `1F 8B 08` | 0 |
| BZ2 | bz2 | `42 5A 68` | 0 |
| XZ | xz | `FD 37 7A 58 5A 00` | 0 |
| CAB | cab | `4D 53 43 46` | 0 |
| ISO | iso | `43 44 30 30 31` | 0x8001 |

### Exécutables et binaires

| Type | Extension | Signature (hex) | Offset |
|------|-----------|-----------------|--------|
| EXE | exe | `4D 5A` | 0 |
| DLL | dll | `4D 5A` | 0 |
| ELF | - | `7F 45 4C 46` | 0 |
| DMG | dmg | `78 01 73 0D 62 62 60` | 0 |
| APK | apk | `50 4B 03 04` | 0 |
| DEX | dex | `64 65 78 0A 30 33` | 0 |
| CLASS | class | `CA FE BA BE` | 0 |

### Bases de données

| Type | Extension | Signature (hex) | Offset |
|------|-----------|-----------------|--------|
| SQLite | sqlite, db | `53 51 4C 69 74 65 20 66 6F 72 6D 61 74 20 33 00` | 0 |
| MDB | mdb | `00 01 00 00 53 74 61 6E 64 61 72 64 20 4A 65 74` | 0 |
| ACCDB | accdb | `00 01 00 00 53 74 61 6E 64 61 72 64 20 41 43 45` | 0 |

### Email et messagerie

| Type | Extension | Signature (hex) | Offset |
|------|-----------|-----------------|--------|
| PST | pst | `21 42 44 4E` | 0 |
| OST | ost | `21 42 44 4E` | 0 |
| EML | eml | Texte : "From:", "Subject:" | 0 |
| MSG | msg | `D0 CF 11 E0 A1 B1 1A E1` | 0 |

### Autres formats utiles

| Type | Extension | Signature (hex) | Offset |
|------|-----------|-----------------|--------|
| VHD | vhd | `63 6F 6E 6E 65 63 74 69 78` | 0 |
| VMDK | vmdk | `4B 44 4D` | 0 |
| QCP | qcp | `52 49 46 46` | 0 |
| PDB | pdb | `00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00` | 11 (+ check) |

## 🎯 Signatures les plus demandées

1. **Photos RAW** (CR2, NEF, ARW) - Photographes professionnels
2. **HEIC** - Photos iPhone/iPad
3. **MKV** - Vidéos haute qualité
4. **FLAC** - Audio sans perte
5. **SQLite** - Bases de données mobiles/apps
6. **PST/OST** - Emails Outlook
7. **EXE/DLL** - Programmes

## 📖 Ressources pour trouver des signatures

1. **Wikipedia** : https://en.wikipedia.org/wiki/List_of_file_signatures
2. **Gary Kessler's File Signatures** : https://www.garykessler.net/library/file_sigs.html
3. **FileFormat.info** : https://www.fileformat.info/
4. **Hexed.it** : Éditeur hex en ligne pour vérifier les signatures

## 🔧 Outil d'aide

Pour trouver la signature d'un fichier :

### Windows (PowerShell)
```powershell
Get-Content -Path "fichier.ext" -Encoding Byte -TotalCount 16 | ForEach-Object { $_.ToString("X2") }
```

### Linux
```bash
xxd -l 16 fichier.ext
# ou
hexdump -C fichier.ext | head -n 1
```

### Python
```python
with open("fichier.ext", "rb") as f:
    header = f.read(16)
    print(" ".join(f"{b:02X}" for b in header))
```

## 💪 Pour un projet de niveau "Disk Drill"

Il faudrait implémenter **100-200+ signatures** couvrant :

- ✅ Tous les formats d'images courants (30+)
- ✅ Tous les formats vidéo populaires (20+)
- ✅ Tous les formats audio (15+)
- ✅ Documents Office et alternatifs (20+)
- ✅ Archives et compression (10+)
- ✅ Exécutables et bibliothèques (10+)
- ✅ Bases de données (5+)
- ✅ Emails et messagerie (5+)
- ✅ Formats système (10+)
- ✅ Formats spécialisés (20+)

**Total recommandé : 150-200 signatures**

Actuellement : **15 signatures** (10% de l'objectif)
