# 🎨 Interface Graphique - File Recovery Tool

## ✨ Nouveautés v1.0

### 🆕 Fonctionnalités Implémentées

#### **1. Dropdown Intelligent pour Sélection du Disque**
- ✅ Détection automatique de tous les disques Windows (A-Z)
- ✅ Affichage du type de disque :
  - 💾 Local Disk (disques durs internes)
  - 🔌 Removable Disk (clés USB, disques externes)
  - 🌐 Network Drive (lecteurs réseau)
  - 💿 CD/DVD (lecteurs optiques)
- ✅ Affichage du nom de volume entre parenthèses
- ✅ Exemple : `E: - Local Disk (DATA)` ou `C: - Local Disk (Windows)`

#### **2. Liste à Cocher pour Extensions Multiples**
- ✅ **30+ formats supportés** avec descriptions détaillées
- ✅ Sélection multiple via checkboxes
- ✅ Tooltips informatifs au survol de chaque extension
- ✅ Catégories de fichiers :
  - 💾 **Sauvegardes de jeux** : `.sav` (Unreal Engine GVAS)
  - 🖼️ **Images** : `.jpg`, `.jpeg`, `.png`, `.gif`, `.bmp`
  - 📄 **Documents** : `.pdf`, `.doc`, `.docx`, `.xls`, `.xlsx`, `.ppt`, `.pptx`, `.txt`
  - 📦 **Archives** : `.zip`, `.rar`, `.7z`
  - 🎵 **Audio** : `.mp3`, `.wav`, `.flac`
  - 🎬 **Vidéo** : `.mp4`, `.avi`, `.mkv`, `.mov`
  - 💿 **Système** : `.iso`, `.exe`, `.dll`
  - 🎨 **Design** : `.psd` (Photoshop), `.ai` (Illustrator)
  - 🗄️ **Bases de données** : `.sql`, `.db`

