# Tests pour le File Recovery Tool
# À exécuter après compilation

import subprocess
import os
import sys

def run_command(cmd):
    """Exécute une commande et retourne le résultat"""
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, shell=True)
        return result.returncode == 0, result.stdout, result.stderr
    except Exception as e:
        return False, "", str(e)

def test_compilation():
    """Vérifie que le projet compile"""
    print("Test 1: Compilation...")
    if os.path.exists("build/Release/file_recovery.exe") or os.path.exists("build/file_recovery.exe") or os.path.exists("build/file_recovery"):
        print("✓ Compilation réussie")
        return True
    else:
        print("✗ Exécutable non trouvé")
        return False

def test_help():
    """Test de l'affichage de l'aide"""
    print("\nTest 2: Affichage de l'aide...")
    
    exe = None
    if os.path.exists("build/Release/file_recovery.exe"):
        exe = "build/Release/file_recovery.exe"
    elif os.path.exists("build/file_recovery.exe"):
        exe = "build/file_recovery.exe"
    elif os.path.exists("build/file_recovery"):
        exe = "./build/file_recovery"
    
    if not exe:
        print("✗ Exécutable non trouvé")
        return False
    
    success, stdout, stderr = run_command(f"{exe} --help")
    if "Usage:" in stdout or "Usage:" in stderr:
        print("✓ Aide affichée correctement")
        return True
    else:
        print("✗ Erreur lors de l'affichage de l'aide")
        return False

def test_list_disks():
    """Test de l'énumération des disques (nécessite des privilèges)"""
    print("\nTest 3: Énumération des disques...")
    print("⚠ Ce test nécessite des privilèges administrateur")
    
    exe = None
    if os.path.exists("build/Release/file_recovery.exe"):
        exe = "build/Release/file_recovery.exe"
    elif os.path.exists("build/file_recovery.exe"):
        exe = "build/file_recovery.exe"
    elif os.path.exists("build/file_recovery"):
        exe = "sudo ./build/file_recovery"
    
    if not exe:
        print("✗ Exécutable non trouvé")
        return False
    
    success, stdout, stderr = run_command(f"{exe} --list")
    if "Available disks" in stdout or "Device" in stdout:
        print("✓ Énumération réussie")
        return True
    else:
        print("⚠ Impossible d'énumérer (privilèges requis probablement)")
        return False

def main():
    print("=" * 50)
    print(" Tests du File Recovery Tool")
    print("=" * 50)
    
    tests = [
        test_compilation,
        test_help,
        test_list_disks
    ]
    
    results = []
    for test in tests:
        try:
            results.append(test())
        except Exception as e:
            print(f"✗ Erreur: {e}")
            results.append(False)
    
    print("\n" + "=" * 50)
    print(f" Résultats: {sum(results)}/{len(results)} tests passés")
    print("=" * 50)
    
    if all(results):
        print("✓ Tous les tests sont passés!")
        return 0
    else:
        print("⚠ Certains tests ont échoué")
        return 1

if __name__ == "__main__":
    sys.exit(main())
