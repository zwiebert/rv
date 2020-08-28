#pragma once

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
#define USE_ZONE_NAME

#define USE_MALLOC_IN_NEW_GLOBAL
//#define USE_MALLOC_IN_NEW_NODE
//#define USE_STD_LIST
#define USE_ALLOCATOR_MALLOC

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


