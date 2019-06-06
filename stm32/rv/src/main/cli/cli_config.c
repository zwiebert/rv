/*
 * cli_config.c
 *
 *  Created on: 21.05.2019
 *      Author: bertw
 */

#include "user_config.h"
//#include "config/config.h"
#include "main/rtc.h"
#include "cli_imp.h"
#include "rtc.h"

#define ENABLE_RESTART 0 // allow software reset


#ifdef CONFIG_GPIO_SIZE
//PIN_DEFAULT=0, PIN_INPUT, PIN_INPUT_PULLUP, PIN_OUTPUT, PIN_ERROR, PIN_READ, PIN_CLEAR, PIN_SET, PIN_TOGGLE

const char pin_state_args[] = "dipo ?01t";
#endif

const char help_parmConfig[]  =
    "'config' sets or gets options. Use: config option=value ...; to set. Use: config option=? ...; to get, if supported\n\n"
    "rtc=(ISO_TIME|?)   set local time\n"
    "baud=(N|?)         serial baud rate\n"
    "verbose=(0..5|?)   diagnose output verbosity level\n"
#if ENABLE_RESTART
    "restart            restart MCU\n"
#endif
;

int ICACHE_FLASH_ATTR
process_parmConfig(clpar p[], int len) {
  int arg_idx;
  int errors = 0;

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL || val == NULL) {  // don't allow any default values in config
      return reply_failure();

#if ENABLE_RESTART
    } else if (strcmp(key, "restart") == 0) {
      extern void  mcu_restart(void);
      mcu_restart();
#endif

    } else if (strcmp(key, "time") == 0) {
    	rtc_set_counter_val(atol(val));
    } else {
      ++errors;
    }
  }

  reply(errors==0);
  return 0;
}

