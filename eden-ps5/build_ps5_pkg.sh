#!/bin/bash

# Eden Nintendo Switch Emulator - PS5 PKG Builder
# SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project - PS5 Port
# SPDX-License-Identifier: GPL-3.0-or-later

set -e

echo "============================================"
echo "   Eden PS5 PKG Builder"
echo "============================================"

# Configuration
PROJECT_NAME="eden-ps5"
VERSION="1.0.0"
TITLE_ID="EDEN00001"
BUILD_DIR="build_ps5"
PKG_DIR="pkg_output"

# Check for PS5 development tools
if [ -z "$PS5_SDK_ROOT" ]; then
    echo "Warning: PS5_SDK_ROOT not set. Using default path."
    export PS5_SDK_ROOT="/opt/orbis-sdk"
fi

if [ ! -d "$PS5_SDK_ROOT" ]; then
    echo "Error: PS5 SDK not found at $PS5_SDK_ROOT"
    echo "Please install Orbis SDK or set PS5_SDK_ROOT environment variable"
    exit 1
fi

echo "Using PS5 SDK: $PS5_SDK_ROOT"

# Create build directory
echo "Creating build directory..."
mkdir -p $BUILD_DIR
mkdir -p $PKG_DIR

# Configure CMake for PS5
echo "Configuring CMake for PS5..."
cd $BUILD_DIR

cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=$PS5_SDK_ROOT/cmake/orbis-toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DPS5_BUILD=ON \
    -DENABLE_PS5_GNM=ON \
    -DENABLE_PS5_AUDIO=ON \
    -DENABLE_PS5_INPUT=ON \
    -DENABLE_ETAHEN_PLUGIN=ON \
    -DBUILD_PS5_PKG=ON \
    -f ../CMakeLists_PS5.txt

# Build the project
echo "Building Eden PS5..."
make -j$(nproc)

# Create PKG structure
echo "Creating PKG structure..."
PKG_TEMP_DIR="pkg_temp"
mkdir -p $PKG_TEMP_DIR

# Copy executable
cp bin/eden-ps5 $PKG_TEMP_DIR/eden_emulator.elf

# Create param.sfo (PS5 package metadata)
echo "Creating param.sfo..."
cat > param.sfo.xml << EOF
<?xml version="1.0" encoding="UTF-8"?>
<sfo>
    <value name="APP_TYPE" type="integer">1</value>
    <value name="APP_VER" type="string">$VERSION</value>
    <value name="ATTRIBUTE" type="integer">0</value>
    <value name="CATEGORY" type="string">gd</value>
    <value name="CONTENT_ID" type="string">UP9000-${TITLE_ID}_00-EDENEMU0000000000</value>
    <value name="FORMAT" type="string">obs</value>
    <value name="TITLE" type="string">Eden Nintendo Switch Emulator</value>
    <value name="TITLE_ID" type="string">$TITLE_ID</value>
    <value name="VERSION" type="string">$VERSION</value>
</sfo>
EOF

# Convert param.sfo.xml to param.sfo
if command -v psp2sfo >/dev/null 2>&1; then
    psp2sfo param.sfo.xml $PKG_TEMP_DIR/param.sfo
else
    echo "Warning: psp2sfo not found, creating dummy param.sfo"
    touch $PKG_TEMP_DIR/param.sfo
fi

# Create icon (placeholder)
echo "Creating icon..."
if [ -f "../dist/eden_named.png" ]; then
    cp ../dist/eden_named.png $PKG_TEMP_DIR/icon0.png
else
    echo "Warning: Icon not found, creating placeholder"
    # Create a simple 512x512 placeholder icon
    if command -v convert >/dev/null 2>&1; then
        convert -size 512x512 xc:blue -pointsize 40 -fill white -gravity center \
                -annotate +0+0 "Eden\nEmulator" $PKG_TEMP_DIR/icon0.png
    else
        touch $PKG_TEMP_DIR/icon0.png
    fi
fi

# Create eboot.bin (main executable)
echo "Creating eboot.bin..."
cp $PKG_TEMP_DIR/eden_emulator.elf $PKG_TEMP_DIR/eboot.bin

# Create PKG file
echo "Creating PKG file..."
PKG_OUTPUT="../$PKG_DIR/eden-emulator-ps5-v$VERSION.pkg"

if command -v orbis-pub-gen >/dev/null 2>&1; then
    # Use official PS5 packaging tools if available
    orbis-pub-gen -contentid UP9000-${TITLE_ID}_00-EDENEMU0000000000 \
                  -passcode 00000000000000000000000000000000 \
                  $PKG_TEMP_DIR $PKG_OUTPUT
else
    echo "Warning: orbis-pub-gen not found, creating simple archive"
    # Create a simple tar archive as fallback
    tar -czf $PKG_OUTPUT -C $PKG_TEMP_DIR .
fi

# Create installation script for etaHEN
echo "Creating etaHEN installation script..."
cat > ../$PKG_DIR/install_eden.sh << 'EOF'
#!/bin/bash

# Eden Emulator Installation Script for etaHEN
echo "Installing Eden Nintendo Switch Emulator..."

# Check if etaHEN is running
if ! pgrep etahen >/dev/null; then
    echo "Error: etaHEN not detected. Please run etaHEN first."
    exit 1
fi

# Create installation directory
mkdir -p /data/eden
mkdir -p /data/eden/games
mkdir -p /data/eden/saves
mkdir -p /data/eden/config

# Install PKG using etaHEN DPI
echo "Installing PKG via etaHEN..."
curl -X POST -H "Content-Type: application/json" \
     -d '{"url":"file://$(pwd)/eden-emulator-ps5-v1.0.0.pkg"}' \
     http://localhost:9090/install

echo "Installation complete!"
echo "Eden Emulator has been installed and can be launched from the PS5 home screen."
EOF

chmod +x ../$PKG_DIR/install_eden.sh

# Cleanup
rm -rf $PKG_TEMP_DIR
rm -f param.sfo.xml

cd ..

echo "============================================"
echo "   Build Complete!"
echo "============================================"
echo "PKG file: $PKG_DIR/eden-emulator-ps5-v$VERSION.pkg"
echo "Installation script: $PKG_DIR/install_eden.sh"
echo ""
echo "To install on PS5 with etaHEN:"
echo "1. Transfer PKG and install script to PS5"
echo "2. Run: ./install_eden.sh"
echo "3. Launch Eden from PS5 home screen"
echo "============================================"