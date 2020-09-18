#pragma once

constexpr char app_VERSION[] = "0.5.0";

constexpr bool use_HTTP = true;
constexpr bool use_LAN = true;
constexpr bool use_WLAN = true;
constexpr bool use_WLAN_AP = true;
constexpr bool use_NTP = true;
constexpr bool use_JSON = true;
constexpr bool use_MQTT = true;
constexpr bool use_POSIX_TIME = true;
constexpr bool use_SERIAL = true;
constexpr bool use_WDG = true;
constexpr bool use_FS = true;
constexpr bool use_HTTP_GET = false;
constexpr bool use_HTTP_CLIENT = true;
constexpr bool use_OTA = true;
constexpr bool use_STM32OTA = true;
constexpr bool use_TCPS_TASK = true;
constexpr bool use_TCPS = false;
constexpr bool use_EG = true;
constexpr bool use_LPH = true;
constexpr bool use_WS = true;
constexpr bool use_CLI_MUTEX = true;
constexpr bool use_NETWORK = true;
constexpr bool use_STM32COM = true;
constexpr bool use_AP_FALLBACK = true;
#if 0
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "esp32/rom/ets_sys.h"
#include "misc/int_types.h"
#include "txtio/inout.h"

#define MCU_TYPE "esp32"
#define ISO_BUILD_TIME (__DATE__ "T" __TIME__)

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL 3
#endif

#define STM32_RESET_PIN  (GPIO_NUM_33)
#define STM32_BOOT_PIN (GPIO_NUM_32)
#define STM32_BOOT_PIN_INV config_read_stm32_inv_bootpin()
#define STM32_UART_TX_PIN  (GPIO_NUM_4)
#define STM32_UART_RX_PIN  (GPIO_NUM_36)

#define CHECK_NETWORK_INTERVAL 15
#define PING_INTERVAL 10
#define LOOP_PERIODIC_INTERVAL_MS 100
#define LOOP_INTERVAL_MS 25

enum board { board_ESP32_WLAN, board_OLIMEX_POE, board_OLIMEX_GATEWAY, };

#endif
