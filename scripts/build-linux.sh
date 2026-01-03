#!/bin/bash
set -e

# Build script for WorkraveQt on Linux
# Usage: ./scripts/build-linux.sh [PREFIX]
#
# Examples:
#   ./scripts/build-linux.sh              # Install to /usr
#   ./scripts/build-linux.sh /usr/local   # Install to /usr/local
#   ./scripts/build-linux.sh ~/.local     # Install to user directory

PREFIX="${1:-/usr}"
BUILD_DIR="build"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

echo "=== WorkraveQt Build Script ==="
echo "PREFIX: $PREFIX"
echo "BUILD_DIR: $BUILD_DIR"
echo ""

cd "$PROJECT_DIR"

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Run qmake - prefer Qt6 over Qt5
echo "Running qmake..."
if command -v qmake6 &> /dev/null; then
    QMAKE=qmake6
elif command -v qmake-qt6 &> /dev/null; then
    QMAKE=qmake-qt6
elif command -v qmake-qt5 &> /dev/null; then
    QMAKE=qmake-qt5
elif command -v qmake &> /dev/null; then
    QMAKE=qmake
else
    echo "Error: qmake not found. Please install Qt development packages."
    echo "  Fedora: sudo dnf install qt6-qtbase-devel"
    echo "  Ubuntu: sudo apt install qt6-base-dev"
    echo "  Arch:   sudo pacman -S qt6-base"
    exit 1
fi

echo "Using: $QMAKE"
$QMAKE ../src/WorkraveQt.pro PREFIX="$PREFIX"

# Build
echo "Building..."
make -j$(nproc)

echo ""
echo "=== Build complete! ==="
echo ""
echo "To install (requires root for system directories):"
echo "  sudo make install"
echo ""
echo "Or for user installation:"
echo "  cd $BUILD_DIR && make INSTALL_ROOT=\$HOME/.local install"
