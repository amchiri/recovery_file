# 🚀 START PHASE 9 - Guide de Démarrage

**Date** : Octobre 2025  
**Status** : ✅ READY TO START  
**Phase précédente** : Phase 8 complétée (100%)  

---

## 📋 Documents Importants

| Document | Description | Lignes |
|----------|-------------|--------|
| `PHASE9_PLAN.md` | Plan complet avec code structure | 900+ |
| `PHASE9_SUMMARY.md` | Résumé exécutif (quick read) | 200 |
| `ROADMAP.md` | Roadmap mis à jour | 328 |

**👉 Commencer par lire** : `PHASE9_SUMMARY.md` (5 minutes) puis `PHASE9_PLAN.md` (détails)

---

## 🎯 Rappel : Les 5 Features

1. **Pagination** 📄 - Navigation par pages de 100 fichiers
2. **Quick Preview** 👁️ - Aperçu images/text/hex
3. **Batch Operations** 📦 - Sélection multiple et actions groupées
4. **Auto-Clean** 🗑️ - Nettoyage automatique Dead/Corrupted
5. **Duplicate Finder** 🔍 - Détection et suppression doublons (MD5)

---

## ⏱️ Sprint 1 : Pagination + Batch Operations (4-6h)

### Étape 1 : Créer `ResultsPagination` class (1h)

**Fichier** : `src/gui/ResultsPagination.h`

```cpp
#pragma once
#include <cstddef>

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

**Fichier** : `src/gui/ResultsPagination.cpp`

```cpp
#include "ResultsPagination.h"
#include <imgui.h>
#include <algorithm>

ResultsPagination::ResultsPagination(size_t itemsPerPage) 
    : itemsPerPage_(itemsPerPage) {}

void ResultsPagination::setTotalItems(size_t total) {
    totalItems_ = total;
    currentPage_ = std::min(currentPage_, getTotalPages() - 1);
}

size_t ResultsPagination::getEndIndex() const {
    return std::min(getStartIndex() + itemsPerPage_, totalItems_);
}

size_t ResultsPagination::getTotalPages() const {
    if (totalItems_ == 0) return 1;
    return (totalItems_ + itemsPerPage_ - 1) / itemsPerPage_;
}

void ResultsPagination::nextPage() {
    if (currentPage_ < getTotalPages() - 1) {
        currentPage_++;
    }
}

void ResultsPagination::prevPage() {
    if (currentPage_ > 0) {
        currentPage_--;
    }
}

void ResultsPagination::goToPage(size_t page) {
    currentPage_ = std::min(page, getTotalPages() - 1);
}

void ResultsPagination::render() {
    size_t totalPages = getTotalPages();
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Previous button
    if (currentPage_ == 0) {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button("◀ Previous")) {
        prevPage();
    }
    if (currentPage_ == 0) {
        ImGui::EndDisabled();
    }
    
    ImGui::SameLine();
    
    // Page numbers (show 5 pages max)
    size_t startPage = (currentPage_ > 2) ? currentPage_ - 2 : 0;
    size_t endPage = std::min(startPage + 5, totalPages);
    
    for (size_t i = startPage; i < endPage; i++) {
        if (i == currentPage_) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.7f, 1.0f, 1.0f));
        }
        
        std::string label = std::to_string(i + 1);
        if (ImGui::Button(label.c_str())) {
            goToPage(i);
        }
        
        if (i == currentPage_) {
            ImGui::PopStyleColor();
        }
        
        ImGui::SameLine();
    }
    
    // Next button
    if (currentPage_ >= totalPages - 1) {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button("Next ▶")) {
        nextPage();
    }
    if (currentPage_ >= totalPages - 1) {
        ImGui::EndDisabled();
    }
    
    ImGui::SameLine();
    ImGui::Text("(Page %zu / %zu)", currentPage_ + 1, totalPages);
    
    ImGui::Spacing();
}
```

**✅ Test** : Créer instance, setTotalItems(1000), vérifier getTotalPages() == 10

---

### Étape 2 : Créer `BatchOperations` class (1h)

**Fichier** : `src/gui/BatchOperations.h`

```cpp
#pragma once
#include <unordered_set>
#include <vector>
#include <cstddef>
#include "../core/RecoveredFileInfo.h"

class BatchOperations {
public:
    void renderBatchControls();
    void toggleSelection(size_t index);
    void selectAll();
    void deselectAll();
    
    bool isSelected(size_t index) const;
    size_t getSelectedCount() const { return selectedFiles_.size(); }
    std::vector<size_t> getSelectedIndices() const;
    
