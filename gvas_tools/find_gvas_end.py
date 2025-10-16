#!/usr/bin/env python3
"""Trouve exactement où un fichier GVAS se termine"""

import struct
import sys

def analyze_end(filepath):
    """Analyse la fin d'un fichier GVAS"""
    with open(filepath, 'rb') as f:
        # Aller à la fin
        f.seek(0, 2)
        filesize = f.tell()
        
        print(f"File size: {filesize} bytes ({filesize/1024:.2f} KB)")
        
        # Lire les derniers 512 bytes
        f.seek(max(0, filesize - 512))
        tail = f.read()
        
        print("\nLast 512 bytes (hex):")
        for i in range(0, len(tail), 32):
            offset = filesize - len(tail) + i
            hex_str = ' '.join(f'{b:02X}' for b in tail[i:i+32])
            ascii_str = ''.join(chr(b) if 32 <= b < 127 else '.' for b in tail[i:i+32])
            print(f"{offset:08X}: {hex_str:<95} | {ascii_str}")
        
        # Chercher le pattern "None" dans les derniers 100 bytes
        print("\n\nSearching for 'None' pattern in last 100 bytes:")
        f.seek(max(0, filesize - 100))
        search_area = f.read()
        
        # Chercher "None" avec length prefix (FString format)
        # Format: length (int32) + string data
        for i in range(len(search_area) - 8):
            # Chercher 05 00 00 00 4E 6F 6E 65 00 (length=5, "None\0")
            if (search_area[i:i+4] == b'\x05\x00\x00\x00' and 
                search_area[i+4:i+9] == b'None\x00'):
                offset = filesize - len(search_area) + i
                print(f"✅ Found FString 'None' at offset {offset} (0x{offset:X})")
                print(f"   Pattern: 05 00 00 00 4E 6F 6E 65 00")
                print(f"   File should end at offset {offset + 9} (0x{offset+9:X})")
                return offset + 9
        
        # Chercher juste "None" sans FString wrapper
        none_pos = search_area.rfind(b'None')
        if none_pos != -1:
            offset = filesize - len(search_area) + none_pos
            print(f"⚠️  Found 'None' string at offset {offset} (0x{offset:X})")
            print(f"   But not as FString - might not be the real end marker")
        
        return filesize

if __name__ == '__main__':
    filepath = r"C:\Users\polom\OneDrive\Documents\My Games\Octopath_Traveler2\Steam\76561198314156229\SaveGames\SaveData1.sav"
    
    if len(sys.argv) > 1:
        filepath = sys.argv[1]
    
    print(f"Analyzing: {filepath}\n")
    end_offset = analyze_end(filepath)
    print(f"\n✅ Recommended file end: {end_offset} bytes")
