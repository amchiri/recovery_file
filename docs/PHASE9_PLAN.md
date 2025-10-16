# Phase 9: Quick Preview, Pagination & Batch Operations Plan 🎯

**Date**: 16 octobre 2025  
**Status**: PLANNING  
**Priority**: HIGH  
**Impact**: Amélioration majeure de l'UX

## Overview
Phase 9 ajoute cinq fonctionnalités essentielles pour améliorer l'expérience utilisateur lors de la gestion de nombreux fichiers récupérés.

---

## 🎯 Objectifs Phase 9

### 1. Quick Preview 👁️ (PRIORITY 1)
**Quoi** : Aperçu rapide du contenu des fichiers sans les ouvrir

**Formats supportés** :
- ✅ **Images** (JPG, PNG, BMP, GIF) → Miniature
- ✅ **Texte** (TXT, LOG, CSV) → 20 premières lignes
- ✅ **Fichiers de save** (SAV, DAT) → Infos de base (hex dump des premiers bytes)
- ⚠️ **PDF, DOCX** → Métadonnées uniquement (complexe à parser)

**Pourquoi c'est utile** :
- Identifier rapidement le bon fichier parmi 100 fichiers similaires
- Éviter d'ouvrir 50 fichiers un par un
- Voir si le fichier est vraiment récupérable visuellement

---

### 2. Pagination 📄 (PRIORITY 1)
**Quoi** : Afficher les résultats par pages de 100 fichiers

**Pourquoi c'est nécessaire** :
- Performance : ImGui ralentit avec 1000+ lignes dans un tableau
- UX : Scroller dans 5000 fichiers est horrible
- Mémoire : Charger 10,000 miniatures d'un coup = crash

**Interface** :
```
┌─────────────────────────────────────────────┐
│ Showing 1-100 of 2,547 files                │
│ [◀ Previous] Page 1 of 26 [Next ▶]         │
└─────────────────────────────────────────────┘

Files 1-100 displayed in table...

┌─────────────────────────────────────────────┐
│ [◀ Previous] [1][2][3]...[26] [Next ▶]     │
└─────────────────────────────────────────────┘
```

---

### 3. Batch Operations 🔄 (PRIORITY 2)
**Quoi** : Sélectionner plusieurs fichiers et faire des actions groupées

**Fonctionnalités** :
- ☑ **Sélection multiple** avec checkboxes
- 🗑️ **Delete Selected** - Supprimer tous les fichiers cochés
- 📁 **Copy All To...** - Copier vers un autre dossier
- 🚀 **Open All** - Ouvrir tous les fichiers sélectionnés
- ☑️ **Select All** / **Deselect All** - Cocher/décocher tout

**Pourquoi c'est utile** :
- Récupération de 100 fichiers dont 80 sont morts → Cocher et DELETE
- Sauvegarder les 20 bons fichiers ailleurs → SELECT + COPY ALL
- **Gain de temps massif** vs 80 clics individuels !

**Interface** :
```
┌──────────────────────────────────────────────────────┐
│ [Select All] [Deselect All] | Selected: 5           │
│ [Delete Selected] [Copy To...] [Open All]           │
├─────┬────────────┬──────┬─────────┬────────────────┤
│ ☑   │ Filename   │ Size │ Quality │ Actions        │
├─────┼────────────┼──────┼─────────┼────────────────┤
│ ☑   │ file1.sav  │ 2MB  │ Good    │ Open | Delete │
│ ☑   │ file2.jpg  │ 5MB  │ Exc.    │ Open | Delete │
│ ☐   │ file3.txt  │ 1KB  │ Dead    │ Open | Delete │
│ ☑   │ file4.sav  │ 3MB  │ Good    │ Open | Delete │
└─────┴────────────┴──────┴─────────┴────────────────┘
```

---

### 4. Auto-Clean 🗑️ (PRIORITY 2)
**Quoi** : Nettoyage automatique des fichiers inutiles

**Fonctionnalités** :
- 🗑️ **Auto-Clean Button** - Bouton de nettoyage rapide
- ⚙️ **Options configurables** :
  - ☑ Dead files (0% récupérable)
  - ☑ Corrupted files (>90% zéros)
  - ☐ Poor quality files (60-90% zéros)
