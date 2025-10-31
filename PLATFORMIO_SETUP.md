# PlatformIO Setup Guide

This project has been configured to work with PlatformIO for easier dependency management and building.

## Quick Start

### Installation

1. **Install PlatformIO Core**
   ```bash
   pip install platformio
   ```

   Or install PlatformIO IDE extension in VS Code.

2. **Clone and Build**
   ```bash
   git clone https://github.com/yourusername/tesla-bms-esp32s3.git
   cd tesla-bms-esp32s3
   pio run
   ```

3. **Upload to Board**
   ```bash
   pio run --target upload
   ```

4. **Monitor Serial Output**
   ```bash
   pio device monitor
   ```

## Project Structure

The project now follows PlatformIO conventions:

```
tesla-bms-esp32s3/
├── platformio.ini      # Project configuration
├── src/                # Source files (.cpp, .c)
│   └── main.cpp        # Main application (renamed from .ino)
├── include/            # Header files (.h)
└── lib/                # Custom libraries (optional)
```

**Legacy Support:** The original Arduino IDE files remain in the root directory for backward compatibility.

## Configuration

### platformio.ini

The `platformio.ini` file contains all build configuration:

```ini
[env:lilygo-t-display-s3]
platform = espressif32
board = lilygo-t-display-s3
framework = arduino
monitor_speed = 115200
lib_deps = lvgl/lvgl@^8.3.0
```

### Board Settings

- **Target:** LilyGO T-Display-S3 (ESP32-S3)
- **Framework:** Arduino
- **Upload Speed:** 921600 baud
- **Monitor Speed:** 115200 baud
- **CPU Frequency:** 240 MHz
- **Flash Mode:** QIO

### Build Flags

- `CORE_DEBUG_LEVEL=3` - Enable verbose debugging
- `BOARD_HAS_PSRAM` - Enable PSRAM support
- `ARDUINO_USB_CDC_ON_BOOT=1` - Enable USB CDC on boot
- `ARDUINO_USB_MODE=1` - Use USB mode

## Common Commands

### Build
```bash
pio run
```

### Upload
```bash
pio run --target upload
```

### Clean Build
```bash
pio run --target clean
```

### Serial Monitor
```bash
pio device monitor
```

### Build + Upload + Monitor
```bash
pio run --target upload && pio device monitor
```

### Update Libraries
```bash
pio lib update
```

### List Devices
```bash
pio device list
```

## VS Code Integration

If using VS Code with PlatformIO IDE extension:

1. Open project folder: `File > Open Folder`
2. PlatformIO will automatically detect `platformio.ini`
3. Use the PlatformIO toolbar at the bottom:
   - ✓ Build
   - → Upload
   - 🔌 Monitor
   - 🗑️ Clean

## Troubleshooting

### "Board not found"

If PlatformIO doesn't recognize `lilygo-t-display-s3`:

1. Update platform: `pio pkg update -p espressif32`
2. Or specify platform version in `platformio.ini`:
   ```ini
   platform = espressif32@^6.0.0
   ```

### Upload Issues

If upload fails:
- Hold BOOT button while uploading
- Try lower upload speed: `upload_speed = 115200`
- Check USB cable (must support data transfer)
- Verify port in `platformio.ini` or use auto-detect

### Library Dependencies

LVGL is automatically installed by PlatformIO. If issues occur:
```bash
pio lib install lvgl@^8.3.0
```

### Missing Files

If you see "file not found" errors:
- Verify files are in `src/` and `include/` directories
- Check that `src/main.cpp` exists (copy of .ino file)
- Ensure all header files are in `include/`

## Comparison: Arduino IDE vs PlatformIO

| Feature | Arduino IDE | PlatformIO |
|---------|-------------|------------|
| Dependency Management | Manual | Automatic |
| Build Speed | Slower | Faster |
| Multi-Project | Difficult | Easy |
| Version Control | Limited | Excellent |
| CI/CD Integration | Hard | Native |
| Testing | Limited | Built-in |

## Migration Notes

### File Locations

- **Arduino IDE:** All files in root directory
- **PlatformIO:**
  - Source: `src/` directory
  - Headers: `include/` directory
  - Libraries: `lib/` or auto-managed

### Include Paths

PlatformIO automatically includes:
- `include/` directory
- Library headers from `lib_deps`
- Framework headers (Arduino.h, etc.)

No changes needed to `#include` statements.

### Building Both Ways

The project supports both build systems:

- **PlatformIO:** Use `src/` and `include/` directories
- **Arduino IDE:** Use root directory files

Files are duplicated to maintain compatibility.

## Advanced Configuration

### Custom Build Flags

Add to `platformio.ini`:
```ini
build_flags =
    -DCORE_DEBUG_LEVEL=3
    -DMY_CUSTOM_FLAG=1
```

### Multiple Environments

Support different boards:
```ini
[env:lilygo-t-display-s3]
board = lilygo-t-display-s3
...

[env:esp32-dev]
board = esp32dev
...
```

Build specific environment:
```bash
pio run -e esp32-dev
```

### Serial Port Override

Specify port in `platformio.ini`:
```ini
upload_port = /dev/ttyUSB0
monitor_port = /dev/ttyUSB0
```

Or on command line:
```bash
pio run --target upload --upload-port /dev/ttyUSB0
```

## Resources

- **PlatformIO Docs:** https://docs.platformio.org
- **ESP32 Platform:** https://docs.platformio.org/en/latest/platforms/espressif32.html
- **Arduino Framework:** https://docs.platformio.org/en/latest/frameworks/arduino.html
- **Board Definition:** Search "lilygo-t-display-s3" on https://registry.platformio.org

## Support

For issues specific to:
- **PlatformIO setup:** Check PlatformIO documentation
- **Project code:** See main README.md
- **Hardware:** See T-Display-S3 documentation
