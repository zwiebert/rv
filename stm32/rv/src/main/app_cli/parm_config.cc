/**
 * \file cli/parm_config.cc
 * \brief handle cli command
 */
extern "C" {
#include <time/real_time_clock.h>
#include <time/real_time_clock.h>
#include "user_config.h"
#include "peri/uart.h"
}
#include "cli_imp.h"
#include "loop/loop.hh"
#include "rv/rv_timer.hh"
#include "rv/rv_zones.hh"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

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

enum { ZN, LPH };
static const char *zoneKeysN[] = {
    "zn", "lph", 0
};

bool kvs_store_string(const char *key, const char *val) {
  char buf[128];
  if (0 < std::snprintf(buf, sizeof buf, "{\"to\":\"cli\",\"kvs\":{\"%s\":\"%s\"}};\n", key, val)) {
    esp32_puts(buf);
    return true;
  }
  return false;
}

static int match_zoneKeyN(const char *key) {
  for (int i = 0; zoneKeysN[i]; ++i) {
    if (key == std::strstr(key, zoneKeysN[i]))
      return i;
  }
  return -1;
}

int process_parmConfig(clpar p[], int len, class UoutWriter &td) {
  int arg_idx;
  int errors = 0;
  int needSecondPass = 0;
  int zkIdx = -1;

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL || val == NULL) {  // don't allow any default values in config
      return reply_failure();

#if ENABLE_RESTART
    } else if (std::strcmp(key, "restart") == 0) {
      extern void  mcu_restart(void);
      mcu_restart();
#endif

    } else if (std::strcmp(key, "time") == 0) {
      rtc_setCurrentTime(atol(val));
    } else if (std::strcmp(key, "tz") == 0) {
      setenv("TZ", val, 1);
    } else if ((zkIdx = match_zoneKeyN(key)) >= 0) {
      if (*val == '?') {
        ++needSecondPass;  // handle this on second pass below
      } else {
        if (!kvs_store_string(key, val)) {
          ++errors;
        }
        if (isdigit(key[std::strlen(key) - 1]))
          switch (zkIdx) {
          case ZN: {
            int idx = atoi(key + 2);
            rvz[idx].setName(val);
          }
            break;
          case LPH: {
            int idx = atoi(key + 3);
            rvz[idx].setLph(atoi(val));
            got_zoneData = true;
          }
            break;
          default:
            break;
          }
      }
    } else {
      ++errors;
    }
  }

  reply(errors == 0);
  return 0;
}

