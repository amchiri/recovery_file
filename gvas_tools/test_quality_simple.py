#!/usr/bin/env python3
"""
File Quality Analyzer - Test les fichiers récupérés
"""

import sys
import os

def analyze_file(filepath):
    """Analyse la qualité d'un fichier récupéré"""
    print(f"\n{'='*80}")
    print(f"File: {os.path.basename(filepath)}")
    print('='*80)
    
    with open(filepath, 'rb') as f:
        data = f.read()
    
    total_bytes = len(data)
    zero_bytes = data.count(0)
    nonzero_bytes = total_bytes - zero_bytes
    zero_percentage = (zero_bytes / total_bytes) * 100 if total_bytes > 0 else 100
    
    # Check GVAS header
    has_gvas = len(data) >= 4 and data[:4] == b'GVAS'
    
    # Check GVAS footer
    has_footer = False
    pattern = b'\x05\x00\x00\x00None\x00'
    for i in range(len(data) - 100, len(data) - 9):
        if data[i:i+9] == pattern:
            bytes_after = len(data) - i
            if 9 <= bytes_after <= 20:
                has_footer = True
                break
    
    # Determine quality
    if zero_percentage > 99:
        quality = "DEAD"
        color = '\033[91m'  # Red
    elif zero_percentage > 90:
        quality = "CORRUPTED"
        color = '\033[93m'  # Yellow
    elif zero_percentage > 60:
        quality = "POOR"
        color = '\033[93m'
    elif zero_percentage > 30:
        if has_gvas and has_footer:
            quality = "EXCELLENT"
            color = '\033[92m'  # Green
        else:
            quality = "GOOD"
            color = '\033[96m'  # Cyan
    else:
        quality = "EXCELLENT"
        color = '\033[92m'
    
    reset = '\033[0m'
    
    print(f"\nTotal Size:        {total_bytes:,} bytes")
    print(f"Zero bytes:        {zero_bytes:,} ({zero_percentage:.1f}%)")
    print(f"Non-zero bytes:    {nonzero_bytes:,} ({100-zero_percentage:.1f}%)")
    print(f"Valid GVAS header: {'YES ✓' if has_gvas else 'NO ✗'}")
    print(f"Valid GVAS footer: {'YES ✓' if has_footer else 'NO ✗'}")
    
    print(f"\nQuality Level:     {color}{quality}{reset}")
    
    worth_recovering = quality != "DEAD" and (quality != "CORRUPTED" or has_gvas)
    worth_color = '\033[92m' if worth_recovering else '\033[91m'
    print(f"Worth recovering?  {worth_color}{'YES ✓' if worth_recovering else 'NO ✗'}{reset}")
    
    # Recommendation
    recommendations = {
        "EXCELLENT": "File is in excellent condition and should be fully usable.",
        "GOOD": "File is in good condition. Minor data loss possible but should be usable.",
        "POOR": "File is partially corrupted but may contain recoverable data." if has_gvas else "File is partially corrupted. Recovery uncertain.",
        "CORRUPTED": "File is heavily corrupted but header is intact. May be partially recoverable." if has_gvas else "File is heavily corrupted. Recovery unlikely.",
        "DEAD": "File has been overwritten with zeros. Data is unrecoverable. Skip this file."
    }
    
    print(f"\nRecommendation: {recommendations[quality]}")
    print('='*80)

if __name__ == "__main__":
    print("\n╔═══════════════════════════════════════════════════════════════╗")
    print("║          File Quality Analysis - Recovered Files             ║")
    print("╚═══════════════════════════════════════════════════════════════╝")
    
    files = [
        r"E:\recovered_212206600192.sav",
        r"E:\recovered_280231346176.sav",
        r"E:\recovered_204881215488.sav"
    ]
    
    for filepath in files:
        if os.path.exists(filepath):
            analyze_file(filepath)
        else:
            print(f"\n⚠ File not found: {filepath}")
    
    print("\n✓ Analysis complete!\n")
