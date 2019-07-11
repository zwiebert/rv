/*
 * user_config.h
 *
 *  Created on: 15.05.2019
 *      Author: bertw
 */

#ifndef MAIN_USER_CONFIG_H_
#define MAIN_USER_CONFIG_H_

#define MCU_ESP32

#define USE_HTTP
#define USE_LAN
#define USE_WLAN
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


#ifdef BOARD_ESP32WLAN
#undef USE_LAN
#elif defined BOARD_ESP32POE
#undef USE_WLAN
#define STM32_BOOT_PIN_REVERSE
#define OLIMEX_ESP32_POE
#define DISTRIBUTION 1
#elif defined BOARD_ESP32GATEWAY
#undef USE_WLAN
#define OLIMEX_ESP32_GATEWAY
#define DISTRIBUTION 1
#else
#error // no board defined
#endif



#define STM32_RESET_PIN  (GPIO_NUM_33)
#define STM32_BOOT_PIN (GPIO_NUM_32)

#define STM32_UART_TX_PIN  (GPIO_NUM_4)
#define STM32_UART_RX_PIN  (GPIO_NUM_36)


#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "userio/inout.h"
#include "esp32/rom/ets_sys.h"

#define ICACHE_FLASH_ATTR
//#define vrbDebug 0

#endif /* MAIN_USER_CONFIG_H_ */
