# 🧪 Phase 8 - Guide de Test

**Date**: 16 octobre 2025  
**Version**: Phase 8 - Performance Optimizations

## Checklist de Test 📋

### 1. Interface Utilisateur ✅

**Vérifier que les nouveaux contrôles apparaissent** :

```
PERFORMANCE OPTIONS (nouvelle section)
├─ ☑ Smart Scan (Skip Empty Sectors)  ← Coché par défaut
└─ ☐ Large Buffers (1MB blocks)       ← Décoché par défaut
```

**Actions à tester** :
- [ ] La section "PERFORMANCE OPTIONS" est visible
- [ ] Les deux checkboxes sont présentes
- [ ] Smart Scan est coché par défaut (recommandé)
- [ ] Les tooltips apparaissent au survol
- [ ] Les checkboxes répondent aux clics

---

### 2. Panneau de Progression ⏱️

**Vérifier les nouvelles statistiques** :

```
PROGRESS
┌──────────────┬───────────────┬─────────────┬──────────────┐
│ Files Found  │ Bytes Scanned │ Scan Speed  │ Est. Time    │
│ 10           │ 5.2 GB        │ 156.3 MB/s  │ 5:23         │
└──────────────┴───────────────┴─────────────┴──────────────┘

Sectors Skipped (empty): 1,234,567
```

**Actions à tester** :
- [ ] La colonne "Est. Time" affiche "Calculating..." au début
- [ ] Après 5% de progression, affiche le format "X:XX" (minutes:secondes)
- [ ] Le temps restant diminue progressivement
- [ ] Si Smart Scan activé, "Sectors Skipped" apparaît sous les stats
- [ ] Le compteur de secteurs skippés augmente en temps réel
- [ ] Tooltip "Smart Scan skipped these empty sectors" au survol

---

### 3. Test de Performance ⚡

**Test 1 : Smart Scan OFF vs ON**

