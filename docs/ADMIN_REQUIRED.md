# ⚠️ ERREUR : Accès Refusé (Win32 Error: 5)

## 🔍 Problème

L'application ne peut pas accéder au disque **C:** car elle nécessite les **privilèges administrateur**.

**Erreur affichée** :
```
[ERROR] Failed to open device: C: - Win32 Error: 5
```

**Win32 Error 5** = `ACCESS_DENIED` (Accès refusé)

## 💡 Pourquoi ?

Sur Windows, l'accès direct aux disques physiques (via `\\.\C:`) nécessite les droits administrateur pour des raisons de sécurité. C'est normal et attendu.

## ✅ Solution

### 🚀 Méthode 1 : Utiliser le script de lancement automatique

**Double-cliquez sur** :
```
launch_gui_admin.bat
```

Ce script :
1. Vérifie si vous êtes déjà admin
2. Si non, demande automatiquement les privilèges admin
3. Lance l'interface graphique

### 🔧 Méthode 2 : Lancer manuellement en admin

**Windows Explorer** :
1. Naviguez vers `build\Release\`
2. **Clic droit** sur `file_recovery_gui.exe`
3. Sélectionnez **"Exécuter en tant qu'administrateur"**
4. Cliquez **"Oui"** dans la fenêtre UAC

**PowerShell** :
```powershell
Start-Process '.\build\Release\file_recovery_gui.exe' -Verb RunAs
```

### 🎯 Méthode 3 : Scanner un autre disque

Si vous ne voulez pas utiliser les droits admin, scannez un autre disque :
- **E:** (disque externe/USB) - Ne nécessite généralement PAS les droits admin
- **D:** (partition secondaire)
- Etc.

Pour scanner C:, les droits admin sont **obligatoires**.

## 🔒 Sécurité

**Est-ce sûr de donner les droits admin ?**

✅ **OUI**, car :
- Le code est open-source (vous pouvez le vérifier)
- L'application a seulement besoin de **LIRE** le disque (pas d'écriture)
- C'est une exigence Windows standard pour tout outil de récupération

**Ce que fait l'application** :
- ✅ Lit les secteurs du disque
- ✅ Cherche des signatures de fichiers
- ✅ Extrait les fichiers trouvés
- ❌ N'écrit RIEN sur le disque source
- ❌ Ne modifie RIEN sur le système

## 📝 Note pour les développeurs

Si vous développez et testez, vous pouvez aussi :

### Créer un raccourci permanent en admin :
1. Clic droit sur `file_recovery_gui.exe` → **Créer un raccourci**
2. Clic droit sur le raccourci → **Propriétés**
3. Onglet **Raccourci** → **Avancé...**
4. Cocher **"Exécuter en tant qu'administrateur"**
5. **OK** → **Appliquer** → **OK**

### Désactiver UAC (NON recommandé) :
- Tapez `UAC` dans la recherche Windows
- Déplacez le curseur vers le bas
- **Attention** : réduit la sécurité de Windows

## 🎮 Test sur disque USB

Si vous voulez tester **sans droits admin** :
1. Branchez une clé USB ou disque externe
2. Lancez l'interface **normalement** (pas en admin)
3. Sélectionnez le disque USB (ex: E:, F:, etc.)
4. Le scan devrait fonctionner sans problème

## 📊 Logs détaillés

Les logs montrent exactement ce qui s'est passé :
```
[INFO ] Converting drive letter to device path: C: -> \\.\C:
[ERROR] Failed to open device: C: - Win32 Error: 5
[ERROR] Failed to open device for scanning
```

L'application a bien essayé de convertir `C:` en `\\.\C:` (format Windows pour accès physique), mais l'ouverture a échoué avec l'erreur 5 (ACCESS_DENIED).

## 🔗 Références

- [Windows Device Paths](https://docs.microsoft.com/en-us/windows/win32/fileio/naming-a-file#win32-device-namespaces)
- [CreateFile function](https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea)
- [UAC (User Account Control)](https://docs.microsoft.com/en-us/windows/security/identity-protection/user-account-control/user-account-control-overview)

---

**TL;DR** : Double-cliquez sur `launch_gui_admin.bat` ou lancez l'exe en tant qu'administrateur. C'est normal et sûr. 🚀
