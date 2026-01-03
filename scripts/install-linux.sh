#!/bin/bash
set -e

# Install script for WorkraveQt on Linux
# Usage: ./scripts/install-linux.sh [--user | --system | --prefix PREFIX]
#
# Options:
#   --user      Install to ~/.local (no root required)
#   --system    Install to /usr (requires root)
#   --prefix    Install to custom prefix

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build"

# Parse arguments
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

echo "=== WorkraveQt Install Script ==="
echo "Install type: $INSTALL_TYPE"
echo "PREFIX: $PREFIX"
echo ""

# Always rebuild with the correct PREFIX
echo "Building with PREFIX=$PREFIX..."
"$SCRIPT_DIR/build-linux.sh" "$PREFIX"

cd "$BUILD_DIR"

# Install
echo "Installing..."
if [ "$INSTALL_TYPE" = "user" ]; then
    # User installation - no root needed
    make INSTALL_ROOT="" install
    
    # Fix desktop file to use full path (since ~/.local/bin may not be in PATH for GUI apps)
    sed -i "s|Exec=workraveqt|Exec=$PREFIX/bin/workraveqt|" "$PREFIX/share/applications/workraveqt.desktop"
    
    # Create autostart directory if needed (user location instead of /etc/xdg)
    mkdir -p "$HOME/.config/autostart"
    sed "s|Exec=workraveqt|Exec=$PREFIX/bin/workraveqt|" "$PROJECT_DIR/data/workraveqt-autostart.desktop" > "$HOME/.config/autostart/workraveqt.desktop"
    
    echo ""
    echo "=== Installed to $PREFIX ==="
elif [ "$INSTALL_TYPE" = "system" ]; then
    # System installation - needs root
    if [ "$EUID" -ne 0 ]; then
        echo "System installation requires root. Running with sudo..."
        sudo make install
    else
        make install
    fi
    
    # Update icon cache
    if command -v gtk-update-icon-cache &> /dev/null; then
        sudo gtk-update-icon-cache -f -t /usr/share/icons/hicolor 2>/dev/null || true
    fi
    
    # Update desktop database
    if command -v update-desktop-database &> /dev/null; then
        sudo update-desktop-database /usr/share/applications 2>/dev/null || true
    fi
    
    echo ""
    echo "=== Installed to $PREFIX ==="
else
    # Custom prefix
    make INSTALL_ROOT="" install
    echo ""
    echo "=== Installed to $PREFIX ==="
fi

# Generate PNG icons from SVG for better compatibility
echo "Generating PNG icons..."
ICON_SVG="$PREFIX/share/icons/hicolor/scalable/apps/workraveqt.svg"
for SIZE in 16 24 32 48 64 128; do
    ICON_DIR="$PREFIX/share/icons/hicolor/${SIZE}x${SIZE}/apps"
    mkdir -p "$ICON_DIR"
    if command -v rsvg-convert &> /dev/null; then
        rsvg-convert -w $SIZE -h $SIZE "$ICON_SVG" -o "$ICON_DIR/workraveqt.png" 2>/dev/null
    elif command -v magick &> /dev/null; then
        magick -background none -resize ${SIZE}x${SIZE} "$ICON_SVG" "$ICON_DIR/workraveqt.png" 2>/dev/null
    elif command -v convert &> /dev/null; then
        convert -background none -resize ${SIZE}x${SIZE} "$ICON_SVG" "$ICON_DIR/workraveqt.png" 2>/dev/null
    fi
done

# Update icon cache
if command -v gtk-update-icon-cache &> /dev/null; then
    gtk-update-icon-cache -f "$PREFIX/share/icons/hicolor" 2>/dev/null || true
fi

echo ""
echo "WorkraveQt will start automatically on next login."
echo "To start now, run: workraveqt &"
