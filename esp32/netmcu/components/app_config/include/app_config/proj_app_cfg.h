// User configuration
#ifndef PROJ_APP_CONFIG_H_
#define PROJ_APP_CONFIG_H_

#define APP_VERSION "0.5.0"

#define USE_HTTP true
#define USE_LAN true
#define USE_WLAN true
#define USE_WLAN_AP true
#define USE_NTP  true
#define USE_JSON true
#define USE_MQTT true
#define USE_POSIX_TIME true
#define POSIX_TIME true
#define USE_SERIAL true
#define CONFIG_DICT true
#define USE_WDG true
#define USE_FS true
#define USE_HTTP_CLIENT true
#define USE_OTA true
#define USE_STM32OTA true
#define USE_TCPS_TASK true
#define USE_EG true

#define USE_LPH true

#define USE_WS

#ifdef __cplusplus
#include "proj_app_cfg.hh"
extern "C"
#endif
void ws_send_json(const char *json);
#define ws_print_json ws_send_json

#define USE_CLI_MUTEX true
#define USE_NETWORK true

#define USE_STM32COM true


#define STM32_RESET_PIN  (GPIO_NUM_33)
#define STM32_BOOT_PIN (GPIO_NUM_32)
#define STM32_BOOT_PIN_INV config_read_stm32_inv_bootpin()
#define STM32_UART_TX_PIN  (GPIO_NUM_4)
#define STM32_UART_RX_PIN  (GPIO_NUM_36)

#ifndef __cplusplus
#include <stdbool.h>
#endif
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

#define USE_AP_FALLBACK true
#define CHECK_NETWORK_INTERVAL 15
#define PING_INTERVAL 10
#define LOOP_PERIODIC_INTERVAL_MS 100
#define LOOP_INTERVAL_MS 25

enum board { board_ESP32_WLAN, board_OLIMEX_POE, board_OLIMEX_GATEWAY, };

#endif /* PROJ_APP_CONFIG_H_ */
