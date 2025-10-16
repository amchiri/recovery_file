# Exemples d'utilisation du File Recovery Tool

## 1. Lister les disques disponibles

```bash
file_recovery --list
```

Sortie attendue:
```
Available disks:

Device         Volume         Filesystem   Size (GB)  Status
----------------------------------------------------------------
C:             Windows        NTFS         500        Healthy
D:             Data           NTFS         1000       Healthy
\\.\PhysicalDrive0             NTFS         500        Healthy
```

## 2. Scan rapide d'un disque

```bash
file_recovery --scan C: --output recovered/
```

Effectue un scan rapide du disque C: en analysant la MFT (pour NTFS) et sauvegarde les fichiers dans le dossier `recovered/`.

## 3. Scan profond avec types de fichiers spécifiques

```bash
file_recovery --scan D: --mode deep --types jpg,png,pdf --output recovered/
```

Effectue un scan profond (secteur par secteur) du disque D: en recherchant uniquement les fichiers JPG, PNG et PDF.

## 4. Scan complet avec multi-threading

```bash
file_recovery --scan \\.\PhysicalDrive1 --mode complete --threads 8 --output recovered/
```

Utilise tous les modes de scan disponibles avec 8 threads pour une récupération maximale.

## 5. Scan avec filtres de taille

```bash
file_recovery --scan C: --types docx,xlsx,pptx --min-size 10240 --max-size 104857600 --output recovered/
```

Récupère uniquement les documents Office entre 10KB et 100MB.

## 6. Scan sans vérification de signatures (plus rapide)

```bash
file_recovery --scan D: --mode quick --no-verify --output recovered/
```

Désactive la vérification des signatures pour un scan plus rapide (moins fiable).

## 7. Scan d'une partition spécifique

```bash
# Windows
file_recovery --scan \\.\E: --output E:\recovered\

# Linux
sudo ./file_recovery --scan /dev/sdb1 --output /mnt/recovery/
```

## 8. Récupération après formatage rapide

```bash
file_recovery --scan \\.\PhysicalDrive2 --mode deep --deep --threads 4 --output recovered/
```

Le mode deep avec l'option `--deep` active le scan le plus exhaustif possible.

## Scénarios d'utilisation

### Scénario 1: Fichiers supprimés accidentellement (NTFS)

```bash
# 1. Lister les disques
file_recovery --list

# 2. Scan rapide (fichiers récemment supprimés)
file_recovery --scan C: --output recovered/

# 3. Vérifier les résultats
dir recovered\
```

### Scénario 2: Récupération après formatage

```bash
# 1. Scan profond avec signatures
file_recovery --scan D: --mode deep --types jpg,png,mp4,pdf,docx --threads 8 --output recovered/

# 2. Le scan peut prendre plusieurs heures selon la taille du disque
# Progression affichée en temps réel
```

### Scénario 3: Disque endommagé

```bash
# 1. Utiliser le mode signature (ne dépend pas du système de fichiers)
file_recovery --scan \\.\PhysicalDrive3 --mode signature --output recovered/

# 2. Les fichiers seront nommés par leur offset: recovered_12345678.jpg
```

### Scénario 4: Récupération sélective

```bash
# 1. Scan pour identifier les fichiers
file_recovery --scan C: --types pdf,docx > scan_results.txt

# 2. Récupération avec filtres précis
file_recovery --scan C: --types pdf,docx --min-size 100000 --output recovered/
```

## Conseils et bonnes pratiques

### ⚠️ Avant de commencer

1. **NE PAS** écrire sur le disque source
2. **TOUJOURS** récupérer vers un disque différent
3. **FAIRE** un backup si possible avant toute opération
4. **VÉRIFIER** deux fois le chemin du périphérique

### 🚀 Optimisation des performances

- Utilisez `--threads 8` ou plus sur les systèmes multi-cœurs
- Pour les SSD, augmentez le nombre de threads (jusqu'à 16)
- Pour les HDD, limitez à 2-4 threads pour éviter la thrashing
- Utilisez `--quick` pour les fichiers récemment supprimés
- Utilisez `--deep` seulement si nécessaire (très lent)

### 📊 Interpréter les résultats

Le scan affiche:
```
[75%] Scanning sector 1500000 / 2000000 | Elapsed: 5m 23s | ETA: 1m 47s
Files found: 1250
```

- **Progress**: Avancement du scan
- **Files found**: Nombre de fichiers détectés
- **Elapsed**: Temps écoulé
- **ETA**: Temps restant estimé

### 🔍 Types de fichiers supportés

**Images**: jpg, png, gif, bmp, tiff, raw, psd
**Documents**: pdf, doc, docx, xls, xlsx, ppt, pptx, txt
**Archives**: zip, rar, 7z, tar, gz
**Vidéos**: mp4, avi, mkv, mov, wmv
**Audio**: mp3, wav, flac, aac, ogg
**Et plus de 100 autres types...**

## Dépannage

### Problème: "Failed to open device"

**Solution**: Lancez avec les privilèges administrateur

```bash
# Windows (PowerShell admin)
.\file_recovery.exe --scan C: --output recovered\

# Linux
sudo ./file_recovery --scan /dev/sdb --output /mnt/recovery/
```

### Problème: "No files found"

**Essayez**:
1. Mode deep: `--mode deep`
2. Sans filtres de types
3. Vérifier que le disque n'a pas été réécrit

### Problème: Scan très lent

**Solutions**:
- Réduire le nombre de threads
- Vérifier l'état du disque (peut être défaillant)
- Utiliser `--quick` au lieu de `--deep`

### Problème: Fichiers corrompus

**Vérifications**:
- Utiliser `--verify` pour vérifier les signatures
- Certains fichiers peuvent être partiellement écrasés
- Essayer avec des outils de réparation spécifiques au type de fichier

## Support et contribution

Pour plus d'informations, voir README.md et DEVELOPMENT.md
```
