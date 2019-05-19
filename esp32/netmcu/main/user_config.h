/*
 * user_config.h
 *
 *  Created on: 15.05.2019
 *      Author: bertw
 */

#ifndef MAIN_USER_CONFIG_H_
#define MAIN_USER_CONFIG_H_

#define MCU_ESP32

#define USE_LAN
#define USE_HTTP
#define USE_NTP
#define USE_JSON
#define USE_MQTT
#define USE_POSIX_TIME
#define POSIX_TIME
#define USE_SERIAL
#define CONFIG_DICT

#define DISTRIBUTION 1

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "userio/inout.h"
#include "esp32/rom/ets_sys.h"

#define ICACHE_FLASH_ATTR
//#define vrbDebug 0

#endif /* MAIN_USER_CONFIG_H_ */
