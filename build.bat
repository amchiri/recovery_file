@echo off
REM Script de compilation pour Windows

echo ======================================
echo  File Recovery Tool - Build Script
echo ======================================
echo.

REM Vérifier si CMake est installé
where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERREUR: CMake n'est pas installé ou pas dans le PATH
    echo Téléchargez CMake depuis: https://cmake.org/download/
    pause
    exit /b 1
)

REM Créer le répertoire de build
if not exist "build" (
    echo Création du répertoire build...
    mkdir build
)

cd build

echo.
echo Configuration du projet avec CMake...
echo.

REM Détecter Visual Studio
cmake .. -G "Visual Studio 17 2022" 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Visual Studio 2022 non trouvé, essai avec VS 2019...
    cmake .. -G "Visual Studio 16 2019" 2>nul
    if %ERRORLEVEL% NEQ 0 (
        echo Visual Studio non trouvé, essai avec MinGW...
        cmake .. -G "MinGW Makefiles"
        if %ERRORLEVEL% NEQ 0 (
            echo ERREUR: Impossible de configurer le projet
            cd ..
            pause
            exit /b 1
        )
    )
)

echo.
echo Compilation du projet...
echo.

cmake --build . --config Release

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ======================================
    echo  Compilation réussie!
    echo ======================================
    echo.
    echo L'exécutable se trouve dans: build\Release\file_recovery.exe
    echo ou build\file_recovery.exe
    echo.
    echo Pour tester:
    echo   cd build\Release
    echo   file_recovery.exe --list
    echo.
) else (
    echo.
    echo ERREUR lors de la compilation
    echo.
)

cd ..
pause
