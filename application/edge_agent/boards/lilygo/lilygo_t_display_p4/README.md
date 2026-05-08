# LilyGO T-Display-P4 Board Support

## **English | [中文](./README_CN.md)**

Upstream board repository:
[T-Display-P4](https://github.com/Xinyuan-LilyGO/T-Display-P4)

## Overview

The T-Display-P4 board integration mainly uses `lilygo_device_driver`, which
handles the low-level board bring-up flow.

Adapted custom devices:

| Device name | Purpose |
| --- | --- |
| `power_ctrl` | Starts the whole board driver once, using the T-Display-P4 async initialization flow. |
| `display_lcd` | Exposes the initialized MIPI LCD panel handle to esp-claw UI and Lua display modules. |
| `lcd_brightness` | Exposes board-specific screen brightness control through `lilygo_device_driver`. |

## Directory Layout

| File | Description |
| --- | --- |
| `board_info.yaml` | Board name, chip, manufacturer, and description metadata. |
| `board_devices.yaml` | Board manager device list and component dependencies. |
| `board_peripherals.yaml` | Intentionally empty because the LilyGO driver owns the T-Display-P4 buses and power sequence. |
| `Kconfig.projbuild` | Screen type, camera type, pixel format, board type, and board version options. |
| `sdkconfig.defaults.board` | Default configuration for flash, PSRAM, display support, Hosted Wi-Fi, camera, and logs. |
| `setup_device.cpp` | Custom device implementation. |
| `gen_bmgr_config_codes.py` | Board-local helper script for generating board-manager code. |

## Quick Start

### Generate Board Manager Files

Run this command from the board directory:

```powershell
python gen_bmgr_config_codes.py
```

The script automatically selects `lilygo_t_display_p4` and scans the project
`boards` directory. Generated files are written to:

```text
components/gen_bmgr_codes
```

> [!IMPORTANT]
> `managed_components\espressif__esp_board_manager` must exist before running
> this command, otherwise generation will fail.

### Required sdkconfig Configuration

Make sure the board defaults are merged into the project configuration. The
generated sdkconfig defaults are written to:

```text
components/gen_bmgr_codes/board_manager.defaults
```

### Board Options

The board-specific Kconfig menu provides these options:

| Option group | Choices |
| --- | --- |
| Camera type | `OV2710`, `SC2336`, `OV5645` |
| Screen type | `HI8561`, `RM69A10` |
| Screen pixel format | `RGB565`, `RGB888` |
| Camera pixel format | `RGB565`, `RGB888` |
| Board type | `T-Display-P4`, `T-Display-P4-Keyboard` |
| Board version | `V1.0`, `V2.0` |

Default configuration:

- `OV2710`
- `HI8561`
- screen format `RGB565`
- camera format `RGB565`
- `T-Display-P4`
- `V2.0`

If you use another screen, camera, or board version, adjust the settings in
`menuconfig`.