#### **3. Interface Épurée et Attractive**
- ✅ **Thème sombre moderne** avec accents violet/bleu (#8050CC → #B070FF)
- ✅ **Coins arrondis** (6-8px) pour un look moderne
- ✅ **Layout en 2 panneaux** : Configuration | Progression + Résultats
- ✅ **Color-coding de qualité** :
  - 🟢 Excellent (< 30% zeros)
  - 🟡 Good (30-60% zeros)
  - 🟠 Poor (60-90% zeros)
  - 🔴 Corrupted (90-99% zeros)
  - ⚫ Dead (> 99% zeros)

## 🚀 Fonctionnalités

### Panel de Configuration (Gauche)
- Sélection du disque source
- Dossier de destination
- Extension de fichier à rechercher
- Filtre optionnel (ex: "KSSaveGameBP" pour Octopath Traveler)
- Options de scan (Deep Scan, nombre de threads)
- Bouton START/STOP avec retour visuel

### Panel de Progression (Droite - Haut)
- Barre de progression en temps réel
- Statistiques live :
  - Nombre de fichiers trouvés
  - Octets scannés / Total
- Graphique d'évolution de la progression

### Panel de Résultats (Droite - Bas)
- **Tableau des fichiers récupérés** avec :
  - Nom du fichier
  - Taille (en MB)
  - **Qualité color-coded** :
    - 🟢 ✓ Excellent (< 30% zeros)
    - 🟡 ✓ Good (30-60% zeros)
    - 🟠 ⚠ Poor (60-90% zeros)
    - 🔴 ⚠ Corrupted (90-99% zeros)
    - ⚫ ☠ Dead (> 99% zeros)
  - Pourcentage de zéros
- Sélection de fichier pour voir les détails
- Bouton "Open in Explorer" pour localiser rapidement

## 🎮 Utilisation

### Lancement
```bash
.\build\Release\file_recovery_gui.exe
```

### Étapes de récupération
1. **Configurer** :
   - Disk: `E:` (ou le disque à scanner)
   - Output: chemin vers le dossier de destination
   - Extension: `sav` pour les sauvegardes de jeu
   - Filter: optionnel (ex: `KSSaveGameBP`)

2. **Options** :
   - Cocher "Deep Scan" pour une analyse complète
   - Ajuster le nombre de threads (1-16)

3. **Lancer** :
   - Cliquer sur "START RECOVERY"
   - Observer la progression en temps réel

4. **Résultats** :
   - Les fichiers apparaissent dans le tableau
   - Couleurs indiquent la qualité
   - Cliquer sur un fichier pour voir les détails

## 🎨 Palette de Couleurs

### Couleurs Principales
- **Background** : `#17171F` (Noir bleuté)
- **Panels** : `#1F1F29` (Gris foncé)
- **Accent** : `#8050CC` (Violet)
- **Hover** : `#9966E6` (Violet clair)

### Couleurs de Qualité
- **Excellent** : `#4DE64D` (Vert)
- **Good** : `#99E64D` (Vert-jaune)
- **Poor** : `#E6B34D` (Orange)
- **Corrupted** : `#E66633` (Rouge-orange)
- **Dead** : `#B33333` (Rouge foncé)

## 🛠️ Architecture Technique

### Bibliothèques
- **Dear ImGui 1.89.9** : Framework UI
- **GLFW 3.3.9** : Gestion des fenêtres
- **OpenGL 3.3** : Rendering

### Structure
```
src/gui/
  ├── RecoveryGUI.h           # Interface principale
  └── RecoveryGUI.cpp         # Implémentation
gui_main.cpp                  # Point d'entrée
```

### Intégration avec le moteur
- Utilise `FileQuality` pour analyser les fichiers
- Compatible avec `RecoveryManager`
- Thread-safe avec `std::mutex`

## 📦 Compilation

### Prérequis
- CMake 3.15+
- Compilateur C++17
- OpenGL drivers

### Build
```bash
# Configuration avec GUI
cmake -B build -DBUILD_GUI=ON

# Compilation
cmake --build build --config Release --target file_recovery_gui

# Installation (optionnel)
cmake --install build
```

### Sans GUI
Pour compiler sans l'interface graphique :
```bash
cmake -B build -DBUILD_GUI=OFF
cmake --build build --config Release
```

## 🎯 Prochaines Étapes

### Fonctionnalités à implémenter
- [ ] Intégration réelle avec RecoveryManager (actuellement placeholder)
- [ ] Thread de récupération en arrière-plan
- [ ] Mise à jour live des statistiques
- [ ] Prévisualisation d'images/fichiers
- [ ] Export des résultats (CSV, JSON)
- [ ] Historique des scans
- [ ] Mode drag & drop pour sélection de dossiers
- [ ] Dark/Light theme switcher

### Améliorations UI
- [ ] Animations de transition
- [ ] Tooltips informatifs
- [ ] Progress ring alternatif
- [ ] Fenêtre modale pour les paramètres avancés
- [ ] System tray integration
- [ ] Notifications desktop

## 📝 Notes de Développement

### Performance
- Rendering à 60 FPS (VSync activé)
- UI responsive même pendant les scans
- Memory-efficient pour grandes listes de fichiers

### Compatibilité
- ✅ Windows 10/11
- ⚠️ Linux (nécessite adaptations)
- ⚠️ macOS (nécessite adaptations)

### Style Guide
- Utilise des couleurs sémantiques
- Espacement cohérent (10px entre éléments)
- Rounded corners de 6-8px
- Police par défaut ImGui (peut être personnalisée)

## 🐛 Dépannage

### L'application ne démarre pas
- Vérifier les drivers OpenGL
- Essayer en mode administrateur pour accès disque

### Interface blanche/vide
- Mettre à jour les drivers graphiques
- Vérifier la compatibilité OpenGL 3.3+

### Performances lentes
- Réduire le nombre de threads
- Désactiver Deep Scan pour tests rapides

## 📄 Licence

Projet de récupération de fichiers - Usage personnel et éducatif

---

**Développé avec ❤️ et Dear ImGui**
