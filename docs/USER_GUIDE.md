# File Recovery Tool - User Guide

**Version:** 1.0.0
**For:** End Users
**Platform:** Windows, Linux, macOS

---

## Table of Contents

1. [Introduction](#introduction)
2. [Installation](#installation)
3. [Quick Start](#quick-start)
4. [Using the GUI](#using-the-gui)
5. [Using the Command Line](#using-the-command-line)
6. [Configuration](#configuration)
7. [Recovery Modes](#recovery-modes)
8. [Advanced Features](#advanced-features)
9. [File Quality Indicators](#file-quality-indicators)
10. [Troubleshooting](#troubleshooting)
11. [FAQ](#faq)
12. [Tips & Best Practices](#tips--best-practices)

---

## Introduction

### What is File Recovery Tool?

File Recovery Tool is a powerful application that can **recover deleted files** from your hard drives, USB drives, SD cards, and other storage devices. Whether you accidentally deleted important photos, documents, or videos, this tool can help you get them back.

### Key Features

✅ **Two Recovery Modes:**
- **Quick Scan:** Fast recovery using filesystem metadata (for recently deleted files)
- **Deep Scan:** Comprehensive sector-by-sector recovery (for older deletions)

✅ **70+ File Types Supported:**
- Images: JPEG, PNG, GIF, BMP, TIFF, PSD, RAW photos (CR2, NEF), and more
- Videos: MP4, AVI, MKV, MOV, WMV, and more
- Audio: MP3, WAV, FLAC, OGG, and more
- Documents: PDF, Word (DOC/DOCX), Excel, PowerPoint, and more
- Archives: ZIP, RAR, 7Z, ISO
- Executables, Databases, 3D files, Crypto wallets

✅ **Intelligent Quality Analysis:**
- Automatically detects corrupted or incomplete files
- Rates file quality: Excellent, Good, Poor, Corrupted, or Dead
- Saves you time by filtering out unrecoverable files

✅ **Advanced Features:**
- **File Preview:** See text, hex, or image previews before recovering
- **Duplicate Detection:** Find and remove duplicate recovered files
- **Fragmented File Recovery:** Reconstruct files split across disk sectors
- **Multi-threaded Scanning:** Fast recovery using all CPU cores

---

## Installation

### Windows

**Prerequisites:**
- Windows 10/11 (64-bit)
- Administrator privileges

**Steps:**

1. **Download** the latest release from [GitHub Releases](https://github.com/yourusername/recovery_file/releases)

2. **Extract** the ZIP file to a folder (e.g., `C:\FileRecovery`)

3. **Run as Administrator:**
   - Right-click `file_recovery_gui.exe`
   - Select "Run as administrator"

**Why administrator?** Direct disk access requires elevated privileges to read raw disk sectors.

### Linux (Ubuntu/Debian)

**Prerequisites:**
- Ubuntu 20.04+ or Debian 11+
- Root access

**Steps:**

```bash
# Download and extract
wget https://github.com/yourusername/recovery_file/releases/latest/download/file_recovery_linux.tar.gz
tar -xzf file_recovery_linux.tar.gz
cd file_recovery

# Make executable
chmod +x file_recovery_gui

# Run with root privileges
sudo ./file_recovery_gui
```

### macOS

**Prerequisites:**
- macOS 11 (Big Sur) or later
- Administrator password

**Steps:**

```bash
# Download and extract
curl -L https://github.com/yourusername/recovery_file/releases/latest/download/file_recovery_macos.tar.gz -o file_recovery_macos.tar.gz
tar -xzf file_recovery_macos.tar.gz
cd file_recovery

# Run with sudo
sudo ./file_recovery_gui
```

---

## Quick Start

### 5-Minute Recovery Guide

**Goal:** Recover deleted photos from a USB drive

**Steps:**

1. **Launch the application** (as administrator)

2. **Select your USB drive** from the dropdown menu
   - Example: "E:\ - USB Drive (16GB)"

3. **Choose file types** to recover
   - Check: `jpg`, `png`, `gif`
   - Uncheck others for faster scanning

4. **Set output directory**
   - Example: `C:\RecoveredFiles`

5. **Start Quick Scan** (for recently deleted files)
   - Click "Start Quick Scan" button
   - Wait for scan to complete (1-5 minutes)

6. **Review results**
   - Files are listed with quality indicators
   - Green = Excellent, Yellow = Good, Red = Poor/Corrupted

7. **Recover files**
   - Files are automatically saved to output directory
   - Open folder to view recovered files

🎉 **Done!** Your recovered files are in `C:\RecoveredFiles`

---

## Using the GUI

### Main Window Overview

```
┌────────────────────────────────────────────────────────────┐
│ FILE RECOVERY TOOL                    Modern Edition v1.0  │
├────────────────────────────────────────────────────────────┤
│                                                            │
│ ┌──────────────────┐  ┌─────────────────────────────────┐ │
│ │                  │  │ PROGRESS                        │ │
│ │  CONFIGURATION   │  │                                 │ │
│ │                  │  │ Status: Scanning...             │ │
│ │ • Disk Selection │  │ Progress: ████████░░ 45%        │ │
│ │ • File Types     │  │ Files Found: 127                │ │
│ │ • Output Path    │  │ Speed: 250 MB/s                 │ │
│ │ • Scan Options   │  │                                 │ │
│ │                  │  └─────────────────────────────────┘ │
│ │                  │                                       │
│ │ [Quick Scan]     │  ┌─────────────────────────────────┐ │
│ │ [Deep Scan]      │  │ RECOVERED FILES                 │ │
│ │ [Stop]           │  │                                 │ │
│ │                  │  │ ☑ Filename    Size    Quality   │ │
│ │                  │  │ ─────────────────────────────── │ │
│ │                  │  │ ☑ photo1.jpg  2.5MB   ★★★★★    │ │
│ │                  │  │ ☑ doc.pdf     1.2MB   ★★★★☆    │ │
│ │                  │  │ ☐ video.mp4   45MB    ★★☆☆☆    │ │
│ │                  │  │ ...                             │ │
│ │                  │  │                                 │ │
│ │                  │  │ [Preview] [Find Duplicates]     │ │
│ │                  │  └─────────────────────────────────┘ │
│ └──────────────────┘                                      │
└────────────────────────────────────────────────────────────┘
```

### Configuration Panel

#### 1. Disk Selection

**Dropdown:** Shows all available disks

```
Disk: [PhysicalDrive0 - SSD (500GB)    ▼]

Options:
- PhysicalDrive0 - SSD (500GB)
- PhysicalDrive1 - HDD (2TB)
- E:\ - USB Drive (16GB)
- F:\ - SD Card (32GB)
```

**Tip:** Select the disk where deleted files were located.

#### 2. File Type Filters

**Checkboxes:** Select file types to recover

```
☑ jpg     - JPEG Images
☑ png     - PNG Images
☐ pdf     - PDF Documents
☑ docx    - Word Documents
☐ mp4     - Video Files
☑ mp3     - Audio Files
```

**Tip:** Selecting fewer types = faster scanning

#### 3. Output Directory

**Path:** Where recovered files will be saved

```
Output: [C:\RecoveredFiles               ] [Browse...]
```

**⚠ Important:** Choose a different drive than the one being scanned!
- ❌ BAD: Scanning C:\, saving to C:\Recovered
- ✅ GOOD: Scanning C:\, saving to D:\Recovered

#### 4. Advanced Options

```
☑ Deep Scan           - Sector-by-sector recovery
☑ Smart Scan          - Skip empty sectors (faster)
☑ Large Buffers       - Use 16MB buffers (faster, more RAM)
☐ Scan Specific Folder - Scan only a folder instead of entire disk
```

**Thread Count:** [4  ▼] (Number of CPU cores to use)

### Progress Panel

**Displays real-time recovery statistics:**

```
Status: Scanning...
Progress: ████████████████░░░░ 80% (400GB / 500GB)
Files Found: 1,247
Speed: 320 MB/s
Time Elapsed: 00:25:43
Estimated Remaining: 00:06:25
Sectors Skipped: 15,234,567 (Smart Scan enabled)
```

### Results Panel

#### File List

**Columns:**
- **☑** - Checkbox (for batch operations)
- **Filename** - Recovered file name
- **Size** - File size in MB/GB
- **Quality** - Star rating (★★★★★ = Excellent)
- **Zeros** - Percentage of zero bytes (lower = better)

**Filters:**

```
Search: [photo        ] 🔍

Quality Filters:
☑ Excellent  ☑ Good  ☑ Poor  ☐ Corrupted  ☐ Dead
```

**Showing 43 of 1,247 files**

#### Context Menu (Right-Click)

```
┌─────────────────────┐
│ Open File           │
│ Open in Explorer    │
│ Copy Path           │
│ ──────────────────  │
│ Delete File         │
└─────────────────────┘
```

**Double-click** a file to open it with default application.

### Advanced Features (Buttons)

#### 🔍 Preview Selected

**Opens preview window:**

```
┌───────────────────────────────────────┐
│ File Preview                      [X] │
├───────────────────────────────────────┤
│ File: photo.jpg                       │
│ Size: 2.5 MB                          │
│ Path: C:\Recovered\photo.jpg          │
│ ───────────────────────────────────   │
│ [IMAGE PREVIEW]                       │
│                                       │
│ Dimensions: 1920x1080                 │
│ Thumbnail: [Placeholder]              │
│                                       │
│ Note: Full image decoding coming soon │
│                                       │
│                      [Close]          │
└───────────────────────────────────────┘
```

**Supports:**
- **Images:** Thumbnail preview
- **Text Files:** First 100 lines
- **Other Files:** Hexadecimal dump

#### 🔎 Find Duplicates

**Analyzes recovered files for duplicates:**

```
┌─────────────────────────────────────────────┐
│ Duplicate File Detector               [X]  │
├─────────────────────────────────────────────┤
│ Total Files: 1,247                          │
│ Unique: 1,180   Duplicates: 67             │
│ Wasted Space: 234.5 MB                      │
│ ─────────────────────────────────────────   │
│ Found 23 groups of duplicate files:         │
│                                             │
│ ▼ Group 1 - 3 duplicates (SHA256: a4f3...) │
│   ✓ KEEP    photo1.jpg (2.5 MB) Q: 98%     │
│   ✗ DELETE  photo1_copy.jpg (2.5 MB) Q: 87%│
│   ✗ DELETE  photo1_old.jpg (2.5 MB) Q: 72% │
│                                             │
│ ▶ Group 2 - 2 duplicates (SHA256: b7e9...) │
│ ▶ Group 3 - 5 duplicates (SHA256: c2a1...) │
│                                             │
│ [Delete All Lower-Quality Duplicates]       │
│                                             │
│                                    [Close]  │
└─────────────────────────────────────────────┘
```

**How it works:**
1. Calculates SHA256 hash of each file
2. Groups files with identical hashes
3. Selects best quality file to keep
4. Marks others for deletion

---

## Using the Command Line

### Basic Usage

```bash
# Windows (PowerShell as Admin)
.\file_recovery.exe --disk \\.\PhysicalDrive1 --output D:\Recovered

# Linux (with sudo)
sudo ./file_recovery --disk /dev/sdb --output /home/user/recovered

# macOS
sudo ./file_recovery --disk /dev/disk2 --output ~/recovered
```

### Command-Line Options

```
Usage: file_recovery [OPTIONS]

Required:
  --disk <path>       Disk or partition to scan
  --output <path>     Output directory for recovered files

Optional:
  --quick             Use Quick Scan (default: Deep Scan)
  --types <list>      Comma-separated file types (e.g., jpg,png,pdf)
  --threads <n>       Number of threads (default: 4)
  --quality <n>       Minimum quality threshold 0-100 (default: 60)
  --no-smart-scan     Disable smart scan optimization
  --folder <path>     Scan specific folder instead of entire disk

Examples:
  # Recover all JPG and PNG files
  file_recovery --disk E: --output D:\Photos --types jpg,png

  # Quick scan with 8 threads
  file_recovery --disk /dev/sdb --output ~/recovered --quick --threads 8

  # Deep scan with quality filter
  file_recovery --disk \\.\PhysicalDrive1 --output D:\Recovered --quality 80
```

### Batch Quality Analyzer

**Analyze quality of recovered files without re-scanning:**

```bash
# Analyze all files in directory
.\batch_analyzer.exe --input D:\Recovered --output quality_report.txt

# Analyze specific file types
.\batch_analyzer.exe --input D:\Recovered --types jpg,png --output photo_quality.txt
```

**Output format:**

```
FILE QUALITY REPORT
==================

Total Files Analyzed: 1,247
Excellent: 823 (66.0%)
Good: 287 (23.0%)
Poor: 94 (7.5%)
Corrupted: 38 (3.0%)
Dead: 5 (0.4%)

Detailed Results:
-----------------
photo1.jpg - EXCELLENT (Entropy: 7.2, Structure: Valid)
document.pdf - GOOD (Entropy: 6.8, Structure: Valid, Footer: Missing)
video.mp4 - POOR (Entropy: 3.4, Structure: Invalid)
...
```

---

## Configuration

### config.ini File

**Location:** Same folder as executable

**Structure:**

```ini
# File Recovery Tool Configuration

[Paths]
# Default output directory for recovered files
output_directory = .\recovered_files

# Signature database file (optional)
signature_file = signatures.txt

[Performance]
# Number of threads for scanning (0 = auto-detect)
thread_count = 4

# Enable deep scan by default
deep_scan = true

# Buffer size for disk I/O in MB (higher = faster but more RAM)
buffer_size_mb = 16

[Scanning]
# Enable smart scan optimization (skip empty sectors)
smart_scan = true

# Enable large buffer optimization
large_buffers = true

[FileTypes]
# Default file types to recover (comma-separated)
# Leave empty to recover all types
default_types = jpg,png,pdf,docx,mp4,zip

[GUI]
# Window dimensions
window_width = 1280
window_height = 720

# Enable dark theme
dark_theme = true

[Logging]
# Log level: DEBUG, INFO, WARNING, ERROR
log_level = INFO

# Log file path
log_file = recovery_log.txt

# Enable console output
console_output = true
```

### Customizing Settings

**Edit `config.ini` with any text editor:**

```bash
# Windows
notepad config.ini

# Linux/macOS
nano config.ini
```

**Restart application** for changes to take effect.

---

## Recovery Modes

### Quick Scan

**When to use:**
- File was deleted recently (within days/weeks)
- Filesystem was not formatted
- No new files written to disk after deletion

**How it works:**
- Reads Master File Table (MFT) on NTFS
- Reads File Allocation Table (FAT) on FAT32
- Extracts metadata of deleted files
- Reconstructs files from existing data

**Advantages:**
✅ Very fast (10-100x faster than Deep Scan)
✅ Preserves original filenames
✅ Preserves timestamps (created, modified, accessed)

**Limitations:**
❌ Only works if MFT/FAT entry exists
❌ Cannot recover overwritten files
❌ Filesystem-dependent (NTFS/FAT32 only)

**Speed:** 10-50 GB/s (memory-limited)

**Example:** Accidentally deleted photos from Camera folder 2 days ago.

---

### Deep Scan

**When to use:**
- File was deleted long ago (months/years)
- Disk was formatted
- MFT/FAT entry was overwritten
- Quick Scan found nothing

**How it works:**
- Reads disk sector-by-sector (512-byte blocks)
- Searches for file signatures (magic bytes)
- When signature found:
  - Identifies file type (JPEG, PDF, etc.)
  - Estimates file size
  - Extracts file data
  - Analyzes quality
- Saves high-quality files

**Advantages:**
✅ Can recover very old files
✅ Works after formatting
✅ Filesystem-independent
✅ Recovers 70+ file types

**Limitations:**
❌ Slower than Quick Scan
❌ Filenames are lost (generates new names)
❌ Timestamps are lost
❌ May find partial/corrupted files

**Speed:** 100-500 MB/s (disk-limited)

**Example:** Formatted USB drive last month, need to recover old documents.

---

## File Quality Indicators

### Quality Levels

The tool rates every recovered file using intelligent analysis:

| Level | Stars | Meaning | Recommendation |
|-------|-------|---------|----------------|
| **EXCELLENT** | ★★★★★ | Perfect file, all checks passed | ✅ Definitely use |
| **GOOD** | ★★★★☆ | Minor issues, fully usable | ✅ Probably use |
| **POOR** | ★★★☆☆ | Partial corruption, may be usable | ⚠️ Check before using |
| **CORRUPTED** | ★★☆☆☆ | Heavy corruption, limited usability | ⚠️ Probably unusable |
| **DEAD** | ★☆☆☆☆ | Completely unusable | ❌ Don't use |

### How Quality is Determined

The tool analyzes multiple factors:

#### 1. **Header Validation**
- Checks if file starts with correct magic bytes
- Example: JPEG must start with `FF D8 FF`

#### 2. **Footer Validation**
- Checks if file ends with correct end marker
- Example: JPEG must end with `FF D9`

#### 3. **Entropy Analysis**
- Measures data randomness (0.0 - 8.0)
- **Low entropy** (0-2): Mostly zeros/repeated patterns → Likely overwritten
- **Normal entropy** (3-7.5): Normal file data → Good
- **High entropy** (7.5-8): Random garbage OR encrypted/compressed → May be bad

#### 4. **Structural Validation**
- Verifies internal file structure
- Example: PDF must have valid object references

#### 5. **Data Consistency**
- Checks if data makes sense for file type
- Example: JPEG should have expected marker spacing

### Examples

```
photo.jpg - EXCELLENT ★★★★★
├─ Header: ✓ Valid (FF D8 FF E0)
├─ Footer: ✓ Valid (FF D9)
├─ Entropy: 7.2 (Normal range)
├─ Structure: ✓ Valid JPEG markers
└─ Consistency: 98%

document.pdf - GOOD ★★★★☆
├─ Header: ✓ Valid (%PDF-1.4)
├─ Footer: ✗ Missing (%%EOF)
├─ Entropy: 6.8 (Normal range)
├─ Structure: ✓ Valid objects
└─ Consistency: 87%
Recommendation: File is truncated but readable

corrupted.mp4 - POOR ★★★☆☆
├─ Header: ✓ Valid
├─ Footer: ✗ Missing
├─ Entropy: 3.2 (Low - possible overwrite)
├─ Structure: ✗ Invalid atom structure
└─ Consistency: 45%
Recommendation: Video may not play or have artifacts

garbage.dat - DEAD ★☆☆☆☆
├─ Header: ✗ Invalid
├─ Footer: ✗ Invalid
├─ Entropy: 7.9 (Too random)
├─ Structure: ✗ No recognizable structure
└─ Consistency: 12%
Recommendation: File is completely corrupted, do not use
```

---

## Troubleshooting

### Application Won't Start

**Problem:** Double-clicking does nothing or shows error

**Solutions:**

1. **Run as Administrator** (Windows)
   - Right-click `file_recovery_gui.exe`
   - Select "Run as administrator"

2. **Use sudo** (Linux/macOS)
   ```bash
   sudo ./file_recovery_gui
   ```

3. **Check dependencies** (Linux)
   ```bash
   ldd file_recovery_gui
   # Install missing libraries with apt/yum
   ```

### "Access Denied" Error

**Problem:** Cannot open disk for scanning

**Cause:** Insufficient privileges

**Solutions:**

**Windows:**
- Close application
- Right-click `file_recovery_gui.exe`
- "Run as administrator"
- Click "Yes" on UAC prompt

**Linux/macOS:**
```bash
sudo ./file_recovery_gui
# Enter your password
```

### No Files Found

**Problem:** Scan completes but finds 0 files

**Possible Causes:**

1. **Wrong disk selected**
   - Verify you selected the correct disk
   - Check disk letter/number

2. **File types not selected**
   - Ensure at least one file type is checked
   - Try selecting all file types

3. **Files were overwritten**
   - If you wrote new data to disk after deletion, files may be unrecoverable
   - Try Deep Scan if Quick Scan found nothing

4. **Disk is encrypted**
   - BitLocker (Windows) or FileVault (Mac) encryption prevents raw access
   - Decrypt disk first, then scan

### Recovered Files Won't Open

**Problem:** File recovers but won't open in application

**Diagnosis:**

1. **Check quality rating**
   - ★★★★★ (Excellent) → Should work perfectly
   - ★★☆☆☆ (Corrupted) → Expected to have issues

2. **Check file size**
   - Very small files (< 1KB) → Likely incomplete
   - Huge files (> expected) → May be incorrect extraction

3. **Use preview feature**
   - Click "Preview Selected"
   - Check if data looks correct

**Solutions:**

- **Images:** Try opening with multiple viewers (Paint, Photoshop, IrfanView)
- **Documents:** Try repair tools (PDF Repair, Word Repair)
- **Videos:** Try VLC media player (can play partial files)

### Scan is Very Slow

**Problem:** Deep Scan taking hours/days

**Optimizations:**

1. **Enable Smart Scan**
   ```ini
   [Scanning]
   smart_scan = true
   ```
   Skips empty sectors, 40-60% faster

2. **Enable Large Buffers**
   ```ini
   [Scanning]
   large_buffers = true
   buffer_size_mb = 16
   ```
   Reduces I/O overhead

3. **Increase thread count**
   ```ini
   [Performance]
   thread_count = 8
   ```
   Use number of CPU cores

4. **Select specific file types**
   - Don't scan for all 70 types if you only need JPEGs
   - Fewer types = faster signature matching

5. **Scan specific folder** instead of entire disk
   ```
   ☑ Scan Specific Folder
   Folder: C:\Users\YourName\Documents
   ```

**Expected Speeds:**
- HDD: 100-200 MB/s
- SSD: 300-500 MB/s
- USB 2.0: 20-40 MB/s
- USB 3.0: 100-300 MB/s

### Application Crashes

**Problem:** App closes unexpectedly during scan

**Causes & Solutions:**

1. **Disk read error**
   - Bad sectors on disk
   - Solution: Run disk check (chkdsk/fsck)

2. **Out of memory**
   - Too many files found
   - Solution: Reduce buffer size in config.ini:
     ```ini
     buffer_size_mb = 8
     ```

3. **Corrupted disk**
   - Disk hardware failure
   - Solution: Use ddrescue to create disk image first

**Get crash logs:**
```
Check recovery_log.txt for error messages
```

---

## FAQ

### Can I recover files after formatting?

**Yes!** Use **Deep Scan** mode. Formatting only erases the filesystem index, not the actual file data. Deep Scan searches sector-by-sector for file signatures and can recover many files even after format.

**Caveat:** Quick format = good recovery chances. Full format = much lower chances (disk may be overwritten with zeros).

### How long should a scan take?

**Quick Scan:**
- Small disk (< 100GB): 30 seconds - 2 minutes
- Large disk (> 1TB): 2-10 minutes

**Deep Scan:**
- Small disk (< 100GB): 10-30 minutes
- Medium disk (500GB): 1-3 hours
- Large disk (2TB): 4-10 hours

**With optimizations (Smart Scan + Large Buffers):** 40-60% faster

### Can I stop a scan and resume later?

**Currently:** No, scans cannot be resumed.

**Workaround:**
1. Let scan complete
2. Save results
3. Re-run only if needed

**Future feature:** Incremental scanning (planned)

### Why are filenames wrong?

**Reason:** Deep Scan cannot recover original filenames because it bypasses the filesystem. Files are named sequentially:

```
recovered_file_00001.jpg
recovered_file_00002.pdf
recovered_file_00003.mp4
```

**Quick Scan preserves original names** if MFT entry exists.

**Workaround:** Use file preview or metadata viewers to identify files.

### Are recovered files safe to use?

**Depends on quality:**

- ★★★★★ **Excellent:** Yes, 100% safe
- ★★★★☆ **Good:** Yes, minor issues if any
- ★★★☆☆ **Poor:** Check before using, may have errors
- ★★☆☆☆ **Corrupted:** Probably unusable, may crash applications
- ★☆☆☆☆ **Dead:** Do not use

**Always verify:**
1. Use file preview
2. Open with safe application (not critical software)
3. Check file integrity (MD5/SHA256 if you have original hash)

### Can I recover files from a broken/dead disk?

**Depends:**

**If disk is detected** by computer (shows up in Disk Management):
- ✅ Yes, try Deep Scan

**If disk makes clicking noises** (mechanical failure):
- ⚠️ Stop immediately, seek professional data recovery
- Continued use may cause permanent damage

**If disk is not detected** at all:
- ❌ Cannot scan with software
- Need professional recovery service

**Best practice:** Create disk image first using ddrescue or dd:

```bash
# Linux
sudo ddrescue /dev/sdb disk_image.img recovery.log

# Then scan the image file
./file_recovery --disk disk_image.img --output ./recovered
```

### How much space do I need for recovered files?

**Estimate:** Same as amount of deleted data

**Example:**
- Deleted 50GB of photos
- Need 50GB+ free space on output drive

**Tip:** Recovery to external drive with plenty of free space.

### Can it recover files from cloud storage?

**No.** This tool only recovers files from:
- Local hard drives (HDD/SSD)
- External USB drives
- SD cards / Memory cards
- Virtual disk images (.img, .vhd)

**For cloud files:** Use cloud provider's trash/recycle bin.

---

## Tips & Best Practices

### Before Recovery

1. **Stop using the disk immediately**
   - Every new file written reduces recovery chances
   - Turn off applications that write to disk

2. **Identify the correct disk**
   - Check Disk Management (Windows) or Disk Utility (Mac)
   - Note the correct drive letter or device name

3. **Prepare output location**
   - Use a different drive than the one being scanned
   - Ensure sufficient free space

4. **Choose appropriate scan mode**
   - Recently deleted? → Quick Scan
   - Old deletion or formatted? → Deep Scan

### During Recovery

1. **Don't interrupt the scan**
   - Let it complete fully
   - Interrupting may miss files

2. **Monitor progress**
   - Check "Files Found" counter
   - If no files after 10%, may indicate wrong disk

3. **Adjust filters if needed**
   - If too many junk files, enable quality filters
   - If missing files, lower quality threshold

### After Recovery

1. **Verify recovered files**
   - Open a few files to ensure they work
   - Check file sizes are reasonable

2. **Use duplicate detection**
   - Remove redundant files
   - Save disk space

3. **Organize files**
   - Rename files from recovered_xxxx.ext
   - Create folders by type or date

4. **Backup important files**
   - Copy to external drive
   - Upload to cloud storage
   - Prevent future data loss!

### Maximizing Recovery Success

1. **Act quickly**
   - Sooner you scan = better recovery chances
   - Every disk write reduces success rate

2. **Use Deep Scan for best results**
   - More thorough than Quick Scan
   - Worth the extra time for important files

3. **Scan entire disk, not partitions**
   - Select PhysicalDrive (Windows) or /dev/sdX (Linux)
   - Not C:, D:, etc. (partitions may miss data)

4. **Try multiple recovery tools**
   - If this tool doesn't find files, try others
   - Different algorithms may have different success rates

5. **Prevent future data loss**
   - Enable Windows File History / Time Machine
   - Use cloud backup (OneDrive, Google Drive, Dropbox)
   - Regular backups to external drive

---

## Getting Help

### Log Files

**Location:** `recovery_log.txt` (same folder as executable)

**Contains:**
- Scan progress messages
- Errors and warnings
- File quality analyses
- Performance metrics

**When reporting issues, include this log file.**

### Support Channels

- **GitHub Issues:** https://github.com/yourusername/recovery_file/issues
- **Documentation:** https://github.com/yourusername/recovery_file/wiki
- **Email:** support@example.com

### Reporting Bugs

**Include:**
1. Operating system & version (Windows 11, Ubuntu 22.04, etc.)
2. Steps to reproduce the issue
3. Expected behavior vs. actual behavior
4. Screenshots if applicable
5. Log file (`recovery_log.txt`)

**Example:**

```
Bug: Application crashes when scanning USB drive

OS: Windows 11 Pro (22H2)
Drive: SanDisk 32GB USB 3.0

Steps:
1. Launch file_recovery_gui.exe as admin
2. Select "E:\ - USB Drive (32GB)"
3. Click "Start Deep Scan"
4. Application crashes after 2 minutes

Expected: Scan should complete successfully
Actual: Application crashes with error "Memory allocation failed"

Logs attached: recovery_log.txt
```

---

## Appendix

### Supported File Types (70+)

**Images (13):**
jpg, jpeg, png, gif, bmp, tiff, tif, webp, ico, psd, cr2, nef, heic, svg

**Videos (14):**
mp4, avi, mkv, mov, flv, wmv, mpeg, mpg, webm, m4v

**Audio (9):**
mp3, wav, flac, ogg, m4a, aac, wma, midi, mid

**Documents (11):**
pdf, doc, docx, xls, xlsx, ppt, pptx, rtf, epub, odt

**Archives (9):**
zip, rar, 7z, tar, gz, bz2, iso

**Executables (6):**
exe, dll, elf, apk, msi, dmg

**Databases (5):**
db, sqlite, mdb, accdb, pst, ost

**3D/CAD (7):**
obj, stl, fbx, blend, 3ds, dxf, dwg

**Crypto (4):**
wallet.dat, keystore, pem, p12

**And more...**

### Keyboard Shortcuts (GUI)

| Shortcut | Action |
|----------|--------|
| Ctrl+S | Start scan |
| Ctrl+X | Stop scan |
| Ctrl+F | Focus search box |
| Ctrl+A | Select all files |
| Delete | Delete selected files |
| F5 | Refresh file list |
| Ctrl+Q | Quit application |
| F1 | Show about dialog |

---

## Legal & Privacy

### Data Privacy

**This tool runs entirely locally.** No data is sent to external servers.

**We do NOT:**
- Upload your files to the cloud
- Send analytics or telemetry
- Track your usage
- Collect personal information

**Your recovered files stay on your computer.**

### License

This software is licensed under the MIT License. See LICENSE file for details.

### Disclaimer

**Use at your own risk.** While this tool is designed to safely recover deleted files, we cannot guarantee:
- 100% recovery success
- No damage to existing data
- Compatibility with all hardware

**Always:**
- Backup important data regularly
- Test on non-critical disks first
- Understand that recovery is never guaranteed

---

**Thank you for using File Recovery Tool! 🎉**

**Questions? Check the FAQ or contact support.**
