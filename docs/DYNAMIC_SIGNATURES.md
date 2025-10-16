# 🎯 Système de Signatures Dynamiques

## Vue d'ensemble

Le projet supporte maintenant **3 méthodes** pour gérer les signatures de fichiers :

### ✅ Méthode 1 : Fichier Externe (RECOMMANDÉ)
**Avantages :**
- ✨ Ajouter des signatures **sans recompiler**
- 📝 Facile à éditer (format texte simple)
- 🚀 Partageable entre utilisateurs
- 🔧 Modifiable en production

**Utilisation :**
```bash
# Lancer avec le fichier signatures.txt
file_recovery.exe -d C: -o recovered
```

Le programme cherchera `signatures.txt` dans :
1. Dossier courant
2. Dossier parent
3. Sous-dossier `config/`

---

### Méthode 2 : Signatures Codées (Fallback)
Si aucun fichier externe n'est trouvé, utilise les 15 signatures intégrées :
- JPG, PNG, GIF, BMP
- MP4, AVI
- MP3, WAV
- PDF, DOC, DOCX
- ZIP, RAR, 7Z

---

### Méthode 3 : API Programmatique
```cpp
FileSignatures& signatures = FileSignatures::getInstance();

FileSignature custom;
custom.extension = "xyz";
custom.mimeType = "application/xyz";
custom.description = "Custom Format";
custom.header = {0xAB, 0xCD, 0xEF};

signatures.addSignature(custom);
```

---

## 📝 Format du fichier signatures.txt

### Syntaxe de base
```
extension|mime_type|description|header_hex|footer_hex|header_offset|max_size
```

### Exemples

#### Image simple
```
jpg|image/jpeg|JPEG Image|FF D8 FF|FF D9|0|0
```

#### Avec offset
```
mp4|video/mp4|MP4 Video|66 74 79 70||4|0
```

#### Avec wildcards (en développement)
```
webp|image/webp|WebP Image|52 49 46 46 ?? ?? ?? ?? 57 45 42 50||0|0
```

#### Taille maximale
```
ico|image/x-icon|Icon File|00 00 01 00||0|1048576
```

### Champs

| Champ | Description | Obligatoire |
|-------|-------------|-------------|
| `extension` | Extension sans le point (jpg, png) | ✅ Oui |
| `mime_type` | Type MIME (image/jpeg) | ✅ Oui |
| `description` | Description lisible | ✅ Oui |
| `header_hex` | Magic bytes en hexadécimal | ✅ Oui |
| `footer_hex` | Magic bytes de fin (optionnel) | ❌ Non |
| `header_offset` | Position du header en octets | ❌ Non (défaut: 0) |
| `max_size` | Taille max en octets (0 = illimité) | ❌ Non (défaut: 0) |

### Commentaires
```
# Ceci est un commentaire
# Les lignes vides sont ignorées

jpg|image/jpeg|JPEG Image|FF D8 FF|FF D9|0|0
```

---

## 🚀 Ajouter de nouvelles signatures

### Étape 1 : Identifier les magic bytes
```bash
# Linux/Mac
xxd fichier.xyz | head

# Windows PowerShell
Format-Hex fichier.xyz | Select-Object -First 10
```

### Étape 2 : Ajouter au fichier
```
xyz|application/xyz|Custom XYZ Format|AB CD EF 01||0|0
```

### Étape 3 : Tester
```bash
file_recovery.exe -d C: -o test -t xyz
```

---

## 📚 Signatures pré-configurées

Le fichier `signatures.txt` fourni contient déjà **40+ formats** :

### Images (9)
- JPEG, PNG, GIF, BMP, TIFF, WebP, ICO, PSD, HEIC

### Vidéos (7)
- MP4, AVI, MKV, MOV, WMV, FLV, MPEG

### Audio (6)
- MP3, WAV, FLAC, OGG, M4A, AAC

### Documents (7)
- PDF, DOC, DOCX, XLS, XLSX, PPTX, RTF

### Archives (6)
- ZIP, RAR, 7Z, TAR, GZ, BZ2

