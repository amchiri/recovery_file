import sys

filename = sys.argv[1] if len(sys.argv) > 1 else 'E:\\recovered_128191873024.sav'
data = open(filename, 'rb').read()

print(f'File: {filename}')
print(f'File size: {len(data):,} bytes ({len(data)/1024/1024:.2f} MB)')

# Check GVAS
has_gvas = data[:4] == b'GVAS'
print(f'Has GVAS signature: {has_gvas}')

# Check Octopath
octopath = b'KSSaveGameBP' in data[:4096]
print(f'Has Octopath marker: {octopath}')

# Find last "None"
pattern = b'\x05\x00\x00\x00None\x00'
idx = data.rfind(pattern)
print(f'Last None at: {idx:,}')
if idx >= 0:
    bytes_after = len(data) - idx
    print(f'Bytes after None start: {bytes_after}')
    
    # Show what's after None
    if bytes_after > 13:
        print(f'\n⚠️ WARNING: File has {bytes_after} bytes after None (should be 13)')
        print(f'Extra bytes: {bytes_after - 13}')
        print(f'Next 32 bytes after None: {data[idx:idx+32].hex()}')
