#pragma once

#include <stdint.h>
typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;

#if defined TEST_HOST
#include "app/config/host/proj_app_cfg.h"

#elif defined MCU_ESP8266
#include "app/config/esp8266/proj_app_cfg.h"

#elif defined MCU_ESP32
#include "app/config/esp32/proj_app_cfg.h"

#else
#error "no supported MCU"

#endif

#include "proj_kconfig.h"



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
//#include "esp32/rom/ets_sys.h"
//#include "misc/int_types.h"
//#include "txtio/inout.h"

#define ISO_BUILD_TIME (__DATE__ "T" __TIME__)

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL 3
#endif


#define CFG_NAMESPACE "Tronferno"


#define CHECK_NETWORK_INTERVAL 15
#define PING_INTERVAL 10
#define LOOP_PERIODIC_INTERVAL_MS 100
#define LOOP_INTERVAL_MS 25

enum board { board_ESP32_WLAN, board_OLIMEX_POE, board_OLIMEX_GATEWAY, };

#ifdef __cplusplus
#include <misc/cstring_utils.hh>
#define STRLCPY(dst,src,size) csu_copy((dst),(size),(src))
#define STRCPY(dst,src) csu_copy((dst),(src))
#else
#define STRLCPY(dst,src,size) strlcpy((dst),(src),(size))
#define STRCPY(dst,src) strcpy((dst),(src))
#endif
