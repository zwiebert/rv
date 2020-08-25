/*
 * cli_config.c
 *
 *  Created on: 21.05.2019
 *      Author: bertw
 */
extern "C" {
#include <time/real_time_clock.h>
#include "user_config.h"
#include "cli_imp.h"
#include "peri/uart.h"
}
#include "rv/rv_zones.hh"
#include "loop/loop.hh"
#include "rv/rv_timer.hh"
#include <stdio.h>
#include <ctype.h>

enum { ZN, LPH };
static const char *zoneKeysN[] = {
    "zn", "lph", 0
};

static bool kvs_store_string(const char *key, const char *val) {
  char buf[128];
  if (0 < snprintf(buf, sizeof buf, "{\"to\":\"cli\",\"kvs\":{\"%s\":\"%s\"}};\n", key, val)) {
    esp32_puts(buf);
    return true;
  }
  return false;
}

static int match_zoneKeyN(const char *key) {
  for (int i = 0; zoneKeysN[i]; ++i) {
    if (key == strstr(key, zoneKeysN[i]))
      return i;
  }
  return -1;
}

int process_parmKvs(clpar p[], int len) {
  int arg_idx;
  int errors = 0;
  int needSecondPass = 0;
  int zkIdx = -1;

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL || val == NULL) {  // don't allow any default values in config
      return reply_failure();

    } else if ((zkIdx = match_zoneKeyN(key)) >= 0) {
      if (*val == '?') {
        ++needSecondPass;  // handle this on second pass below
      } else {
        if (!kvs_store_string(key, val)) {
          ++errors;
        }
        if (isdigit(key[strlen(key) - 1]))
          switch (zkIdx) {
          case ZN: {
            int idx = atoi(key + 2);
            rvz[idx].name() = std::string(val);
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



