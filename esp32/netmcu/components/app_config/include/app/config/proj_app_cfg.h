#pragma once

#define APP_VERSION "0.5.0"

#include <sdkconfig.h>
#include "proj_kconfig.h"

#define CONFIG_DICT
#define POSIX_TIME USE_POSIX_TIME

#ifdef __cplusplus
//#include "proj_app_cfg.hh"
#endif





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

#define MCU_TYPE "esp32"
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
