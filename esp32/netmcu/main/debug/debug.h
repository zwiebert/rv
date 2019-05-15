/*
 * debug.h
 *
 *  Created on: 15.05.2019
 *      Author: bertw
 */

#ifndef MAIN_DEBUG_DEBUG_H_
#define MAIN_DEBUG_DEBUG_H_

#include "user_config.h"

#ifdef MCU_ESP8266
// FIXME: don't want link an entire libc just for assert()
#define assert(x)
#else
#include <assert.h>
#endif

#ifdef MCU_ESP32
#include <esp32/rom/ets_sys.h>
#define db_puts(s) ets_printf("%s\n",s)
#define db_printf ets_printf
#endif

void abort_precond(const char *msg, int line);
void abort_postcond(const char *msg, int line);

#if 0
#define db_puts(s) io_puts(s)
#endif

#ifdef DISTRIBUTION
#undef db_puts
#define db_puts(s)
#endif



#if defined NO_PRECOND || defined DISTRIBUTION
#define precond(x)
#else
#define precond(x)  assert(x)
#endif


#if defined NO_POSTCOND  || defined DISTRIBUTION
#define postcond(x)
#else
#define postcond(x) assert(x)
#endif





#define dbg_trace()
#define dbg_printf #error

#ifndef dbg_printf
#define dbg_printf io_printf
#endif
#ifndef dbg_trace
#define dbg_trace()   dbg_printf("trace: %s()\n", __func__)
#endif

#ifndef DISTRIBUTION
#define dbg_dbd(x) do { if (C.app_verboseOutput >= vrbDebug) { x; } } while(0)
#else
#define dbg_dbd(x)
#endif

#define if_verbose_do(x) do { if (C.app_verboseOutput >= vrbDebug) { x; } } while(0)

#if defined db_printf && ! defined DISTRIBUTION
#define dbg_vpf(x) do { if (C.app_verboseOutput >= vrbDebug) { x; } } while(0)
#else
#define dbg_vpf(x)
#endif

#endif /* MAIN_DEBUG_DEBUG_H_ */
