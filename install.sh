#!/bin/sh
set -e

REPO="LandonHarter/maylang"
INSTALL_DIR="$HOME/.may/bin"
BINARY_NAME="may"

DOWNLOAD_URL="https://github.com/$REPO/releases/latest/download/$BINARY_NAME"

echo "Installing May programming language..."

mkdir -p "$INSTALL_DIR"

if command -v curl > /dev/null 2>&1; then
    curl -fSL "$DOWNLOAD_URL" -o "$INSTALL_DIR/$BINARY_NAME"
elif command -v wget > /dev/null 2>&1; then
    wget -q "$DOWNLOAD_URL" -O "$INSTALL_DIR/$BINARY_NAME"
else
    echo "Error: curl or wget is required to download May."
    exit 1
fi

chmod +x "$INSTALL_DIR/$BINARY_NAME"

echo ""
echo "May has been installed to $INSTALL_DIR/$BINARY_NAME"
echo ""

if echo "$PATH" | grep -q "$INSTALL_DIR"; then
    echo "The install directory is already in your PATH. You're all set!"
else
    echo "Add the following line to your shell config to use 'may' from anywhere:"
    echo ""
    echo "  Bash (~/.bashrc):"
    echo "    export PATH=\"\$HOME/.may/bin:\$PATH\""
    echo ""
    echo "  Zsh (~/.zshrc):"
    echo "    export PATH=\"\$HOME/.may/bin:\$PATH\""
    echo ""
    echo "  Fish (~/.config/fish/config.fish):"
    echo "    fish_add_path \$HOME/.may/bin"
    echo ""
    echo "Then restart your shell or run 'source' on your config file."
fi