- 📊 **Preview avant suppression** - Montre combien de fichiers seront supprimés
- ✅ **Confirmation** - Demande validation avant de supprimer

**Pourquoi c'est utile** :
- Après un scan de 500 fichiers, 300 sont morts/corrompus
- Au lieu de les supprimer manuellement pendant 30 minutes...
- **1 clic → CLEAN → Terminé !** ✨

**Interface** :
```
┌──────────────────────────────────────────────────────┐
│ 🗑️ AUTO-CLEAN                                        │
│                                                      │
│ ☑ Delete Dead files (213 files)                     │
│ ☑ Delete Corrupted files (87 files)                 │
│ ☐ Delete Poor quality files (45 files)              │
│                                                      │
│ Total to delete: 300 files (234 MB)                 │
│                                                      │
│ [🗑️ Clean Now] [Cancel]                             │
└──────────────────────────────────────────────────────┘
```

---

### 5. Duplicate Finder 🔍 (PRIORITY 3)
**Quoi** : Détection et suppression des fichiers en double

**Comment ça marche** :
1. Calcule le **hash MD5** de chaque fichier
2. Compare les hashs pour trouver les doublons
3. Groupe les fichiers identiques
4. Permet de garder le meilleur (plus récent, meilleur qualité)
5. Supprime les autres copies

**Pourquoi c'est utile** :
- Le scanner trouve souvent le même fichier plusieurs fois :
  - Fichier original
  - Copie dans la corbeille
  - Backup automatique
  - Fichier temporaire
- **Évite de garder 5 copies du même fichier !**

**Interface** :
```
┌──────────────────────────────────────────────────────┐
│ 🔍 DUPLICATE FINDER                                  │
│                                                      │
│ [Scan for Duplicates]  Status: Analyzing 245/500... │
│                                                      │
│ ═══════════════════════════════════════════════════  │
│ FOUND: 45 duplicate files in 12 groups              │
│ Total wasted space: 67.8 MB                         │
│ ═══════════════════════════════════════════════════  │
│                                                      │
│ Group 1: savegame.sav (3 copies - 6.9 MB total)     │
│   ✓ recovered_123.sav  2.3 MB  Excellent  [KEEP]    │
│   ☐ recovered_456.sav  2.3 MB  Good       [DELETE]  │
│   ☐ recovered_789.sav  2.3 MB  Poor       [DELETE]  │
│                                                      │
│ Group 2: screenshot.jpg (2 copies - 2.4 MB total)   │
│   ✓ recovered_234.jpg  1.2 MB  Excellent  [KEEP]    │
│   ☐ recovered_567.jpg  1.2 MB  Excellent  [DELETE]  │
│                                                      │
│ [Smart Clean - Keep Best] [Manual Review] [Cancel]  │
└──────────────────────────────────────────────────────┘

Actions:
• Smart Clean: Garde automatiquement la meilleure copie
• Manual Review: Tu choisis quoi garder
```

**Algorithme de sélection "Smart"** :
```
Pour chaque groupe de doublons:
  1. Trier par qualité (Excellent > Good > Poor > Corrupted)
  2. Si égalité, garder le fichier avec le moins de zéros
  3. Si égalité, garder le plus récent
  4. Marquer les autres pour suppression
```

---

## 📐 Design d'Interface

### Layout avec Preview Panel + Batch Operations :

