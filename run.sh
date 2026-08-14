#!/usr/bin/env bash
# "The Search for Steve" - Chaotic Order (TG97)
# Launcher for macOS / Linux

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR" || exit 1

if command -v dosbox-staging >/dev/null 2>&1; then
    EMULATOR="dosbox-staging"
elif command -v dosbox-x >/dev/null 2>&1; then
    EMULATOR="dosbox-x"
elif command -v dosbox >/dev/null 2>&1; then
    EMULATOR="dosbox"
else
    echo "============================================================"
    echo " Error: DOSBox is not installed."
    echo "============================================================"
    echo " macOS:   brew install dosbox-staging"
    echo " Ubuntu:  sudo apt install dosbox-staging (or dosbox)"
    echo " Fedora:  sudo dnf install dosbox-staging"
    echo " Arch:    sudo pacman -S dosbox-staging"
    echo "============================================================"
    exit 1
fi

echo "Starting The Search for Steve via $EMULATOR..."
"$EMULATOR" -conf dosbox.conf
