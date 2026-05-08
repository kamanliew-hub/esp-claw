# LilyGO T-Display-P4 板子支持说明

## **[English](./README.md) | 中文**

板子源仓库：
[T-Display-P4](https://github.com/Xinyuan-LilyGO/T-Display-P4)

## 概览

T-Display-P4 的适配主要通过 `lilygo_device_driver` 完成，这个驱动负责
板子的底层启动流程。

已适配自定义设备：

| 设备名 | 作用 |
| --- | --- |
| `power_ctrl` | 启动整板驱动，只调用一次 T-Display-P4 的异步初始化流程。 |
| `display_lcd` | 将已经初始化好的 MIPI LCD panel handle 暴露给 esp-claw UI 和 Lua 显示模块。 |
| `lcd_brightness` | 通过 `lilygo_device_driver` 暴露板子专用的屏幕亮度控制。 |

## 目录结构

| 文件 | 说明 |
| --- | --- |
| `board_info.yaml` | 板子名称、芯片、厂商和描述信息。 |
| `board_devices.yaml` | board manager 设备列表和组件依赖。 |
| `board_peripherals.yaml` | 目前刻意留空，因为 T-Display-P4 的总线和上电流程由 LilyGO 驱动统一管理。 |
| `Kconfig.projbuild` | 屏幕类型、摄像头类型、像素格式、板子类型和板子版本选项。 |
| `sdkconfig.defaults.board` | Flash、PSRAM、屏幕支持、Hosted Wi-Fi、摄像头和日志相关默认配置。 |
| `setup_device.cpp` | 自定义设备的实现。 |
| `gen_bmgr_config_codes.py` | 本板子专用的 board-manager 代码生成辅助脚本。 |

## 快速开始

### 生成 Board Manager 文件

进入本板子目录后运行：

```powershell
python gen_bmgr_config_codes.py
```

这个脚本会自动选择 `lilygo_t_display_p4`，并扫描项目中的 `boards` 目录。
生成结果会输出到：

```text
components/gen_bmgr_codes
```

> [!IMPORTANT]
> 命令执行前需要有managed_components\espressif__esp_board_manager库文件存在否则会生成失败

### 必要 sdkconfig 配置

请确认板子默认配置已经合并到工程配置中。生成的sdkconfig路径：

```text
components/gen_bmgr_codes/board_manager.defaults
```

### 板子选项

本板子的 Kconfig 菜单提供以下选项：

| 选项组 | 可选项 |
| --- | --- |
| 摄像头类型 | `OV2710`、`SC2336`、`OV5645` |
| 屏幕类型 | `HI8561`、`RM69A10` |
| 屏幕像素格式 | `RGB565`、`RGB888` |
| 摄像头像素格式 | `RGB565`、`RGB888` |
| 板子类型 | `T-Display-P4`、`T-Display-P4-Keyboard` |
| 板子版本 | `V1.0`、`V2.0` |

默认配置为：

- `OV2710`
- `HI8561`
- 屏幕格式 `RGB565`
- 摄像头格式 `RGB565`
- `T-Display-P4`
- `V2.0`

如果你使用的是其他屏幕、摄像头或板子版本，请在 `menuconfig` 中调整。
