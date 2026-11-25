# Advanced File Recovery Tool

A C++17 tool that tries to recover deleted files from a disk. Its main
feature is a multithreaded "deep scan" that reads raw sectors and looks for
known file signatures (file carving). It has a command-line interface and an
experimental Dear ImGui GUI. The project was built with AI assistance and is
an early-stage project, not a finished recovery product.

## What works

- Deep scan: multithreaded read of raw sectors, matched against binary file
  signatures (`src/scanner/DeepScanner.cpp`). Empty blocks are skipped.
- Signatures can be loaded from `signatures.txt`; a built-in list is used
  otherwise (`src/signatures/FileSignatures.cpp`).
- Quality analysis of recovered files (`src/utils/FileQuality.cpp`).
- End-of-file detection for Unreal Engine GVAS save files
  (`src/recovery/RecoveryManager.cpp`).
- Raw disk access on Windows with sector-aligned reads (`src/utils/DiskIO.cpp`).
- Command-line interface (`src/main.cpp`).
- Dear ImGui GUI (`src/gui/`). Its build has not been verified.

## Limitations

- NTFS quick scan only lists deleted MFT records (the first 10,000). It does
  not read names, sizes or data runs, so it cannot restore file content.
- The FAT32 parser reads the boot sector only and returns no files.
- exFAT and ext are detected but have no parser.
- `--mode signature` (SignatureScanner) is a stub.
- No fragmentation detection: only contiguous files can be recovered.
- Outside GVAS, end-of-file detection is limited, so recovered files may
  contain trailing data.
- Linux disk access is experimental and unverified.
- There are no automated tests.
- For real recovery, use the deep scan. Do not treat this tool as equivalent
  to any commercial or mature recovery software.

## Build

Requires CMake 3.15+ and a C++17 compiler.

    cmake -B build -S . -DBUILD_GUI=OFF
    cmake --build build

This builds the CLI target `file_recovery`. On Linux, the CLI target builds
with `-DBUILD_GUI=OFF`. The GUI (`BUILD_GUI=ON`, the default) fetches Dear
ImGui through `cmake/ImGuiConfig.cmake`; it was not tested.
`build.bat` and `build.sh` are also provided.

Raw disk access normally needs Administrator (Windows) or root privileges.

## Usage

    file_recovery --list
    file_recovery --scan D: --mode deep --types jpg,png,pdf --output E:\recovered

Options (from `printUsage` in `src/main.cpp`):

    --list                List all available disks
    --scan <device>       Scan the specified device
    --mode <mode>         quick, deep, signature, complete (default: quick)
    --types <extensions>  File types to recover (e.g. jpg,png,pdf,docx)
    --min-size <bytes>    Minimum file size to recover
    --max-size <bytes>    Maximum file size to recover
    --output <directory>  Output directory for recovered files
    --threads <count>     Number of threads to use (default: 4)
    --deep                Enable deep scan
    --no-verify           Skip signature verification
    --help                Show help

Always write `--output` to a different physical drive than the one being
scanned, otherwise recovered data can overwrite the files you want back.

## Project layout

- `src/main.cpp`, `gui_main.cpp`: CLI and GUI entry points
- `src/core/`: recovery engine and disk enumeration
- `src/scanner/`: deep, quick and signature scanners
- `src/filesystem/`: filesystem detection and NTFS/FAT32 parsers
- `src/recovery/`: file extraction (`RecoveryManager`)
- `src/signatures/`, `signatures.txt`, `data/`: file signatures
- `src/utils/`: disk I/O, logging, quality analysis
- `src/gui/`: Dear ImGui interface
- `gvas_tools/`: Python helper scripts for GVAS files
- `docs/`: additional notes, some of which may be outdated

## License

MIT. See `LICENSE`.
