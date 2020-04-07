// User configuration
#ifndef PROJ_APP_CONFIG_H_
#define PROJ_APP_CONFIG_H_

#define APP_VERSION "0.5.0"

#define USE_HTTP
#define USE_LAN
#define USE_WLAN
#define USE_WLAN_AP
#define USE_NTP
#define USE_JSON
#define USE_MQTT
#define USE_POSIX_TIME
#define POSIX_TIME
#define USE_SERIAL
#define CONFIG_DICT
#define USE_WDG
#define USE_FS
#define USE_HTTP_CLIENT
#define USE_OTA
//#define USE_TCPS
#define USE_EG

#define USE_CLI_MUTEX
#define USE_NETWORK


#define STM32_RESET_PIN  (GPIO_NUM_33)
#define STM32_BOOT_PIN (GPIO_NUM_32)
#define STM32_BOOT_PIN_INV C.stm32_inv_bootpin
#define STM32_UART_TX_PIN  (GPIO_NUM_4)
#define STM32_UART_RX_PIN  (GPIO_NUM_36)


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

#define USE_AP_FALLBACK
#define CHECK_NETWORK_INTERVAL 15
#define PING_INTERVAL 10
#define LOOP_PERIODIC_INTERVAL_MS 100
#define LOOP_INTERVAL_MS 25

enum board { board_ESP32_WLAN, board_OLIMEX_POE, board_OLIMEX_GATEWAY, };

#endif /* PROJ_APP_CONFIG_H_ */
