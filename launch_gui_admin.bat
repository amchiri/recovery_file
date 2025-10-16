@echo off
echo ========================================
echo File Recovery Tool - GUI Edition
echo ========================================
echo.
echo Ce programme necessite les privileges administrateur
echo pour acceder directement aux disques.
echo.
echo Cliquez sur Oui dans la fenetre UAC...
echo.

REM Verifier si deja admin
net session >nul 2>&1
if %errorLevel% == 0 (
    echo Deja lance en administrateur!
    echo Lancement de l'interface...
    echo.
    start "" "%~dp0build\Release\file_recovery_gui.exe"
) else (
    echo Demande de privileges administrateur...
    powershell -Command "Start-Process '%~dp0build\Release\file_recovery_gui.exe' -Verb RunAs"
)

exit
