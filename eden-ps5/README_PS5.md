# Eden Nintendo Switch Emulator - PlayStation 5 Port

![Eden PS5](https://img.shields.io/badge/Platform-PlayStation_5-blue?style=for-the-badge&logo=playstation)
![etaHEN](https://img.shields.io/badge/Homebrew-etaHEN-red?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-In_Development-orange?style=for-the-badge)

**Eden PS5** ist eine vollständige Portierung des Eden Nintendo Switch Emulators auf PlayStation 5 mit Jailbreak 5.50, optimiert für das etaHEN Homebrew-Ökosystem.

## 🎮 Features

### ✨ Core Features
- **Native PS5 Graphics**: GNM/GNMX Grafik-APIs statt Vulkan/OpenGL
- **PS5 Audio System**: Native Audio-Integration ohne SDL/Cubeb
- **DualSense Controller**: Vollständige PS5 Controller-Unterstützung mit haptischem Feedback
- **etaHEN Integration**: Nahtlose Integration mit etaHEN Homebrew-System
- **PKG Installation**: Installierbar als PS5 PKG-Datei

### 🚀 PS5-Specific Optimizations
- **Adaptive Triggers**: Unterstützung für DualSense adaptive Trigger
- **3D Audio**: PlayStation 5 3D-Audio-Unterstützung
- **High Performance Mode**: GPU/CPU Boost für bessere Performance
- **Fast Loading**: Optimiert für PS5 SSD-Geschwindigkeiten

### 🔧 etaHEN Integration Features
- **Plugin System**: Eden als etaHEN Plugin
- **FTP Access**: Dateizugriff über etaHEN FTP-Server
- **Debug Support**: Integration mit PS5Debug
- **Jailbreak Detection**: Automatische Jailbreak-Erkennung und -Aktivierung

## 📋 Systemanforderungen

### PlayStation 5 Requirements
- **PlayStation 5**: Standard oder Digital Edition
- **System Version**: 5.50 (mit Jailbreak)
- **etaHEN**: Version 2.2B oder neuer
- **Freier Speicher**: Mindestens 8GB für ROMs und Save-Dateien

### Entwicklungsumgebung
- **Orbis SDK**: PS5 Entwicklungstools
- **CMake**: Version 3.22 oder neuer
- **Clang**: PS5-kompatible Toolchain
- **Linux**: Ubuntu 20.04+ oder äquivalent

## 🛠️ Kompilierung

### 1. Vorbereitung

```bash
# Clone das Repository
git clone https://git.eden-emu.dev/eden-emu/eden-ps5.git
cd eden-ps5

# PS5 SDK konfigurieren
export PS5_SDK_ROOT="/opt/orbis-sdk"
```

### 2. Dependencies installieren

```bash
# Installiere Build-Dependencies
sudo apt update
sudo apt install cmake build-essential git

# etaHEN SDK (optional, für erweiterte Features)
git clone https://github.com/lightningmods/etaHEN-SDK.git
```

### 3. Build für PS5

```bash
# PS5 PKG erstellen
chmod +x build_ps5_pkg.sh
./build_ps5_pkg.sh
```

### 4. Alternative: Development Build

```bash
# Für Entwicklung/Testing ohne PS5-Hardware
mkdir build_dev
cd build_dev
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

## 📦 Installation auf PS5

### Automatische Installation (mit etaHEN)

1. **etaHEN starten** auf PlayStation 5
2. **PKG und Install-Script** auf PS5 kopieren (via FTP/USB)
3. **Installation ausführen**:
   ```bash
   ./install_eden.sh
   ```

### Manuelle Installation

1. **PKG kopieren** nach `/data/eden/`
2. **etaHEN DPI** verwenden:
   ```bash
   # Via etaHEN Direct PKG Installer
   curl -X POST -H "Content-Type: application/json" \
        -d '{"url":"file:///data/eden/eden-emulator-ps5-v1.0.0.pkg"}' \
        http://localhost:9090/install
   ```

## 🎯 Verwendung

### Erste Schritte

1. **Eden starten** vom PS5 Home Screen
2. **ROMs hinzufügen** nach `/data/eden/games/`
3. **Spiel auswählen** und starten

### Controller-Mapping

| DualSense | Nintendo Switch |
|-----------|-----------------|
| ✖ (Cross) | A |
| ⭕ (Circle) | B |
| ⬜ (Square) | Y |
| 🔺 (Triangle) | X |
| L1/R1 | L/R |
| L2/R2 | ZL/ZR |
| Share | - (Minus) |
| Options | + (Plus) |
| PS Button | Home |
| Touchpad | Capture |

### Erweiterte Features

```bash
# Mit Argumenten starten
./eden-ps5 --game "/data/eden/games/game.nsp"

# Konfiguration anpassen
nano /data/eden/config/settings.ini
```

## 🔧 Architektur

### PS5 Platform Layer
- **ps5_platform**: Grundlegende PS5-System-Integration
- **ps5_graphics**: GNM/GNMX Graphics Abstraction Layer
- **ps5_audio**: Native PS5 Audio-System
- **ps5_input**: DualSense Controller Integration

### etaHEN Integration
- **etahen_integration**: Plugin-System und IPC-Kommunikation
- **Jailbreak Detection**: Automatische Homebrew-Erkennung
- **Service Integration**: FTP, Debug, PKG Installation

### Eden Core Modifications
- **Qt Removal**: Ersetzt durch native PS5 Frontend
- **SDL Removal**: Ersetzt durch PS5 Input/Audio-Systeme
- **Vulkan/OpenGL**: Ersetzt durch GNM/GNMX

## 🚧 Development Status

### ✅ Implemented
- [x] PS5 Platform Layer
- [x] GNM/GNMX Graphics Wrapper
- [x] PS5 Audio System
- [x] DualSense Controller Support
- [x] etaHEN Plugin Integration
- [x] PKG Build System

### 🔄 In Progress
- [ ] Eden Core Integration
- [ ] Shader Translation (Vulkan → GNM)
- [ ] Audio Backend Implementation
- [ ] Controller Haptics
- [ ] Save System Integration

### 📋 Planned
- [ ] Performance Optimizations
- [ ] UI/Frontend Polish
- [ ] Cheat System Integration
- [ ] Network Play (via etaHEN)
- [ ] Advanced 3D Audio

## 🐛 Bekannte Probleme

1. **Shader Compilation**: Vulkan-zu-GNM Übersetzung noch in Arbeit
2. **Performance**: Noch nicht vollständig optimiert
3. **Compatibility**: Nicht alle Spiele funktionieren noch
4. **Memory Management**: GPU-Memory-Allokation benötigt Tuning

## 🤝 Beitragen

### Development Setup
```bash
# Fork das Repository
git clone https://github.com/yourusername/eden-ps5.git

# Feature Branch erstellen
git checkout -b feature/new-feature

# Commit und Push
git commit -am "Add new feature"
git push origin feature/new-feature
```

### Coding Standards
- **C++20** Standard verwenden
- **CMake** für Build-System
- **Conventional Commits** für Commit-Messages
- **Doxygen** für Code-Dokumentation

## 📄 Lizenz

Eden PS5 ist unter der **GPL-3.0-or-later** Lizenz verfügbar.

Original Eden Emulator: Copyright 2025 Eden Emulator Project
PS5 Port: Copyright 2025 Eden Emulator Project - PS5 Port

## 🙏 Credits

### Original Eden Team
- **Camille LaVey** und das Eden Entwicklerteam
- **yuzu Project** (Original Emulator-Basis)

### PS5 Port Contributors
- **etaHEN Team** für Homebrew-Infrastruktur
- **LightningMods** für etaHEN SDK
- **PS5 Homebrew Community**

### Special Thanks
- **sleirsgoevy** für PS5 Exploit-Entwicklung
- **ChendoChap** für PS5 Reverse Engineering
- **buzzer-re** für Homebrew-Tools

---

**⚠️ Disclaimer**: Diese Software ist nur für Homebrew/Emulation-Zwecke gedacht. Verwende nur selbst gekaufte Nintendo Switch Spiele. Die Entwickler übernehmen keine Verantwortung für illegale Nutzung.

---

## 📞 Support

- **Discord**: [Eden Community](https://discord.gg/edenemu)
- **Issues**: GitHub Issues für Bug-Reports
- **Documentation**: [Wiki](https://github.com/eden-emu/eden-ps5/wiki)

**Happy Emulating! 🎮**