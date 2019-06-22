/*
 * user_config.h
 *
 *  Created on: 16.05.2019
 *      Author: bertw
 */

#ifndef MAIN_USER_CONFIG_H_
#define MAIN_USER_CONFIG_H_

#ifndef STM32F1
#define STM32F1
#endif

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define USE_JSON
//#define USE_DLB8
#define USE_WDG

#define USE_BB // use internal pull-up resistors when on bread-board

#define USE_PC_POLLING

#ifdef USE_BB
#define VERSION "rv(bb)-" __DATE__
#else
#define VERSION "rv-" __DATE__
#endif

#define ICACHE_FLASH_ATTR

#endif /* MAIN_USER_CONFIG_H_ */
