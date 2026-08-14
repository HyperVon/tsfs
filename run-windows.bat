@echo off
REM "The Search for Steve" - Chaotic Order (TG97)
REM Launcher for Windows 10 / 11

setlocal
cd /d "%~dp0"

where dosbox-staging >nul 2>&1
if %ERRORLEVEL% equ 0 (
    echo Starting The Search for Steve via DOSBox Staging...
    dosbox-staging --conf "%~dp0dosbox.conf"
    goto :eof
)

where dosbox-x >nul 2>&1
if %ERRORLEVEL% equ 0 (
    echo Starting The Search for Steve via DOSBox-X...
    dosbox-x -conf "%~dp0dosbox.conf"
    goto :eof
)

where dosbox >nul 2>&1
if %ERRORLEVEL% equ 0 (
    echo Starting The Search for Steve via DOSBox...
    dosbox -conf "%~dp0dosbox.conf"
    goto :eof
)

REM Check default install locations
if exist "%LOCALAPPDATA%\DOSBox-Staging\dosbox.exe" (
    "%LOCALAPPDATA%\DOSBox-Staging\dosbox.exe" --conf "%~dp0dosbox.conf"
    goto :eof
)

if exist "%ProgramFiles%\DOSBox-0.74-3\DOSBox.exe" (
    "%ProgramFiles%\DOSBox-0.74-3\DOSBox.exe" -conf "%~dp0dosbox.conf"
    goto :eof
)

if exist "%ProgramFiles(x86)%\DOSBox-0.74-3\DOSBox.exe" (
    "%ProgramFiles(x86)%\DOSBox-0.74-3\DOSBox.exe" -conf "%~dp0dosbox.conf"
    goto :eof
)

echo ============================================================
echo Error: DOSBox is not installed or not in PATH.
echo ============================================================
echo Install DOSBox Staging via Windows Terminal / PowerShell:
echo   winget install DOSBox-Staging.DOSBox-Staging
echo Or download from: https://dosbox-staging.github.io/
echo ============================================================
pause
