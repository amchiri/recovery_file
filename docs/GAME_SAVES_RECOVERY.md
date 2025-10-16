# 🎮 Récupération de Sauvegardes de Jeux Vidéo

## Signature GVAS (Unreal Engine)

### Description
**GVAS** est le format propriétaire de sauvegarde utilisé par **Unreal Engine** pour de nombreux jeux vidéo.

### Magic Bytes
```
47 56 41 53  (ASCII: "GVAS")
```

### Structure Typique

```
Offset  Bytes          Description
------  -------------  ---------------------------
0x00    47 56 41 53    Signature "GVAS"
0x04    02 00 00 00    Version du format (v2)
0x08    0A 02 00 00    Informations de structure
0x0C    ...            Données sérialisées/compressées
```

### Jeux Utilisant GVAS

#### ✅ Confirmés
- **Octopath Traveler 2** (`.sav`)
- **Satisfactory** (`.sav`)
- **Deep Rock Galactic** (`.sav`)
- **Ark: Survival Evolved** (`.arkprofile`, `.arktribe`)
- **Conan Exiles** (`.db`)
- **Palworld** (`.sav`)
- **Grounded** (`.sav`)

#### 🔍 Probables (tous les jeux Unreal Engine)
- Fortnite (sauvegardes STW)
- Astroneer
- Dead by Daylight
- Remnant: From the Ashes
- Borderlands 3
- Gears 5
- Rocket League (anciennes versions)

### Récupération avec Notre Outil

```bash
# Recherche de sauvegardes GVAS
RecoveryFileTool.exe --mode signature --disk C: --extension sav

# Recherche approfondie avec filtres
RecoveryFileTool.exe --mode deep --disk C: --filter "*.sav" --min-size 1024
```

### Structure Interne GVAS

#### En-tête Principal
```cpp
struct GVASHeader {
    char magic[4];           // "GVAS"
    int32_t saveGameVersion; // 2
    int32_t packageVersion;  // Version UE4
    int32_t engineVersion;   // Version majeure/mineure
    int32_t customVersion;   // Version custom du jeu
    // Puis données sérialisées...
};
```

#### Données Sérialisées
Les données sont sérialisées avec le système de propriétés Unreal :
- **Nom de propriété** (FString)
- **Type** (None, IntProperty, FloatProperty, StrProperty, ArrayProperty, etc.)
- **Taille**
- **Valeur**

### Outils Complémentaires

#### Pour Modifier les Sauvegardes GVAS
1. **UE4SS (Unreal Engine Scripting System)**
   - https://github.com/UE4SS-RE/RE-UE4SS
   - Permet d'injecter du code Lua

2. **GVAS Save Editor**
   - https://github.com/iMrShadow/GVAS-Save-Editor
   - Éditeur visuel pour format GVAS

3. **Satisfactory Save Editor** (exemple spécifique)
   - https://github.com/Goz3rr/SatisfactorySaveEditor

#### Pour Analyser la Structure
```bash
# Avec xxd (Linux/Git Bash)
xxd SlotData_01.sav | head -20

# Avec PowerShell
Format-Hex SlotData_01.sav | Select-Object -First 20

# Avec Python
python -c "print(open('SlotData_01.sav', 'rb').read(100).hex())"
```

### Emplacements Typiques des Sauvegardes

#### Octopath Traveler 2
```
C:\Users\<Username>\AppData\Local\Octopath_Traveler2\Saved\SaveGames\
```

#### Satisfactory
```
C:\Users\<Username>\AppData\Local\FactoryGame\Saved\SaveGames\
```

#### Deep Rock Galactic
```
C:\Users\<Username>\AppData\Local\FSD\Saved\SaveGames\
```

#### Ark: Survival Evolved
```
C:\Program Files (x86)\Steam\steamapps\common\ARK\ShooterGame\Saved\SavedArksLocal\
```

#### Palworld
```
C:\Users\<Username>\AppData\Local\Pal\Saved\SaveGames\
```

