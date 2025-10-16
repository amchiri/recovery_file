#!/bin/bash

# Script de compilation pour Linux/macOS

echo "======================================"
echo " File Recovery Tool - Build Script"
echo "======================================"
echo ""

# Vérifier si CMake est installé
if ! command -v cmake &> /dev/null; then
    echo "ERREUR: CMake n'est pas installé"
    echo "Installez-le avec:"
    echo "  Ubuntu/Debian: sudo apt-get install cmake build-essential"
    echo "  Fedora: sudo dnf install cmake gcc-c++"
    echo "  macOS: brew install cmake"
    exit 1
fi

# Créer le répertoire de build
if [ ! -d "build" ]; then
    echo "Création du répertoire build..."
    mkdir build
fi

cd build

echo ""
echo "Configuration du projet avec CMake..."
echo ""

cmake .. -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo "ERREUR lors de la configuration"
    cd ..
    exit 1
fi

echo ""
echo "Compilation du projet..."
echo ""

make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

if [ $? -eq 0 ]; then
    echo ""
    echo "======================================"
    echo " Compilation réussie!"
    echo "======================================"
    echo ""
    echo "L'exécutable se trouve dans: build/file_recovery"
    echo ""
    echo "Pour tester:"
    echo "  ./build/file_recovery --list"
    echo ""
    echo "Note: Vous aurez besoin des privilèges root/sudo"
    echo "      pour accéder aux disques physiques."
    echo ""
else
    echo ""
    echo "ERREUR lors de la compilation"
    echo ""
fi

cd ..