```
┌────────────────┬─────────────────────────────────────────┐
│                │ [Select All] [Deselect All] Selected: 5 │
│  CONTROL       │ [Delete] [Copy To] [Open All] [🗑 Clean]│
│  PANEL         │                                          │
│                │  RESULTS TABLE                           │
│  • Disks       │  ┌───┬────┬────┬─────┬────────┐        │
│  • Extensions  │  │☑  │File│Size│Qual.│ Zeros% │        │
│  • Options     │  ├───┼────┼────┼─────┼────────┤        │
│                │  │☑  │sav │ 2MB│ Good│  23%   │ ←─┐   │
│  [START]       │  │☐  │jpg │ 5MB│ Exc.│   5%   │   │   │
│                │  │☑  │txt │ 1MB│ Poor│  78%   │   │   │
│                │  └───┴────┴────┴─────┴────────┘   │   │
│                │                                     │   │
│                │  Pages: [◀][1][2][3][▶]           │   │
│                │  [🔍 Find Duplicates]               │   │
├────────────────┴──────────────────────────────────────────┤
│                                                           │
│  PREVIEW PANEL (Selected file)                           │
│  ┌───────────────────────────────────────────────────┐   │
│  │ 📄 recovered_12345.jpg                            │   │
│  │ Size: 2.3 MB | Quality: Good | Zeros: 23%        │   │
│  │                                                    │   │
│  │  ┌─────────────────────────────────┐              │   │
│  │  │                                 │              │   │
│  │  │   [IMAGE PREVIEW]               │              │   │
│  │  │                                 │              │   │
│  │  │   256x256 thumbnail             │              │   │
│  │  │                                 │              │   │
│  │  └─────────────────────────────────┘              │   │
│  │                                                    │   │
│  │  [Open File] [Open in Explorer] [Delete]          │   │
│  └───────────────────────────────────────────────────┘   │
│                                                           │
└───────────────────────────────────────────────────────────┘
```

---
│  │ Size: 2.3 MB | Quality: Good | Zeros: 23%│   │
│  │                                           │   │
│  │  ┌─────────────────────────────┐         │   │
│  │  │                             │         │   │
│  │  │   [IMAGE PREVIEW]           │         │   │
│  │  │                             │         │   │
│  │  │   256x256 thumbnail         │         │   │
│  │  │                             │         │   │
│  │  └─────────────────────────────┘         │   │
│  │                                           │   │
│  │  [Open File] [Open in Explorer]          │   │
│  └──────────────────────────────────────────┘   │
│                                                  │
└──────────────────────────────────────────────────┘
```

---

## 🔧 Implementation Details

### A. Quick Preview - Images

**Bibliothèque** : `stb_image.h` (déjà incluse avec ImGui !)

**Code structure** :
```cpp
class FilePreview {
public:
    void renderPreview(const RecoveredFileInfo& file);
    
private:
    void renderImagePreview(const std::string& filepath);
    void renderTextPreview(const std::string& filepath);
    void renderHexPreview(const std::string& filepath);
    
    // Cache pour les miniatures
    std::unordered_map<std::string, GLuint> thumbnailCache_;
    
    GLuint loadImageTexture(const std::string& filepath);
    void clearThumbnailCache();
};
```

**Étapes** :
1. Charger l'image avec `stb_image_load()`
2. Créer une texture OpenGL
3. Afficher avec `ImGui::Image()`
4. Mettre en cache pour ne pas recharger

**Optimisations** :
- Limiter taille des miniatures (256x256 max)
- Charger uniquement l'image sélectionnée (pas toutes !)
- Cache LRU : garder max 50 miniatures en mémoire

---

### B. Quick Preview - Texte

**Code structure** :
```cpp
void FilePreview::renderTextPreview(const std::string& filepath) {
    std::ifstream file(filepath);
    std::string line;
    std::vector<std::string> lines;
    
    // Lire les 20 premières lignes
    int count = 0;
    while (std::getline(file, line) && count < 20) {
        lines.push_back(line);
        count++;
    }
    
    // Afficher dans ImGui
    ImGui::BeginChild("TextPreview", ImVec2(0, 300), true);
    for (const auto& l : lines) {
        ImGui::TextUnformatted(l.c_str());
    }
    if (count == 20) {
        ImGui::TextDisabled("... (truncated)");
    }
    ImGui::EndChild();
}
```

---

### C. Quick Preview - Hex Dump

**Pour fichiers binaires** (SAV, DAT, etc.) :

```cpp
void FilePreview::renderHexPreview(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    std::vector<uint8_t> bytes(256); // Premier 256 bytes
    file.read((char*)bytes.data(), 256);
    
    ImGui::BeginChild("HexPreview", ImVec2(0, 300), true, ImGuiWindowFlags_HorizontalScrollbar);
    
    // Affichage en format hex
    for (size_t i = 0; i < bytes.size(); i += 16) {
        // Offset
        ImGui::Text("%08X:", i);
        ImGui::SameLine();
        
        // Hex values
        for (size_t j = 0; j < 16 && i+j < bytes.size(); j++) {
            ImGui::Text("%02X ", bytes[i+j]);
            if (j < 15) ImGui::SameLine();
        }
        
        ImGui::SameLine(300);
        
        // ASCII representation
        for (size_t j = 0; j < 16 && i+j < bytes.size(); j++) {
            char c = bytes[i+j];
            ImGui::Text("%c", (c >= 32 && c <= 126) ? c : '.');
            if (j < 15) ImGui::SameLine();
        }
    }
    
    ImGui::EndChild();
}
```

---

### D. Pagination

**Code structure** :
```cpp
class ResultsPagination {
public:
    ResultsPagination(size_t itemsPerPage = 100);
    