### Extensions Communes
- `.sav` - Extension standard
- `.arkprofile` / `.arktribe` - Ark: Survival Evolved
- `.db` - Conan Exiles
- `.save` - Variante alternative

### Caractéristiques pour Détection

#### Magic Bytes Secondaires
Après "GVAS", rechercher :
```
Version 2:  02 00 00 00
Version 3:  03 00 00 00
```

#### Taille Typique
- **Petites sauvegardes**: 1 KB - 100 KB (profil joueur)
- **Moyennes sauvegardes**: 100 KB - 10 MB (progression)
- **Grandes sauvegardes**: 10 MB - 100 MB+ (monde complet)

### Validation de Récupération

#### Test Rapide
```cpp
bool isValidGVAS(const ByteArray& data) {
    if (data.size() < 8) return false;
    
    // Vérifier signature
    if (data[0] != 0x47 || data[1] != 0x56 || 
        data[2] != 0x41 || data[3] != 0x53) {
        return false;
    }
    
    // Vérifier version (1-3 généralement)
    uint32_t version = *(uint32_t*)(data.data() + 4);
    return (version >= 1 && version <= 5);
}
```

### Corruption et Réparation

#### Signes de Corruption
1. **Signature intacte mais données corrompues**
   - Taille déclarée ≠ taille réelle
   - Checksums invalides

2. **Signature partielle**
   - Seulement "GV" ou "GVA" visible
   - Données fragmentées

3. **Compression corrompue**
   - Données Zlib invalides après l'en-tête

#### Stratégies de Récupération
```cpp
// 1. Rechercher toutes les occurrences de "GVAS"
// 2. Pour chaque occurrence, valider :
//    - Version plausible (1-5)
//    - Taille cohérente
//    - Structure de propriétés valide
// 3. Extraire le maximum de données possible
// 4. Tenter décompression si compressé
```

### Exemple Complet

#### Fichier Hexdump
```
00000000: 47 56 41 53 02 00 00 00  0a 02 00 00 04 00 00 00  GVAS............
00000010: 55 45 34 00 05 00 00 00  4e 6f 6e 65 00 13 00 00  UE4.....None....
00000020: 00 50 6c 61 79 65 72 4e  61 6d 65 00 0c 00 00 00  .PlayerName.....
00000030: 53 74 72 50 72 6f 70 65  72 74 79 00 08 00 00 00  StrProperty.....
00000040: 00 00 00 00 00 05 00 00  00 4a 6f 68 6e 00        .........John.
```

#### Décodage
- `47 56 41 53` = "GVAS"
- `02 00 00 00` = Version 2
- `0a 02 00 00` = Package version 522
- `50 6c 61 79 65 72 4e 61 6d 65` = "PlayerName"
- `53 74 72 50 72 6f 70 65 72 74 79` = "StrProperty"
- `4a 6f 68 6e` = "John"

### Ajout de Signatures Personnalisées

Si votre jeu utilise une variante :

```cpp
// Dans addGameSignatures()
FileSignature customSave;
customSave.extension = "gsav";  // Extension custom
customSave.mimeType = "application/x-game-save";
customSave.description = "Custom Game Save";
customSave.header = {0x47, 0x53, 0x41, 0x56}; // "GSAV"
addSignature(customSave);
```

### Ressources

- **UE4 Save Format**: https://wiki.unrealengine.com/Save_Game
- **GVAS Spec**: Reverse engineering community docs
- **Hex Editors**: HxD, 010 Editor (avec templates)
- **UE4SS Project**: https://github.com/UE4SS-RE/RE-UE4SS

---

## 🚀 Support Futur

### Signatures Planifiées
- **Unity SaveData** (varies par jeu)
- **RPG Maker Save** (`.rxdata`, `.rvdata`)
- **GameMaker Save** (varies)
- **Godot Save** (`.tres`, `.save`)

Ajoutez vos propres signatures dans `signatures.txt` !
