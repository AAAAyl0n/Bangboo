# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview
"Bangboo" (邦布) is an embedded application framework for ESP32-S3 devices, designed to run on handheld gaming devices or embedded systems with displays. It's built around a modular app-based architecture using the Mooncake framework.

## Build System and Commands
This is a PlatformIO-based project targeting ESP32-S3:

- **Build**: `pio run` (or `platformio run`)
- **Upload to device**: `pio run --target upload`
- **Monitor serial**: `pio device monitor`
- **Clean**: `pio run --target clean`
- **Format source code**: `./format_src.sh` (uses clang-format)

### Development Environment Setup
- Uses PlatformIO with Arduino framework for ESP32-S3
- Board target: `esp32-s3-devkitc-1`
- CPU frequency: 240MHz
- File system: LittleFS
- Build flags include nofrendo NES emulator paths

## Code Architecture

### Core Structure
1. **Main Entry Point**: `src/main.cpp` - Standard Arduino setup()/loop() calling RACHEL framework
2. **RACHEL Framework**: `src/rachel/` - Core application framework
   - `rachel.cpp/h`: Main framework setup, uses Mooncake for app management
   - HAL abstraction layer for hardware
   - App management and lifecycle

### Hardware Abstraction Layer (HAL)
- **Base HAL**: `src/rachel/hal/hal.h` - Abstract base class
- **ESP32 HAL**: `src/rachel/hal/hal_rachel/` - Hardware-specific implementation
- **Simulator HAL**: `src/rachel/hal/hal_simulator/` - For development/testing
- **Components**:
  - Display (LovyanGFX-based)
  - Input system (gamepad buttons)
  - IMU data
  - Audio/buzzer
  - File system
  - Power management
  - System configuration

### Application Framework
Uses **Mooncake** framework for app lifecycle management:
- Apps are packaged in "Packers" (e.g., `AppBangboo_Packer`)
- Launcher app manages other apps
- Each app has its own directory under `src/rachel/apps/`

### Available Apps
Currently installed apps (see `apps.h`):
- **Settings** - System configuration
- **Genshin** - Gaming-related app
- **Bangboo** - Main character/mascot app
- **Timeview** - Clock/time display
- **ASCII Art** - Text art display
- **Poweroff** - System shutdown

Commented out apps (available but not active):
- Nofrendo (NES emulator)
- BLE Gamepad
- Screencast
- Raylib Games
- Music Player
- IMU Test

### Key Dependencies
- **Mooncake**: App framework (v1.0.4)
- **LovyanGFX**: Graphics library (v1.1.12)
- **ArduinoJson**: JSON handling (v6.21.3)
- **ESP32-BLE-Gamepad**: BLE input (v0.5.2)
- **NimBLE-Arduino**: Bluetooth LE (v1.4.1)

### Development Patterns
- Apps are modular and self-contained
- HAL injection pattern for hardware abstraction
- Event-driven architecture through Mooncake
- Resource management with assets embedded as headers
- Configuration system with persistence

## File Structure Notes
- `src/rachel/apps/*/assets/` - App-specific resources (icons, data)
- `src/rachel/apps/utils/` - Shared utilities (UI components, menu systems)
- `prototype/` - Development prototypes and experiments
- Font and asset files typically embedded as C++ headers

## Development Guidelines
- Follow existing app structure when creating new apps
- Use HAL abstraction for hardware access
- Embedded assets should be in app-specific assets folders
- The project uses Chinese comments in some places - maintain consistency
- Test both on actual hardware (ESP32-S3) and simulator when possible