### Exécutables (4)
- EXE, DLL, ELF, DMG

### Bases de données (2)
- SQLite, MDB

### Email (2)
- PST, OST

---

## 🎨 Cas d'usage avancés

### Scanner uniquement certains types
```bash
# Photos uniquement
file_recovery.exe -d D: -o photos -t jpg,png,heic,cr2

# Documents Office
file_recovery.exe -d C: -o docs -t doc,docx,xls,xlsx,pptx
```

### Créer des profils personnalisés

**profiles/photo-pro.txt** (RAW uniquement) :
```
cr2|image/x-canon-cr2|Canon RAW|49 49 2A 00||0|0
nef|image/x-nikon-nef|Nikon RAW|4D 4D 00 2A||0|0
arw|image/x-sony-arw|Sony RAW|49 49 2A 00||0|0
dng|image/x-adobe-dng|Adobe DNG|49 49 2A 00||0|0
```

**profiles/video-editing.txt** :
```
mov|video/quicktime|QuickTime MOV|66 74 79 70 71 74||4|0
mxf|application/mxf|MXF Video|06 0E 2B 34||0|0
prproj|application/vnd.adobe.premiere|Premiere Project|50 4B 03 04||0|0
```

Utilisation :
```bash
file_recovery.exe --signatures profiles/photo-pro.txt -d D: -o raw_photos
```

---

## 🔧 Dépannage

### "Could not load external signatures"
✅ Vérifiez que `signatures.txt` existe  
✅ Placez-le dans le dossier du programme  
✅ Vérifiez les permissions de lecture

### "Failed to parse: signatures.txt"
✅ Vérifiez la syntaxe (séparateurs `|`)  
✅ Assurez-vous que les hex sont valides  
✅ Pas d'espaces en trop avant/après `|`

### Aucun fichier détecté
✅ Vérifiez que les magic bytes sont corrects  
✅ Testez avec un fichier connu  
✅ Activez le mode debug : `file_recovery.exe --debug`

---

## 🌟 Roadmap

### Version 1.1 (Court terme)
- [ ] Support des wildcards (`??`) dans les signatures
- [ ] Import/Export JSON pour signatures
- [ ] Validation automatique des signatures

### Version 1.2 (Moyen terme)
- [ ] Détection de patterns avec regex
- [ ] Signatures composites (ex: DOCX = ZIP + specific XML)
- [ ] Auto-apprentissage de nouvelles signatures

### Version 2.0 (Long terme)
- [ ] Intégration libmagic
- [ ] Base de données partagée en ligne
- [ ] API REST pour mise à jour automatique

---

## 📖 Ressources

- **Wikipedia File Signatures** : https://en.wikipedia.org/wiki/List_of_file_signatures
- **GCK's File Signatures** : https://www.garykessler.net/library/file_sigs.html
- **FileSignatures.net** : https://filesignatures.net/
- **Hex Editors** :
  - HxD (Windows) : https://mh-nexus.de/en/hxd/
  - ImHex (Cross-platform) : https://github.com/WerWolv/ImHex

---

## 💡 Exemple complet

```bash
# 1. Éditer signatures.txt pour ajouter vos formats
notepad signatures.txt

# 2. Scanner avec vos signatures personnalisées
file_recovery.exe -d D: -o recovered -t jpg,png,pdf,docx

# 3. Vérifier les résultats
dir recovered

# 4. Afficher les statistiques
file_recovery.exe --stats
```

---

## ✨ Contribution

Vous avez identifié de nouvelles signatures ? Partagez-les !

1. Créez un fichier `community_signatures.txt`
2. Testez avec des vrais fichiers
3. Documentez les sources des magic bytes
4. Partagez sur GitHub/forums

**Format de contribution :**
```
# Contributor: VotreNom
# Date: 2025-01-15
# Source: https://example.com/format-spec
# Tested: ✅ Windows 11, 50 files recovered

xyz|application/xyz|XYZ Custom Format|AB CD EF 01|12 34 56 78|0|10485760
```
