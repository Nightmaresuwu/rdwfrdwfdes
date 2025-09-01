# Building Eden for PlayStation 5

This guide explains how to build Eden Nintendo Switch Emulator for PlayStation 5 with etaHEN support.

## Prerequisites

### Required Tools

1. **PS5 Development Environment**
   - Orbis SDK (Sony's official PS5 SDK) OR
   - PS5 Homebrew Toolchain (community-developed)
   - etaHEN 2.2B or later installed on target PS5

2. **Build Tools**
   - CMake 3.22 or later
   - Clang/LLVM (PS5-compatible version)
   - Git
   - Make or Ninja

3. **Host System**
   - Linux (Ubuntu 20.04+ recommended)
   - 16GB+ RAM recommended
   - 50GB+ free disk space

### Setting Up PS5 SDK

#### Option A: Official Orbis SDK (Recommended)
```bash
# Extract Orbis SDK to /opt/orbis-sdk
sudo mkdir -p /opt/orbis-sdk
sudo chown $(whoami):$(whoami) /opt/orbis-sdk

# Extract your Orbis SDK installation
tar -xf orbis-sdk-*.tar.gz -C /opt/orbis-sdk

# Set environment variable
export PS5_SDK_ROOT="/opt/orbis-sdk"
echo 'export PS5_SDK_ROOT="/opt/orbis-sdk"' >> ~/.bashrc
```

#### Option B: Community Homebrew Toolchain
```bash
# Clone community PS5 toolchain
git clone https://github.com/ps5-homebrew/toolchain.git ps5-toolchain
cd ps5-toolchain

# Build toolchain
./build_toolchain.sh

# Set environment
export PS5_SDK_ROOT="$(pwd)/install"
```

## Building Process

### 1. Clone Repository

```bash
git clone https://git.eden-emu.dev/eden-emu/eden-ps5.git
cd eden-ps5

# Initialize submodules if any
git submodule update --init --recursive
```

### 2. Install Dependencies

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install -y \
    cmake \
    build-essential \
    git \
    ninja-build \
    python3 \
    python3-pip \
    curl \
    wget
```

#### Arch Linux
```bash
sudo pacman -S cmake gcc git ninja python python-pip curl wget
```

### 3. Configure Build

#### Production Build (PS5 Hardware)
```bash
mkdir build_ps5
cd build_ps5

cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=${PS5_SDK_ROOT}/cmake/orbis-toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DPS5_BUILD=ON \
    -DENABLE_PS5_GNM=ON \
    -DENABLE_PS5_AUDIO=ON \
    -DENABLE_PS5_INPUT=ON \
    -DENABLE_ETAHEN_PLUGIN=ON \
    -DBUILD_PS5_PKG=ON \
    -DCMAKE_INSTALL_PREFIX=install \
    -G Ninja \
    -f ../CMakeLists_PS5.txt
```

#### Development Build (Testing on PC)
```bash
mkdir build_dev
cd build_dev

cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DPS5_BUILD=OFF \
    -DENABLE_PS5_GRAPHICS=OFF \
    -DENABLE_TESTING=ON \
    -G Ninja
```

### 4. Compile

```bash
# For PS5 build
ninja -j $(nproc)

# Or using make
make -j $(nproc)
```

### 5. Create PS5 PKG

#### Automated PKG Creation
```bash
# Return to project root
cd ..

# Make build script executable
chmod +x build_ps5_pkg.sh

# Run PKG builder
./build_ps5_pkg.sh
```

#### Manual PKG Creation
```bash
cd build_ps5

# Create PKG structure
mkdir -p pkg_temp
cp bin/eden-ps5 pkg_temp/eboot.bin
cp ../assets/icon0.png pkg_temp/
cp ../assets/param.sfo pkg_temp/

# Generate PKG (requires PS5 packaging tools)
orbis-pub-gen \
    -contentid UP9000-EDEN00001_00-EDENEMU0000000000 \
    -passcode 00000000000000000000000000000000 \
    pkg_temp \
    eden-emulator-ps5.pkg
```

## Build Configuration Options

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `PS5_BUILD` | OFF | Enable PS5-specific compilation |
| `ENABLE_PS5_GNM` | ON | Enable GNM/GNMX graphics |
| `ENABLE_PS5_AUDIO` | ON | Enable PS5 native audio |
| `ENABLE_PS5_INPUT` | ON | Enable DualSense support |
| `ENABLE_ETAHEN_PLUGIN` | ON | Enable etaHEN integration |
| `BUILD_PS5_PKG` | ON | Build installable PKG |
| `ENABLE_TESTING` | OFF | Build test suite |
| `ENABLE_DEBUG_SYMBOLS` | OFF | Include debug symbols |

### Build Types

#### Release
- Optimized for performance
- No debug symbols
- Suitable for distribution

```bash
-DCMAKE_BUILD_TYPE=Release
```

#### Debug
- Debug symbols included
- No optimizations
- Suitable for development

```bash
-DCMAKE_BUILD_TYPE=Debug
```

#### RelWithDebInfo
- Optimized but with debug info
- Good for profiling

```bash
-DCMAKE_BUILD_TYPE=RelWithDebInfo
```

## Cross-Compilation Notes

### Target Architecture
- **CPU**: AMD Zen 2 (x86_64)
- **GPU**: Custom RDNA2
- **Endian**: Little-endian
- **ABI**: System V AMD64

### Compiler Flags
```cmake
# Automatically set by PS5 toolchain
-march=znver2
-mtune=znver2
-msse4.2
-mavx2
-DPS5_PLATFORM=1
```

### Linking
```cmake
# PS5-specific libraries
SceKernel_stub_weak
SceGnmDriver_stub_weak
ScePad_stub_weak
SceAudioOut_stub_weak
```

## Troubleshooting

### Common Issues

#### 1. SDK Not Found
```
CMake Error: Could not find PS5 SDK
```

**Solution:**
```bash
# Verify SDK path
ls $PS5_SDK_ROOT

# Set correct path
export PS5_SDK_ROOT="/path/to/your/sdk"
```

#### 2. Compilation Errors
```
error: 'sceGnmSubmitCommandBuffers' was not declared
```

**Solution:**
- Ensure you have the correct SDK version
- Check that all PS5 headers are included
- Verify toolchain configuration

#### 3. Linking Errors
```
undefined reference to `sceKernelAllocateDirectMemory`
```

**Solution:**
```cmake
# Add missing libraries to CMakeLists.txt
target_link_libraries(eden-ps5
    SceKernel_stub_weak
    SceLibc_stub_weak
)
```

#### 4. PKG Generation Fails
```
orbis-pub-gen: command not found
```

**Solution:**
```bash
# Install PS5 publishing tools
# Or use alternative packaging method
tar -czf eden-emulator-ps5.pkg -C pkg_temp .
```

### Build Performance

#### Parallel Compilation
```bash
# Use all CPU cores
ninja -j $(nproc)

# Or specific number
ninja -j 8
```

#### Fast Debug Builds
```bash
# Skip unnecessary features for faster builds
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DENABLE_TESTING=OFF \
    -DENABLE_BENCHMARKS=OFF \
    -DSKIP_DOCUMENTATION=ON
```

#### Incremental Builds
```bash
# Only rebuild changed files
ninja

# Clean and rebuild everything
ninja clean
ninja
```

## Installing on PS5

### Prerequisites on PS5
1. PlayStation 5 with Jailbreak 5.50
2. etaHEN 2.2B+ installed and running
3. FTP access enabled (for file transfer)

### Installation Methods

#### Method 1: etaHEN Direct PKG Installer
```bash
# Transfer PKG to PS5 via FTP/USB
# Use etaHEN's web interface at http://PS5_IP:12800
# Or use command line:

curl -X POST -H "Content-Type: application/json" \
     -d '{"url":"file:///data/eden/eden-emulator-ps5.pkg"}' \
     http://PS5_IP:9090/install
```

#### Method 2: Manual Installation
```bash
# Copy files via FTP
mkdir -p /data/eden
cp eden-emulator-ps5.pkg /data/eden/

# Install using provided script
./install_eden.sh
```

### Verification
```bash
# Check if Eden is installed
ls /system_data/priv/appmeta/

# Launch from PS5 home screen
# Or via etaHEN ELF loader on port 9021
```

## Development Workflow

### 1. Code → Build → Test Cycle
```bash
# Make changes to source code
vim src_ps5/ps5_graphics/ps5_renderer.cpp

# Rebuild only changed components
ninja

# Deploy to PS5 (if connected)
rsync -av build_ps5/bin/eden-ps5 ps5:/data/eden/
```

### 2. Remote Debugging
```bash
# Enable PS5Debug via etaHEN
# Connect GDB to PS5
orbis-gdb build_ps5/bin/eden-ps5
(gdb) target remote PS5_IP:2159
```

### 3. Performance Profiling
```bash
# Build with profiling enabled
cmake .. -DENABLE_PROFILING=ON

# Analyze performance on PS5
# Results available via etaHEN web interface
```

## Continuous Integration

### GitHub Actions Example
```yaml
name: Build PS5
on: [push, pull_request]
jobs:
  build-ps5:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Setup PS5 SDK
        run: |
          # Download and setup PS5 toolchain
          ./scripts/setup_ps5_ci.sh
      - name: Build
        run: |
          ./build_ps5_pkg.sh
      - name: Upload PKG
        uses: actions/upload-artifact@v3
        with:
          name: eden-ps5-pkg
          path: pkg_output/*.pkg
```

---

## Additional Resources

- [PS5 Homebrew Documentation](https://ps5-homebrew.github.io/)
- [etaHEN Wiki](https://github.com/LightningMods/etaHEN/wiki)
- [Orbis SDK Reference](https://ps5-homebrew.github.io/orbis-sdk/)
- [Eden Development Guide](https://git.eden-emu.dev/eden-emu/eden/wiki)

---

**Happy Building! 🛠️**