    void setTotalItems(size_t total);
    void render(); // Affiche les contrôles de pagination
    
    size_t getCurrentPage() const { return currentPage_; }
    size_t getStartIndex() const { return currentPage_ * itemsPerPage_; }
    size_t getEndIndex() const;
    size_t getTotalPages() const;
    
    void nextPage();
    void prevPage();
    void goToPage(size_t page);
    
private:
    size_t currentPage_ = 0;
    size_t itemsPerPage_ = 100;
    size_t totalItems_ = 0;
};
```

---

### E. Batch Operations

**Code structure** :
```cpp
class BatchOperations {
public:
    void renderBatchControls();
    void toggleSelection(size_t index);
    void selectAll();
    void deselectAll();
    
    bool isSelected(size_t index) const;
    size_t getSelectedCount() const;
    std::vector<size_t> getSelectedIndices() const;
    
    void deleteSelected(const std::vector<RecoveredFileInfo>& files);
    void copySelectedTo(const std::vector<RecoveredFileInfo>& files, const std::string& destPath);
    void openSelected(const std::vector<RecoveredFileInfo>& files);
    
private:
    std::unordered_set<size_t> selectedFiles_;
};
```

**UI Code** :
```cpp
void BatchOperations::renderBatchControls() {
    size_t selected = getSelectedCount();
    
    if (ImGui::Button("Select All")) {
        selectAll();
    }
    ImGui::SameLine();
    if (ImGui::Button("Deselect All")) {
        deselectAll();
    }
    ImGui::SameLine();
    ImGui::Text("Selected: %zu", selected);
    
    ImGui::Spacing();
    
    if (selected > 0) {
        if (ImGui::Button(("Delete Selected (" + std::to_string(selected) + ")").c_str())) {
            if (confirmDelete()) {
                deleteSelected(recoveredFiles_);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Copy All To...")) {
            std::string destPath = openFolderDialog();
            if (!destPath.empty()) {
                copySelectedTo(recoveredFiles_, destPath);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Open All")) {
            openSelected(recoveredFiles_);
        }
    } else {
        ImGui::BeginDisabled();
        ImGui::Button("Delete Selected (0)");
        ImGui::SameLine();
        ImGui::Button("Copy All To...");
        ImGui::SameLine();
        ImGui::Button("Open All");
        ImGui::EndDisabled();
    }
}
```

---

### F. Auto-Clean

**Code structure** :
```cpp
class AutoClean {
public:
    void renderAutoCleanDialog();
    
    struct CleanOptions {
        bool cleanDead = true;
        bool cleanCorrupted = true;
        bool cleanPoor = false;
    };
    
    CleanOptions options_;
    
    size_t countFilesToClean(const std::vector<RecoveredFileInfo>& files) const;
    void performClean(std::vector<RecoveredFileInfo>& files);
    
private:
    bool shouldClean(const RecoveredFileInfo& file) const;
};
```

**UI Code** :
```cpp
void AutoClean::renderAutoCleanDialog() {
    if (!ImGui::BeginPopupModal("Auto-Clean", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        return;
    }
    
    ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.3f, 1.0f), "AUTO-CLEAN OPTIONS");
    ImGui::Spacing();
    
    // Count files for each category
    size_t deadCount = 0, corruptedCount = 0, poorCount = 0;
    uint64_t totalSize = 0;
    
    for (const auto& file : recoveredFiles_) {
        if (file.quality == FileQualityLevel::DEAD && options_.cleanDead) {
            deadCount++;
            totalSize += file.size;
        }
        if (file.quality == FileQualityLevel::CORRUPTED && options_.cleanCorrupted) {
            corruptedCount++;
            totalSize += file.size;
        }
        if (file.quality == FileQualityLevel::POOR && options_.cleanPoor) {
            poorCount++;
            totalSize += file.size;
        }
    }
    
    ImGui::Checkbox(("Delete Dead files (" + std::to_string(deadCount) + " files)").c_str(), 
                   &options_.cleanDead);
    ImGui::Checkbox(("Delete Corrupted files (" + std::to_string(corruptedCount) + " files)").c_str(), 
                   &options_.cleanCorrupted);
    ImGui::Checkbox(("Delete Poor quality files (" + std::to_string(poorCount) + " files)").c_str(), 
                   &options_.cleanPoor);
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    size_t totalToDelete = deadCount + corruptedCount + poorCount;
    ImGui::TextColored(ImVec4(0.9f, 0.3f, 0.3f, 1.0f), 
        "Total to delete: %zu files (%.2f MB)", 
        totalToDelete, totalSize / (1024.0 * 1024.0));
    
    ImGui::Spacing();
    
    if (ImGui::Button("🗑️ Clean Now", ImVec2(120, 0))) {
        performClean(recoveredFiles_);
        ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
        ImGui::CloseCurrentPopup();
    }
    
    ImGui::EndPopup();
}
```

---

### G. Duplicate Finder

**Code structure** :
```cpp
class DuplicateFinder {
public:
    struct DuplicateGroup {
        std::string hash;
        std::vector<size_t> fileIndices;  // Indices dans recoveredFiles_
        size_t keepIndex;  // Index du fichier à garder
    };
    
    void scanForDuplicates(const std::vector<RecoveredFileInfo>& files);
    void renderDuplicatesDialog();
    
    std::vector<DuplicateGroup> getDuplicateGroups() const;
    void smartSelectBest();  // Sélectionne automatiquement le meilleur de chaque groupe
    void deleteMarkedDuplicates(std::vector<RecoveredFileInfo>& files);
    
private:
    std::vector<DuplicateGroup> duplicateGroups_;
    bool scanning_ = false;
    float progress_ = 0.0f;
    
    std::string calculateMD5(const std::string& filepath);
    size_t findBestFileInGroup(const DuplicateGroup& group, 
                                const std::vector<RecoveredFileInfo>& files);
};
```

**MD5 Calculation** :
```cpp
// Utiliser OpenSSL ou une bibliothèque légère comme MD5.h
#include <openssl/md5.h>

std::string DuplicateFinder::calculateMD5(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) return "";
    
    MD5_CTX md5Context;
    MD5_Init(&md5Context);
    
    char buffer[4096];
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        MD5_Update(&md5Context, buffer, file.gcount());
    }
    
    unsigned char result[MD5_DIGEST_LENGTH];
    MD5_Final(result, &md5Context);
    
    // Convert to hex string
    std::stringstream ss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)result[i];
    }
    
    return ss.str();
}
```

**Smart Selection Algorithm** :
```cpp
size_t DuplicateFinder::findBestFileInGroup(const DuplicateGroup& group, 
                                             const std::vector<RecoveredFileInfo>& files) {
    size_t bestIdx = group.fileIndices[0];
    const auto& bestFile = files[bestIdx];
    
    for (size_t idx : group.fileIndices) {
        const auto& file = files[idx];
        
        // 1. Compare quality (higher is better)
        if (static_cast<int>(file.quality) < static_cast<int>(bestFile.quality)) {
            bestIdx = idx;
            continue;
        }
        
        // 2. If same quality, compare zero percentage (lower is better)
        if (file.quality == bestFile.quality && file.zeroPercentage < bestFile.zeroPercentage) {
            bestIdx = idx;
            continue;
        }
        
        // 3. If still equal, keep first one (arbitrary but consistent)
    }
    
    return bestIdx;
}
```

---

### D. Pagination (suite)

**Code structure** :
```cpp
class ResultsPagination {
public:
    ResultsPagination(size_t itemsPerPage = 100);
    
