# 🎮 GVAS Tools - Unreal Engine Save File Analysis

This folder contains Python scripts for analyzing and debugging GVAS (Unreal Engine) save files.

## 📜 Scripts

### Main Analysis Tools

- **`analyze_gvas.py`** - Complete GVAS file structure analyzer
  - Parses header, properties, and footer
  - Displays SaveGameClassName
  - Shows file structure breakdown
  - **Usage**: `python analyze_gvas.py <file.sav>`

- **`find_gvas_end.py`** - Detects the end of GVAS files
  - Finds "None" footer pattern (05 00 00 00 4E 6F 6E 65 00)
  - Calculates precise file size
  - Identifies overwritten data
  - **Usage**: `python find_gvas_end.py <file.sav>`

### Validation Tools

- **`check_gvas_size.py`** - Validates GVAS file sizes
  - Compares expected vs actual size
  - Detects truncated files
  - Reports size discrepancies
  - **Usage**: `python check_gvas_size.py <file.sav>`

- **`check_big_file.py`** - Analyzes large recovered files
  - Detects padding and junk data
  - Reports zero-byte percentages
  - Suggests optimal file size
  - **Usage**: `python check_big_file.py <large_file.sav>`

### Quality Testing

- **`test_quality_simple.py`** - Simple quality checker
  - Quick zero-percentage calculation
  - Basic integrity check
  - **Usage**: `python test_quality_simple.py <file.sav>`

- **`test.py`** - General testing script
  - Various GVAS format tests
  - Experimentation sandbox

---

## 🧪 Common Use Cases

### 1. Analyze a recovered .sav file
```bash
python analyze_gvas.py recovered_file.sav
```

### 2. Find the exact end of a GVAS file
```bash
python find_gvas_end.py recovered_file.sav
```

### 3. Check if a recovered file is the right size
```bash
python check_gvas_size.py recovered_file.sav
```

### 4. Analyze a large file with possible padding
```bash
python check_big_file.py large_recovered_file.sav
```

---

## 📚 GVAS Format Overview

### Structure
```
[HEADER]
  - Magic: "GVAS" (47 56 41 53)
  - Engine version
  - Package version
  - SaveGameClassName (FString)

[PROPERTIES]
  - List of properties (key-value pairs)
  - Various data types (Int, Float, String, Array, etc.)

[FOOTER]
  - "None" tag (05 00 00 00 4E 6F 6E 65 00)
  - 4 additional bytes
  - Total: 13 bytes after "None" pattern start
```

### Key Patterns

**Header**: `47 56 41 53` ("GVAS")  
**Footer**: `05 00 00 00 4E 6F 6E 65 00` + 4 bytes

---

## 🎯 Why These Tools?

These scripts were developed during the recovery of **Octopath Traveler 2** save files, where:
- Files were recovered with extra padding (up to 1MB)
- Needed to detect the exact end of valid data
- Required analysis of file structure to determine quality
- Had to distinguish between real data and zero-padding

---

## 🛠️ Requirements

```bash
pip install struct  # Usually built-in
```

---

## 📖 Related Documentation

- [GAME_SAVES_RECOVERY.md](../docs/GAME_SAVES_RECOVERY.md) - Game save recovery guide
- [GVAS_SIGNATURE_ADDED.md](../docs/GVAS_SIGNATURE_ADDED.md) - GVAS format details
- Main C++ implementation: `src/formats/GVASParser.cpp`

---

**Last updated**: October 15, 2025
