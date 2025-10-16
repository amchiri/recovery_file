#!/usr/bin/env python3
"""Outil pour analyser la structure d'un fichier GVAS"""

import struct
import sys

def read_int32(f):
    """Lit un int32 little-endian"""
    return struct.unpack('<i', f.read(4))[0]

def read_fstring(f):
    """Lit une FString Unreal"""
    length = read_int32(f)
    if length < 0:
        # Unicode string (UTF-16)
        length = abs(length)
        string = f.read(length * 2).decode('utf-16le', errors='ignore')
        return string.rstrip('\x00')
    else:
        # ASCII string
        string = f.read(length).decode('ascii', errors='ignore')
        return string.rstrip('\x00')

def analyze_gvas(filepath):
    """Analyse un fichier GVAS"""
    with open(filepath, 'rb') as f:
        # Lire le magic
        magic = f.read(4)
        if magic != b'GVAS':
            print(f"❌ Not a GVAS file! Magic: {magic}")
            return
        
        print(f"✅ GVAS file detected!")
        print(f"File size: {f.seek(0, 2)} bytes")
        f.seek(4)  # Retour après magic
        
        # SaveGameFileVersion
        save_version = read_int32(f)
        print(f"SaveGameFileVersion: {save_version}")
        
        # PackageFileUE4Version
        package_version = read_int32(f)
        print(f"PackageFileUE4Version: {package_version}")
        
        # EngineVersion
        major = struct.unpack('<H', f.read(2))[0]
        minor = struct.unpack('<H', f.read(2))[0]
        patch = struct.unpack('<H', f.read(2))[0]
        build = struct.unpack('<I', f.read(4))[0]
        print(f"EngineVersion: {major}.{minor}.{patch}.{build}")
        
        # Branch name
        branch = read_fstring(f)
        print(f"Branch: '{branch}'")
        
        # CustomVersionFormat
        custom_format = read_int32(f)
        print(f"CustomVersionFormat: {custom_format}")
        
        # CustomVersions count
        custom_count = read_int32(f)
        print(f"CustomVersions count: {custom_count}")
        
        # Skip custom versions
        for i in range(custom_count):
            guid = f.read(16)
            version = read_int32(f)
            print(f"  Custom version {i}: version={version}")
        
        # SaveGameClassName
        class_name = read_fstring(f)
        print(f"SaveGameClassName: '{class_name}'")
        
        header_end = f.tell()
        print(f"\n📍 Header ends at offset: {header_end} (0x{header_end:X})")
        
        # Analyser la fin du fichier
        file_size = f.seek(0, 2)
        print(f"\n📊 Analyzing file end...")
        
        # Chercher les derniers octets non-zéros
        chunk_size = 1024 * 1024  # 1MB chunks
        f.seek(max(0, file_size - chunk_size))
        tail = f.read()
        
        # Trouver le dernier octet non-zéro
        last_nonzero = len(tail) - 1
        while last_nonzero > 0 and tail[last_nonzero] == 0:
            last_nonzero -= 1
        
        actual_end = file_size - len(tail) + last_nonzero + 1
        trailing_zeros = file_size - actual_end
        
        print(f"Last non-zero byte at offset: {actual_end} (0x{actual_end:X})")
        print(f"Trailing zeros: {trailing_zeros} bytes ({trailing_zeros/1024:.2f} KB)")
        print(f"Actual data size: {actual_end} bytes ({actual_end/1024:.2f} KB)")
        
        # Afficher les derniers octets non-zéros
        f.seek(max(0, actual_end - 32))
        last_bytes = f.read(32)
        print(f"\nLast 32 bytes before trailing zeros:")
        print(' '.join(f'{b:02X}' for b in last_bytes))

if __name__ == '__main__':
    filepath = r"C:\Users\polom\OneDrive\Documents\My Games\Octopath_Traveler2\Steam\76561198314156229\SaveGames\SaveData1.sav"
    
    if len(sys.argv) > 1:
        filepath = sys.argv[1]
    
    print(f"Analyzing: {filepath}\n")
    analyze_gvas(filepath)
