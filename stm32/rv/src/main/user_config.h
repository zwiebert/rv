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

#define USE_TEST
#define USE_JSON
#define USE_WDG
#define USE_PC_POLLING

#ifdef BUILD_DEBUG
#undef USE_TEST
#define USE_BB // use internal pull-up resistors when on bread-board
#define FAKE_PC // fake pressure control input when valves are active
#define VERSION "rv(breadboard)-" __DATE__ " " __TIME__
//#error "debug"
#elif defined BUILD_RELEASE
//#error "release"
#undef USE_TEST
#define VERSION "rv(release)-" __DATE__ " " __TIME__
#elif defined BUILD_CELLAR
//#error "cellar"
#undef USE_TEST
#define VERSION "rv(cellar)-" __DATE__ " " __TIME__
#else
#error "build version missing"
#endif
#endif /* MAIN_USER_CONFIG_H_ */