    void deleteSelected(const std::vector<RecoveredFileInfo>& files);
    void copySelectedTo(const std::vector<RecoveredFileInfo>& files, const std::string& destPath);
    void openSelected(const std::vector<RecoveredFileInfo>& files);
    
private:
    std::unordered_set<size_t> selectedFiles_;
};
```

**✅ Test** : Sélectionner 10 fichiers, vérifier getSelectedCount() == 10

---

### Étape 3 : Intégrer dans RecoveryGUI (2h)

**Modifier** : `src/gui/RecoveryGUI.h`

Ajouter members :
```cpp
#include "ResultsPagination.h"
#include "BatchOperations.h"

class RecoveryGUI {
    // ...
private:
    ResultsPagination pagination_;
    BatchOperations batchOps_;
};
```

**Modifier** : `src/gui/RecoveryGUI.cpp`

Dans `renderResultsPanel()`, remplacer :
```cpp
// OLD
for (size_t i = 0; i < recoveredFiles_.size(); i++) {
    // ...
}

// NEW
pagination_.setTotalItems(recoveredFiles_.size());
size_t start = pagination_.getStartIndex();
size_t end = pagination_.getEndIndex();

for (size_t i = start; i < end; i++) {
    // ... (ajouter checkbox au début)
    ImGui::Checkbox(("##select" + std::to_string(i)).c_str(), &selected);
    if (ImGui::IsItemClicked()) {
        batchOps_.toggleSelection(i);
    }
    ImGui::SameLine();
    // ... (reste du code)
}

pagination_.render();
```

**✅ Test** : Lancer l'app avec 1000 fichiers, vérifier que seuls 100 s'affichent

---

### Étape 4 : Ajouter Batch Controls (1h)

Dans `RecoveryGUI.cpp`, ajouter avant le tableau :

```cpp
batchOps_.renderBatchControls();
ImGui::Spacing();
```

Implémenter dans `BatchOperations.cpp` :

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
    
    if (selected > 0) {
        ImGui::SameLine();
        if (ImGui::Button("Delete Selected")) {
            // Show confirmation dialog
            ImGui::OpenPopup("Confirm Delete");
        }
        ImGui::SameLine();
        if (ImGui::Button("Copy All To...")) {
            // Open folder picker
        }
    }
}
```

**✅ Test** : Sélectionner 50 fichiers, cliquer "Delete Selected", confirmer suppression

---

### Étape 5 : Compilation et Tests (1h)

**CMakeLists.txt** :
```cmake
add_executable(RecoveryGUI
    # ... existing files ...
    src/gui/ResultsPagination.cpp
    src/gui/BatchOperations.cpp
)
```

**Compiler** :
```powershell
cd c:\Users\polom\recovery_file
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

**Lancer** :
```powershell
Start-Process ".\Release\RecoveryGUI.exe" -Verb RunAs
```

**Tests à faire** :
- [ ] Affiche 100 fichiers par page
- [ ] Navigation Previous/Next fonctionne
- [ ] Numéros de page cliquables
- [ ] Checkbox sélection fonctionne
- [ ] "Select All" sélectionne 100 fichiers (page courante)
- [ ] "Delete Selected" supprime avec confirmation

---

## 📊 Validation Sprint 1

Sprint 1 est validé si :
- [x] Pagination fonctionne avec 1000+ fichiers
- [x] Changement de page < 100ms
- [x] Checkbox sélection fonctionne
- [x] Boutons batch operations présents
- [x] Peut sélectionner 50 fichiers et les supprimer

**Temps estimé : 4-6 heures**

---

## 🔜 Après Sprint 1 : Sprint 2 (Quick Preview)

1. Télécharger `stb_image.h` depuis https://github.com/nothings/stb
2. Créer `PreviewPanel` class
3. Implémenter ImagePreview, TextPreview, HexDump
4. Intégrer dans RecoveryGUI

**Voir détails** : `PHASE9_PLAN.md` Section 2.B

---

## 💡 Tips

### Debugging
- Utiliser `ImGui::ShowDemoWindow()` pour référence
- Ajouter logs : `std::cout << "Page: " << currentPage_ << std::endl;`
- Vérifier indices : start < end < totalItems

### Performance
- Pagination est O(1) (pas de copie de données)
- Utiliser références pour éviter copies : `const auto& file = recoveredFiles_[i];`
- Checkbox rendering ~0.01ms par fichier

### UI/UX
- Couleur bleue pour page active : `ImVec4(0.3f, 0.7f, 1.0f, 1.0f)`
- Disabled buttons grisés automatiquement par ImGui
- Tooltips : `if (ImGui::IsItemHovered()) ImGui::SetTooltip("...");`

---

**Prêt ? C'est parti ! 🚀**

**Commande pour démarrer** :
```powershell
cd c:\Users\polom\recovery_file
code .
```

Ouvre Visual Studio Code et commence par créer `src/gui/ResultsPagination.h` !
