/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "../../../managed_components/espressif__esp_board_manager/devices/dev_display_lcd/dev_display_lcd.h"
#include "esp_board_device.h"
#include "esp_board_manager_defs.h"
#include "esp_check.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gen_board_device_custom.h"
#include "lilygo_device_driver_library.h"

namespace lilygo_t_display_p4 {

constexpr char kTag[] = "lilygo_t_display_p4";
constexpr int kReadyTimeoutMs = 5000;
constexpr int kReadyPollMs = 10;
constexpr uint8_t kDefaultBrightnessPercent = 100;

struct BrightnessHandle {
  bool (*set_percent)(uint8_t percent);
  uint8_t percent;
};

dev_display_lcd_handles_t g_screen_handles;
bool SetBrightnessPercent(uint8_t percent);
BrightnessHandle g_brightness_handle = {
    .set_percent = SetBrightnessPercent,
    .percent = kDefaultBrightnessPercent,
};

const dev_display_lcd_config_t kScreenConfig = {
    .name = ESP_BOARD_DEVICE_NAME_DISPLAY_LCD,
#if defined(CONFIG_SCREEN_TYPE_HI8561)
    .chip = "hi8561",
#elif defined(CONFIG_SCREEN_TYPE_RM69A10)
    .chip = "rm69a10",
#else
    .chip = "unknown",
#endif
    .sub_type = "dsi",
    .lcd_width = SCREEN_WIDTH,
    .lcd_height = SCREEN_HEIGHT,
    .swap_xy = 0,
    .mirror_x = 0,
    .mirror_y = 0,
    .need_reset = 0,
    .invert_color = 0,
    .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
    .data_endian = LCD_RGB_DATA_ENDIAN_LITTLE,
    .bits_per_pixel = SCREEN_BITS_PER_PIXEL,
};

bool GetScreenStatus(const lilygo_device_driver::TDisplayP4Driver& driver) {
#if defined(CONFIG_SCREEN_TYPE_HI8561)
  return driver.status().hi8561.init_flag;
#elif defined(CONFIG_SCREEN_TYPE_RM69A10)
  return driver.status().rm69a10.init_flag;
#else
  return false;
#endif
}

bool GetScreenReadyStatus(lilygo_device_driver::TDisplayP4Driver& driver) {
  int elapsed_ms = 0;
  while (elapsed_ms <= kReadyTimeoutMs) {
    if (GetScreenStatus(driver)) {
      g_screen_handles.panel_handle =
          driver.bus().screen_mipi_bus->device_handle();
      g_screen_handles.io_handle = nullptr;
      if (g_screen_handles.panel_handle != nullptr) {
        return true;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(kReadyPollMs));
    elapsed_ms += kReadyPollMs;
  }

  return false;
}

bool GetBrightnessStatus(const lilygo_device_driver::TDisplayP4Driver& driver) {
#if defined(CONFIG_SCREEN_TYPE_HI8561)
  return driver.status().hi8561_backlight.init_flag;
#elif defined(CONFIG_SCREEN_TYPE_RM69A10)
  return driver.status().rm69a10.init_flag;
#else
  return false;
#endif
}

bool GetBrightnessReadyStatus(lilygo_device_driver::TDisplayP4Driver& driver) {
#if defined(CONFIG_SCREEN_TYPE_HI8561)
  int elapsed_ms = 0;
  while (elapsed_ms <= kReadyTimeoutMs) {
    if (GetBrightnessStatus(driver)) {
      return true;
    }

    vTaskDelay(pdMS_TO_TICKS(kReadyPollMs));
    elapsed_ms += kReadyPollMs;
  }

  return false;
#elif defined(CONFIG_SCREEN_TYPE_RM69A10)
  return GetScreenReadyStatus(driver);
#else
  return false;
#endif
}

bool SetBrightnessPercent(uint8_t percent) {
  if (percent > 100) {
    percent = 100;
  }

  lilygo_device_driver::TDisplayP4Driver& driver =
      lilygo_device_driver::TDisplayP4Driver::GetInstance();

#if defined(CONFIG_SCREEN_TYPE_HI8561)
  if (!driver.status().hi8561_backlight.init_flag ||
      !driver.chip().hi8561_backlight->SetDuty(percent)) {
    return false;
  }
#elif defined(CONFIG_SCREEN_TYPE_RM69A10)
  if (!driver.status().rm69a10.init_flag) {
    return false;
  }
  const uint8_t brightness =
      static_cast<uint8_t>((static_cast<uint16_t>(percent) * 255) / 100);
  if (!driver.chip().rm69a10->SetBrightness(brightness)) {
    return false;
  }
#else
  return false;
#endif

  g_brightness_handle.percent = percent;
  return true;
}

int PowerInit(void*, int, void** device_handle) {
  static bool initialized = false;

  ESP_RETURN_ON_FALSE(device_handle != nullptr, ESP_ERR_INVALID_ARG, kTag,
      "device_handle is NULL");

  lilygo_device_driver::TDisplayP4Driver& driver =
      lilygo_device_driver::TDisplayP4Driver::GetInstance();
  if (!initialized) {
    driver.Init(lilygo_device_driver::TDisplayP4Driver::InitMode::kAsync);
    initialized = true;
  }

  *device_handle = &driver;
  return ESP_OK;
}

int PowerDeinit(void*) { return ESP_OK; }

int ScreenInit(void*, int, void** device_handle) {
  ESP_RETURN_ON_FALSE(device_handle != nullptr, ESP_ERR_INVALID_ARG, kTag,
      "device_handle is NULL");

  if (g_screen_handles.panel_handle != nullptr) {
    *device_handle = &g_screen_handles;
    return ESP_OK;
  }

  lilygo_device_driver::TDisplayP4Driver& driver =
      lilygo_device_driver::TDisplayP4Driver::GetInstance();

  ESP_RETURN_ON_FALSE(GetScreenReadyStatus(driver), ESP_ERR_TIMEOUT, kTag,
      "GetScreenReadyStatus failed");

  esp_err_t ret = esp_board_device_update_config(
      ESP_BOARD_DEVICE_NAME_DISPLAY_LCD, &kScreenConfig);
  ESP_RETURN_ON_ERROR(ret, kTag, "esp_board_device_update_config failed");

  *device_handle = &g_screen_handles;
  return ESP_OK;
}

int ScreenDeinit(void*) {
  g_screen_handles = {};
  return ESP_OK;
}

int BrightnessInit(void*, int, void** device_handle) {
  ESP_RETURN_ON_FALSE(device_handle != nullptr, ESP_ERR_INVALID_ARG, kTag,
      "device_handle is NULL");

  lilygo_device_driver::TDisplayP4Driver& driver =
      lilygo_device_driver::TDisplayP4Driver::GetInstance();

  ESP_RETURN_ON_FALSE(GetBrightnessReadyStatus(driver), ESP_ERR_INVALID_STATE,
      kTag, "GetBrightnessReadyStatus failed");

  ESP_RETURN_ON_FALSE(SetBrightnessPercent(kDefaultBrightnessPercent), ESP_FAIL,
      kTag, "SetBrightnessPercent failed");

  *device_handle = &g_brightness_handle;
  return ESP_OK;
}

int BrightnessDeinit(void*) { return ESP_OK; }

}  // namespace lilygo_t_display_p4

CUSTOM_DEVICE_IMPLEMENT(power_ctrl, lilygo_t_display_p4::PowerInit,
    lilygo_t_display_p4::PowerDeinit);
CUSTOM_DEVICE_IMPLEMENT(display_lcd, lilygo_t_display_p4::ScreenInit,
    lilygo_t_display_p4::ScreenDeinit);
CUSTOM_DEVICE_IMPLEMENT(lcd_brightness, lilygo_t_display_p4::BrightnessInit,
    lilygo_t_display_p4::BrightnessDeinit);