    void setTotalItems(size_t total);
    void render(); // Affiche les contrôles de pagination
    
    size_t getCurrentPage() const { return currentPage_; }
    size_t getStartIndex() const { return currentPage_ * itemsPerPage_; }
    size_t getEndIndex() const;
    size_t getTotalPages() const;
    
    void nextPage();
    void prevPage();
    void goToPage(size_t page);
    
private:
    size_t currentPage_ = 0;
    size_t itemsPerPage_ = 100;
    size_t totalItems_ = 0;
};
```

**UI Code** :
```cpp
void ResultsPagination::render() {
    size_t totalPages = getTotalPages();
    
    ImGui::Text("Showing %zu-%zu of %zu files", 
        getStartIndex() + 1, 
        std::min(getEndIndex(), totalItems_),
        totalItems_);
    
    ImGui::SameLine();
    
    // Previous button
    if (currentPage_ > 0) {
        if (ImGui::Button("◀ Previous")) {
            prevPage();
        }
    } else {
        ImGui::BeginDisabled();
        ImGui::Button("◀ Previous");
        ImGui::EndDisabled();
    }
    
    ImGui::SameLine();
    ImGui::Text("Page %zu of %zu", currentPage_ + 1, totalPages);
    ImGui::SameLine();
    
    // Next button
    if (currentPage_ < totalPages - 1) {
        if (ImGui::Button("Next ▶")) {
            nextPage();
        }
    } else {
        ImGui::BeginDisabled();
        ImGui::Button("Next ▶");
        ImGui::EndDisabled();
    }
    
    // Page number buttons (show 5 pages max)
    ImGui::Spacing();
    size_t startPage = currentPage_ > 2 ? currentPage_ - 2 : 0;
    size_t endPage = std::min(startPage + 5, totalPages);
    
    for (size_t i = startPage; i < endPage; i++) {
        if (i > startPage) ImGui::SameLine();
        
        if (i == currentPage_) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.3f, 0.8f, 1.0f));
        }
        
        if (ImGui::Button(std::to_string(i + 1).c_str())) {
            goToPage(i);
        }
        
        if (i == currentPage_) {
            ImGui::PopStyleColor();
        }
    }
}
```

---

## 📊 Performance Impact

### Sans Pagination (actuellement) :
```
1,000 fichiers   → OK (léger lag au scroll)
5,000 fichiers   → Slow (scroll très lent)
10,000 fichiers  → Unusable (freeze de l'UI)
```

### Avec Pagination (Phase 9) :
```
100 fichiers/page → Toujours fluide ✅
Navigation rapide → Changement de page instantané ✅
Mémoire optimisée → Charge seulement la page active ✅
```

### Avec Quick Preview :
```
Sans cache → Charge image à chaque sélection (lent)
Avec cache → Charge une fois, réutilise (rapide ✅)
LRU cache  → Max 50 miniatures en mémoire (efficace ✅)
```

---

## 🎮 User Experience

### Workflow typique :

1. **Lancer un scan** → Trouve 2,547 fichiers

2. **Voir page 1** (fichiers 1-100)
   - Filtrer par qualité : ☑ Excellent ☑ Good
   - Résultat : 847 fichiers

3. **Navigation** :
   - Page 1 → fichiers 1-100
   - Page 2 → fichiers 101-200
   - Etc.

4. **Sélectionner un fichier** → Preview apparaît en bas
   - Si image : voir la miniature
   - Si texte : voir le contenu
   - Si SAV : voir le hex dump

5. **Décider** :
   - C'est le bon ? → [Open File]
   - Mauvais fichier ? → Sélectionner le suivant
   - Voir dans Explorer ? → [Open in Explorer]

**Temps gagné** : Au lieu d'ouvrir 50 fichiers, tu en ouvres 2-3 ! ⚡

---

## 🚀 Implementation Order

### Sprint 1 : Pagination (2-3 heures)
1. Créer classe `ResultsPagination`
2. Modifier `renderResultsPanel()` pour n'afficher qu'une page
3. Ajouter contrôles de pagination en haut et bas
4. Tester avec 1000+ fichiers

### Sprint 2 : Preview Panel Layout (1 heure)
1. Ajouter panel en bas des résultats
2. Afficher infos de base du fichier sélectionné
3. Boutons [Open File] [Open in Explorer]

### Sprint 3 : Image Preview (2-3 heures)
1. Intégrer `stb_image.h`
2. Charger image et créer texture OpenGL
3. Afficher avec `ImGui::Image()`
4. Implémenter cache LRU

### Sprint 4 : Text & Hex Preview (1-2 heures)
1. Lire et afficher fichiers texte
2. Implémenter hex viewer
3. Auto-détection du type de fichier

### Sprint 5 : Polish & Testing (1 heure)
1. Optimiser performance
2. Gérer les erreurs (fichier corrompu, etc.)
3. Améliorer UI/UX
4. Tests avec différents types de fichiers

**Total estimé : 7-11 heures** (1-2 jours de dev)

---

## ✅ Success Criteria

Phase 9 est validée si :

- ✅ **Pagination** fonctionne avec 10,000+ fichiers sans lag
- ✅ **Preview** d'images (JPG, PNG) s'affiche correctement
- ✅ **Preview** de texte montre 20 premières lignes
- ✅ **Hex dump** fonctionne pour fichiers binaires
- ✅ **Cache** ne consomme pas trop de RAM (< 100 MB)
- ✅ **Batch Operations** permet sélection multiple et actions groupées
- ✅ **Auto-Clean** supprime Dead/Corrupted files en un clic
- ✅ **Duplicate Finder** détecte et élimine doublons intelligemment
- ✅ Changement de page est instantané (< 100ms)
- ✅ Sélection de fichier affiche le preview (< 200ms)
- ✅ UI reste responsive avec 10,000+ fichiers
- ✅ Pas de crash avec images corrompues ou fichiers verrouillés

---

## � Implementation Timeline

### **Sprint 1 : Pagination + Batch Operations (4-6 heures)**
1. Créer `ResultsPagination` class (1h)
2. Créer `BatchOperations` class (1h)
3. Intégrer dans `RecoveryGUI` (2h)
4. Ajouter checkbox column dans la table (1h)
5. Tests et debug (1h)

**Validation** : ✅ Peut sélectionner 50 fichiers sur 1000 et les supprimer en batch

---

### **Sprint 2 : Quick Preview (6-8 heures)**
1. Intégrer `stb_image.h` pour images (1h)
2. Créer `PreviewPanel` class (2h)
3. Implémenter ImagePreview avec thumbnails (2h)
4. Implémenter TextPreview et HexDump (2h)
5. Implémenter LRU cache (1h)
6. Tests et polish (2h)

**Validation** : ✅ Peut prévisualiser JPG 4K, fichier texte 50 KB, et hex dump de fichier binaire

---

### **Sprint 3 : Auto-Clean (2-3 heures)**
1. Créer `AutoClean` class (30min)
2. Implémenter dialog avec options (1h)
3. Implémenter logique de suppression (1h)
4. Tests avec différents scénarios (30min)

**Validation** : ✅ Peut nettoyer 200 fichiers Dead + Corrupted en un clic avec confirmation

---

### **Sprint 4 : Duplicate Finder (6-8 heures)**
1. Intégrer OpenSSL ou MD5 library (1h)
2. Créer `DuplicateFinder` class (2h)
3. Implémenter calcul MD5 threaded (2h)
4. Implémenter smart selection algorithm (1h)
5. Créer dialog avec groupes de doublons (2h)
6. Tests avec vrais doublons (2h)

**Validation** : ✅ Peut détecter 50 groupes de doublons dans 500 fichiers et supprimer intelligemment

---

### **Sprint 5 : Polish & Integration (2-3 heures)**
1. Intégrer toutes les features dans l'UI principale
2. Vérifier que tout fonctionne ensemble
3. Optimiser performances si nécessaire
4. Documenter les nouvelles features

**📊 Total estimé : 20-28 heures (2.5 à 3.5 jours de développement)**

---

## 🎯 Testing Strategy

### **A. Unit Tests**

**Pagination** :
- Test avec 0, 50, 100, 500, 1000, 5000 fichiers
- Vérifier navigation (prev/next/goto)
- Vérifier limites (pas de page négative, pas au-delà du total)

**Batch Operations** :
- Tester sélection/désélection multiple
- Tester suppression batch (rollback si échec)
- Tester copie batch avec gestion d'erreurs

**Auto-Clean** :
- Tester avec différentes combinaisons d'options
- Vérifier comptage correct avant suppression
- Tester annulation

**Duplicate Finder** :
- Tester avec vrais doublons (mêmes fichiers)
- Tester avec fichiers similaires mais différents
- Vérifier algorithme de sélection smart

### **B. Performance Tests**

- **Pagination** : Doit afficher instantanément (<1ms) même avec 10,000+ fichiers
- **Preview** : Image 1920x1080 doit s'afficher en <100ms
- **MD5 Calculation** : 1 GB de fichiers doit se calculer en <30 secondes
- **Batch Delete** : 1000 fichiers doivent se supprimer en <5 secondes

### **C. UI/UX Tests**

- Vérifier que les contrôles sont intuitifs
- Tester avec clavier (Tab, Enter, Esc)
- Vérifier feedback visuel (loading spinners, progress bars)
- Tester fenêtre redimensionnée (responsive)

---

## 🛠️ Technical Notes

### **Libraries Required**
- **stb_image.h** : Pour charger JPG/PNG/BMP (header-only, facile)
- **OpenSSL** ou **MD5.h** : Pour calcul MD5 (OpenSSL déjà disponible sur Windows)
- **ImGui** : Déjà intégré (version 1.89.9)

### **Memory Considerations**
- **LRU Cache** : Limiter à 50 thumbnails max (~ 50 MB)
- **MD5 Calculation** : Libérer mémoire après chaque fichier
- **Preview Buffer** : Limiter à 10 MB max pour text preview

### **Threading**
- **MD5 Calculation** : Utiliser thread pool pour calculer en parallèle
- **Thumbnail Loading** : Charger en background thread pour ne pas bloquer UI
- **Batch Operations** : Exécuter delete/copy dans thread séparé avec progress bar

---

## ⚠️ Risks & Mitigations

| Risque | Impact | Mitigation |
|--------|--------|------------|
| stb_image ne supporte pas certains formats | Moyen | Afficher message "Format not supported" |
| MD5 calcul trop lent sur gros fichiers | Moyen | Afficher progress bar, permettre annulation |
| Batch delete échoue sur fichier verrouillé | Faible | Continue avec les autres, affiche erreur |
| Preview crash sur fichier corrompu | Élevé | Wrap dans try-catch, afficher "Preview failed" |
| Pagination lente avec 10,000+ fichiers | Moyen | Optimiser avec indices précalculés |

---

## 🔮 Future Enhancements (Phase 10+)

Après Phase 9, on pourrait ajouter :
- **Export Report** : Générer PDF/HTML avec liste des fichiers récupérés
- **Advanced Filters** : Filtrer par type, taille, qualité, date
- **Sort Columns** : Trier par colonne (nom, taille, qualité, etc.)
- **Search Function** : Recherche par nom de fichier
- **Favorites** : Marquer des fichiers importants
- **Compare View** : Comparer 2 versions d'un fichier
- **Metadata Viewer** : Afficher EXIF, ID3, etc.
- **Advanced Preview** : PDF, DOCX avec bibliothèques externes
- **Zoom/Rotation** dans preview d'image
- **Export to CSV** : Exporter la liste de fichiers

---

**🚀 Prêt pour Phase 9 !**

**Estimation** : **2.5 à 3.5 jours** de développement pour toutes les features !

**Priority Order** :
1. **Pagination + Batch Ops** (essential, affects workflow)
2. **Quick Preview** (high value, user experience)
3. **Auto-Clean** (quick win, improves quality)
4. **Duplicate Finder** (advanced feature, saves space)

---

**END OF PHASE 9 PLAN**