**Configuration** :
- Disque : C:
- Extensions : sav (1 seule extension pour test rapide)
- Dossier spécifique : ☑ Activé
- Path : `C:\Users\*\AppData` (zone avec beaucoup d'espace vide)
- Threads : 4

**Procédure** :

1. **Premier test - Smart Scan OFF** :
   ```
   ☐ Smart Scan (Skip Empty Sectors)  ← Décocher
   ☐ Large Buffers
   ```
   - Lancer le scan
   - Noter le temps pris
   - Noter la vitesse moyenne (MB/s)

2. **Deuxième test - Smart Scan ON** :
   ```
   ☑ Smart Scan (Skip Empty Sectors)  ← Cocher
   ☐ Large Buffers
   ```
   - Lancer le scan
   - Noter le temps pris
   - Noter la vitesse moyenne (MB/s)
   - Noter les secteurs skippés

**Résultats attendus** :
- Smart Scan ON devrait être **50-80% plus rapide**
- Le compteur "Sectors Skipped" devrait être élevé (>500,000)
- La vitesse de scan devrait être 2-4x supérieure

---

**Test 2 : Large Buffers OFF vs ON**

**Configuration** :
- Disque : C:
- Extensions : sav
- Smart Scan : ☑ ON (pour tester les deux ensemble)
- Threads : 4

**Procédure** :

1. **Premier test - Small Buffers** :
   ```
   ☑ Smart Scan
   ☐ Large Buffers  ← Décocher
   ```
   - Lancer le scan
   - Noter la vitesse (MB/s)

2. **Deuxième test - Large Buffers** :
   ```
   ☑ Smart Scan
   ☑ Large Buffers  ← Cocher
   ```
   - Lancer le scan
   - Noter la vitesse (MB/s)

**Résultats attendus** :
- Large Buffers ON devrait être **15-25% plus rapide**
- Vitesse devrait passer de ~150 MB/s à ~200 MB/s

---

### 4. Test de Stabilité 🛡️

**Actions à tester** :
- [ ] Activer/désactiver Smart Scan pendant un scan → Scan continue
- [ ] Activer/désactiver Large Buffers pendant un scan → Scan continue
- [ ] Cliquer rapidement sur les checkboxes → Pas de crash
- [ ] Lancer plusieurs scans de suite → Pas de fuite mémoire
- [ ] Appuyer sur STOP pendant un scan → Arrêt propre

---

### 5. Test ETA (Estimation de Temps) ⏰

**Vérifier la précision** :

1. Lancer un scan avec Smart Scan activé
2. Attendre que l'ETA s'affiche (après ~5% de progression)
3. Noter l'ETA affiché : par exemple "8:30" (8 min 30 sec)
4. Attendre la fin du scan
5. Comparer le temps réel avec l'ETA initial

**Résultats attendus** :
- ETA devrait être précis à ±10%
- Si ETA dit "8:30", le scan devrait finir entre 7:40 et 9:20
- L'ETA devrait se raffiner au fil du temps

---

### 6. Test de Compteur de Secteurs Skippés 📊

**Configuration pour maximiser les secteurs skippés** :
- Disque : Récemment formaté ou peu utilisé
- Smart Scan : ☑ ON
- Scan complet (pas de dossier spécifique)

**Actions** :
- [ ] Le compteur démarre à 0
- [ ] Le compteur augmente rapidement dans les zones vides
- [ ] Le format est lisible (ex: "1,234,567" avec virgules)
- [ ] La couleur est verte (pour indiquer performance positive)
- [ ] Le tooltip explique ce que c'est

---

### 7. Test des Filtres et Tri (Phase 7 + 8) 🔍

**Vérifier que Phase 7 fonctionne toujours** :

- [ ] Barre de recherche filtre correctement
- [ ] Filtres par qualité fonctionnent
- [ ] Tri par colonnes fonctionne
- [ ] "Showing X of Y files" s'affiche
- [ ] Double-clic ouvre les fichiers
- [ ] Menu contextuel (clic droit) fonctionne

---

## Résultats Attendus 🎯

### Performance Globale :

**Sans optimisations** (Phase 7) :
```
Scan de 100 GB avec 40% d'espace libre
Temps : ~50 minutes
Vitesse : ~35 MB/s
Secteurs lus : 200,000,000
```

**Avec Smart Scan** (Phase 8) :
```
Scan de 100 GB avec 40% d'espace libre
Temps : ~15 minutes (70% plus rapide ⚡)
Vitesse : ~110 MB/s
Secteurs lus : 120,000,000
Secteurs skippés : 80,000,000 (40% du disque)
```

**Avec Smart Scan + Large Buffers** :
```
Scan de 100 GB avec 40% d'espace libre
Temps : ~12 minutes (75% plus rapide ⚡)
Vitesse : ~140 MB/s
Secteurs lus : 120,000,000
Secteurs skippés : 80,000,000
```

---

## Problèmes Connus 🐛

### Aucun pour l'instant ! ✅

Si vous rencontrez des problèmes :

1. **ETA affiche "Calculating..." pendant tout le scan**
   - Vérifier que `scanStartTime_` est bien initialisé
   - Vérifier que `currentProgress > 0.05f`

2. **Secteurs Skippés = 0 même avec Smart Scan**
   - Vérifier que le disque n'est pas complètement plein
   - Tester sur un disque avec beaucoup d'espace libre

3. **Crash avec Large Buffers**
   - RAM insuffisante (< 4 GB)
   - Désactiver Large Buffers

---

## Commandes Utiles 🔧

### Recompiler après modifications :
```powershell
cd c:\Users\polom\recovery_file\build
cmake --build . --config Release
```

### Lancer avec admin :
```powershell
Start-Process ".\build\Release\file_recovery_gui.exe" -Verb RunAs
```

### Tuer le processus si bloqué :
```powershell
taskkill /F /IM file_recovery_gui.exe
```

---

## Rapport de Test 📝

**Date du test** : ___________  
**Testeur** : ___________

### Résultats :

| Feature                    | Status | Notes                |
|---------------------------|--------|----------------------|
| Smart Scan UI             | ☐      |                      |
| Large Buffers UI          | ☐      |                      |
| ETA Display               | ☐      |                      |
| Sectors Skipped Counter   | ☐      |                      |
| Smart Scan Performance    | ☐      | Gain: _____% |
| Large Buffers Performance | ☐      | Gain: _____% |
| ETA Accuracy              | ☐      | Écart: _____% |
| Stabilité                 | ☐      |                      |
| Pas de régression Phase 7 | ☐      |                      |

### Bugs trouvés :
```
(Aucun ou liste ici)
```

### Suggestions :
```
(Idées d'améliorations)
```

---

## Conclusion 🎉

Si tous les tests passent :
✅ **Phase 8 est validée et prête pour production !**

Prochaine étape : **Phase 9 - Export & Profiles**

---

*Bon test ! ⚡*
