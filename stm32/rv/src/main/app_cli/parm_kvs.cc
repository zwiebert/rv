
/**
 * \file cli/parm_kvs.cc
 * \brief handle cli command
 */
extern "C" {
#include <time/real_time_clock.h>
#include "user_config.h"
#include <peri/uart.h>
}
#include "cli_imp.h"
#include "rv/rv_zones.hh"
#include "loop/loop.hh"
#include "rv/rv_timer.hh"
#include <stdio.h>
#include <ctype.h>
#include <debug/log.h>
#ifdef CONFIG_CLI_DEBUG
#define DEBUG
#define D(x) x
#define L(x) x
#else
#define D(x)
#define L(x) x
#endif
#define logtag "cli"

enum { ZN, LPH };
static const char *zoneKeysN[] = {
    "zn", "lph", 0
};

static bool kvs_store_string(const char *key, const char *val) {
  if (0 < std::printf("{\"to\":\"cli\",\"kvs\":{\"%s\":\"%s\"}};\n", key, val)) {
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

int process_parmKvs(clpar p[], int len, class UoutWriter &td) {
  D(db_logi(logtag, "%s(len=%d)", __func__, len));
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



