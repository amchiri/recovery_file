#!/usr/bin/env python3
"""Vérifie le champ de taille dans l'en-tête GVAS"""

import struct

def check_size_field(filepath):
    """Vérifie si offset 0x08 contient la taille du fichier"""
    with open(filepath, 'rb') as f:
        # Lire les premiers 16 bytes
        header = f.read(16)
        
        # Vérifier magic
        magic = header[0:4]
        print(f"Magic: {magic} (should be b'GVAS')")
        
        # Version
        version = struct.unpack('<I', header[4:8])[0]
        print(f"Version (0x04): {version}")
        
        # Taille à 0x08
        size_field = struct.unpack('<I', header[8:12])[0]
        print(f"Size field at 0x08: {size_field} bytes ({size_field/1024:.2f} KB)")
        
        # Autre champ à 0x0C
        other_field = struct.unpack('<I', header[12:16])[0]
        print(f"Field at 0x0C: {other_field}")
        
        # Taille réelle du fichier
        f.seek(0, 2)
        actual_size = f.tell()
        print(f"\nActual file size: {actual_size} bytes ({actual_size/1024:.2f} KB)")
        
        # Comparer
        if size_field == actual_size:
            print("\n✅ Size field at 0x08 MATCHES file size!")
        elif size_field + 4 == actual_size:
            print("\n✅ Size field at 0x08 + 4 bytes MATCHES file size!")
        elif size_field + 8 == actual_size:
            print("\n✅ Size field at 0x08 + 8 bytes MATCHES file size!")
        else:
            print(f"\n❌ Size field does NOT match (difference: {actual_size - size_field} bytes)")
        
        # Afficher les premiers 64 bytes en hex
        f.seek(0)
        first_bytes = f.read(64)
        print("\nFirst 64 bytes:")
        for i in range(0, 64, 16):
            hex_str = ' '.join(f'{b:02X}' for b in first_bytes[i:i+16])
            ascii_str = ''.join(chr(b) if 32 <= b < 127 else '.' for b in first_bytes[i:i+16])
            print(f"{i:04X}: {hex_str:<47} | {ascii_str}")

if __name__ == '__main__':
    filepath = r"C:\Users\polom\OneDrive\Documents\My Games\Octopath_Traveler2\Steam\76561198314156229\SaveGames\SaveData1.sav"
    check_size_field(filepath)
