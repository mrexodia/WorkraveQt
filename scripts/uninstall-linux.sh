#!/bin/bash
set -e

# Uninstall script for WorkraveQt on Linux
# Usage: ./scripts/uninstall-linux.sh [--user | --system | --prefix PREFIX]

INSTALL_TYPE="system"
PREFIX="/usr"

while [[ $# -gt 0 ]]; do
    case $1 in
        --user)
            INSTALL_TYPE="user"
            PREFIX="$HOME/.local"
            shift
            ;;
        --system)
            INSTALL_TYPE="system"
            PREFIX="/usr"
            shift
            ;;
        --prefix)
            INSTALL_TYPE="custom"
            PREFIX="$2"
            shift 2
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [--user | --system | --prefix PREFIX]"
            exit 1
            ;;
    esac
done

echo "=== WorkraveQt Uninstall Script ==="
echo "PREFIX: $PREFIX"
echo ""

# Files to remove
FILES=(
    "$PREFIX/bin/workraveqt"
    "$PREFIX/share/applications/workraveqt.desktop"
    "$PREFIX/share/metainfo/io.workrave.WorkraveQt.metainfo.xml"
    "$PREFIX/share/icons/hicolor/scalable/apps/workraveqt.svg"
)

# Add autostart file
if [ "$INSTALL_TYPE" = "user" ]; then
    FILES+=("$HOME/.config/autostart/workraveqt.desktop")
else
    FILES+=("/etc/xdg/autostart/workraveqt-autostart.desktop")
fi

# Remove files
for file in "${FILES[@]}"; do
    if [ -f "$file" ]; then
        echo "Removing: $file"
        if [ "$INSTALL_TYPE" = "system" ] && [ "$EUID" -ne 0 ]; then
            sudo rm -f "$file"
        else
            rm -f "$file"
        fi
    fi
done

# Update caches
if [ "$INSTALL_TYPE" = "system" ]; then
    if command -v gtk-update-icon-cache &> /dev/null; then
        sudo gtk-update-icon-cache -f -t /usr/share/icons/hicolor 2>/dev/null || true
    fi
    if command -v update-desktop-database &> /dev/null; then
        sudo update-desktop-database /usr/share/applications 2>/dev/null || true
    fi
fi

echo ""
echo "=== WorkraveQt uninstalled ==="
