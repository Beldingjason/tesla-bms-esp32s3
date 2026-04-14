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
   This builds the default environment: `lilygo-t-display-s3`.

3. **Upload to Board**
   ```bash
   pio run -e lilygo-t-display-s3 --target upload
   ```

   For the Waveshare `ESP32-S3-LCD-1.3-B` target:
   ```bash
   pio run -e esp32-s3-lcd-1_3-b --target upload
   ```

4. **Monitor Serial Output**
   ```bash
   pio device monitor
   ```

## Project Structure

The project now follows PlatformIO conventions:

```
tesla-bms-esp32s3/
├── boards/            # Custom PlatformIO board definitions
│   └── esp32-s3-lcd-1_3-b.json
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
[platformio]
default_envs = lilygo-t-display-s3

[env]
platform = espressif32
framework = arduino
monitor_speed = 115200

[env:lilygo-t-display-s3]
board = lilygo-t-display-s3
build_flags =
    ${env.build_flags}
    -DTESLA_BMS_TARGET_LILYGO_T_DISPLAY_S3

[env:esp32-s3-lcd-1_3-b]
board = esp32-s3-lcd-1_3-b
build_flags =
    ${env.build_flags}
    -DTESLA_BMS_TARGET_ESP32_S3_LCD_1_3_B
```

### Board Settings

- **Default target:** LilyGO T-Display-S3 (ESP32-S3)
- **Additional target:** Waveshare ESP32-S3-LCD-1.3-B
- **Framework:** Arduino
- **Upload Speed:** 921600 baud
- **Monitor Speed:** 115200 baud
- **CPU Frequency:** 240 MHz
- **Flash Mode:** QIO

### Supported Environments

- `lilygo-t-display-s3` - LilyGO T-Display-S3 with the existing 8-bit i80 LCD path
- `esp32-s3-lcd-1_3-b` - Waveshare ESP32-S3-LCD-1.3-B with SPI ST7789 LCD path

### Build Flags

- `CORE_DEBUG_LEVEL=3` - Enable verbose debugging
- `BOARD_HAS_PSRAM` - Enable PSRAM support
- `ARDUINO_USB_CDC_ON_BOOT=1` - Enable USB CDC on boot
- `ARDUINO_USB_MODE=1` - Use USB mode
- `TESLA_BMS_TARGET_LILYGO_T_DISPLAY_S3` - Select LilyGO board pin/display config
- `TESLA_BMS_TARGET_ESP32_S3_LCD_1_3_B` - Select Waveshare board pin/display config

## Common Commands

### Build Default Target
```bash
pio run
```

### Build Specific Target
```bash
pio run -e lilygo-t-display-s3
pio run -e esp32-s3-lcd-1_3-b
```

### Upload
```bash
pio run -e lilygo-t-display-s3 --target upload
pio run -e esp32-s3-lcd-1_3-b --target upload
```

### Clean Build
```bash
pio run -e lilygo-t-display-s3 --target clean
pio run -e esp32-s3-lcd-1_3-b --target clean
```

### Serial Monitor
```bash
pio device monitor -b 115200
```

### Build + Upload + Monitor
```bash
pio run -e esp32-s3-lcd-1_3-b --target upload && pio device monitor -b 115200
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

If PlatformIO doesn't recognize a board environment:

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
- Pass the environment explicitly when you have multiple targets:
  ```bash
  pio run -e esp32-s3-lcd-1_3-b --target upload --upload-port /dev/ttyACM0
  ```

For the Waveshare `esp32-s3-lcd-1_3-b` target specifically:
- The project matches the Waveshare Arduino IDE screenshot settings:
  - board profile based on `ESP32S3 Dev Module`
  - `Flash Mode: QIO 80MHz`
  - `Flash Size: 16MB`
  - `Partition Scheme: Huge APP (3MB No OTA/1MB SPIFFS)`
  - `PSRAM: OPI PSRAM`
  - `USB CDC On Boot: Disabled`
  - `Upload Mode: UART0 / Hardware CDC`
  - `Upload Speed: 921600`
- The board has **no BOOT and no RESET button** — only a USB-C port. It uses a **WCH CH343 USB-UART bridge** (VID:PID `1A86:55D3`), NOT the ESP32-S3's native USB-JTAG. Auto-reset (DTR/RTS → EN/IO0) is wired, so `esptool` can enter download mode on its own; no manual button dance is needed.

#### macOS: install the WCH CH34x driver (required)

macOS's built-in generic USB-CDC driver has timing bugs with the CH343 that cause esptool to fail at the "Uploading stub..." step with:

```
A fatal error occurred: Failed to write to target RAM (result was 01070000: Operation timed out)
```

The chip connects and reports its MAC, but every RAM/flash write times out. The fix is to install the **official WCH CH34x macOS driver**:

- Download: https://www.wch-ic.com/downloads/CH34XSER_MAC_ZIP.html
- Or via Homebrew: `brew install --cask wch-ch34x-usb-serial-driver`
- Reboot after install. The port name may change from `/dev/cu.usbmodem…` to `/dev/cu.wchusbserial…`.

With the official driver installed, uploads work at the standard `921600` baud — no `--no-stub`, no `esp-builtin`, no special flags.

#### Things that do NOT fix it (don't bother)

- Lowering `upload_speed` to 460800 or 115200 — same stub timeout.
- `board_upload.use_1200bps_touch = yes` / `wait_for_upload_port = yes` — no effect, this board doesn't use native USB CDC reset.
- `upload_protocol = esp-builtin` — FAILS with `esp_usb_jtag: could not find or open device!` because the board has no native USB-JTAG, only the CH343 UART bridge.
- `upload_flags = --no-stub` (with or without `--no-compress` on `write_flash`) — gets further but fails with `Failed to write … after seq 0 (result was 01050000: Requested resource not found)`.

All of these are symptoms of the macOS/CH343 driver issue, not configuration problems. Install the WCH driver and the default config works.

#### Other upload issues to try first (non-driver)

- Connect directly to the computer, not through a hub.
- Swap to another short USB data cable (many USB-C cables are charge-only).
- Close any open serial monitors before uploading.
- Test from another host OS (Windows/Linux) to separate a host-side issue from a board issue.

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

[env:esp32-s3-lcd-1_3-b]
board = esp32-s3-lcd-1_3-b
...
```

Build specific environment:
```bash
pio run -e esp32-s3-lcd-1_3-b
```

### Serial Port Override

Specify port in `platformio.ini`:
```ini
upload_port = /dev/ttyUSB0
monitor_port = /dev/ttyUSB0
```

Or on command line:
```bash
pio run -e esp32-s3-lcd-1_3-b --target upload --upload-port /dev/ttyUSB0
```

## Resources

- **PlatformIO Docs:** https://docs.platformio.org
- **ESP32 Platform:** https://docs.platformio.org/en/latest/platforms/espressif32.html
- **Arduino Framework:** https://docs.platformio.org/en/latest/frameworks/arduino.html
- **Board Definitions:** Search for PlatformIO boards on https://registry.platformio.org
- **Custom Board:** `boards/esp32-s3-lcd-1_3-b.json` in this repo

## Support

For issues specific to:
- **PlatformIO setup:** Check PlatformIO documentation
- **Project code:** See main README.md
- **Hardware:** See the T-Display-S3 or Waveshare ESP32-S3-LCD-1.3 documentation, depending on target
