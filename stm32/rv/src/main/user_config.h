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
#define USE_DLB8
#define USE_WDG

#define USE_BB

#define ICACHE_FLASH_ATTR

#endif /* MAIN_USER_CONFIG_H_ */
