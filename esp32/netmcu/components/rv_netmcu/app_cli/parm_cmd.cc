#include "app_config/proj_app_cfg.h"
#include "app_config/options.hh"
#include <string.h>


#include "txtio/inout.h"
#include "app_mqtt/mqtt.h"
#include "app_config/proj_app_cfg.h"
#include "cli_imp.h"
#include "cli/cli.h"
#include "app_cli/cli_app.h"
#include "app_uout/status_output.h"
#include "uout/uout_builder_json.hh"
#include <uout/cli_out.h>
#include "app_misc/opt_map.hh"
#include "debug/dbg.h"
#include <debug/log.h>
#include "utils_misc/int_macros.h"
#include <stdio.h>

#include <stm32/stm32.h>


#ifdef CONFIG_RV_NETMCU_DEBUG
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "rv.cli.parm_cmd"



volatile bool cli_cmd_wait_for_response;

#define ONE_MINUTE (60)
#define ONE_HOUR (ONE_MINUTE * 60)
#define ONE_DAY (ONE_HOUR * 24)

const char cli_help_parmCmd[]  =
    "durN=[0-60]      activate zone N for up to 60 minutes (0=off). Example: cmd dur3=45;\n"
    "dur=?            request durations of all activated zones\n"
    "rem=?            request remaining times of all activated zones\n"
;

#define KEY_DURATION_PREFIX "dur"
#define KEY_DURATION_PREFIX_LEN ((sizeof KEY_DURATION_PREFIX) - 1)

#define RV_VERSION "rv-version"
#define CMD_ASK_VERSION "\"version\":\"?\","

#define BUF_SIZE 128
#define ZONE_COUNT 14
#define MAX_DURATION 60

int
process_parmCmd(clpar p[], int len, class UoutWriter &td) {
  int arg_idx;
  char buf[BUF_SIZE] = "{\"cmd\":{";
  int buf_idx = sizeof(buf);

  bool hasCmdLine = false;

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;
    D(db_logi(logtag, "key=%s, val=%s", key, val));

    if (!key)
      return -1;
#if 1
    otok kt = optMap_findToken(key);
    if (kt != otok::NONE) {
      switch (kt) {

      default:
        break;
      }
    }
#endif
    if (*val == '?') {
      if (strcmp(key, RV_VERSION) == 0) {
        strcat(buf, CMD_ASK_VERSION);
      } else {
        snprintf(buf + strlen(buf), sizeof buf - strlen(buf), "\"%s\":\"?\",", key);
      }
      hasCmdLine = true;

    } else if (strncmp(key, KEY_DURATION_PREFIX, KEY_DURATION_PREFIX_LEN) == 0) {
      int zone=-1, timer_number=0;
      sscanf((key + KEY_DURATION_PREFIX_LEN), "%d.%d", &zone, &timer_number);
      if (strchr(val, ',')) {
        float on = 0, off = 0;
        int repeats = 0;
        float period = 0;
        int dInterval = 0;
        float dhBegin = 0, dhEnd = 0;
        int ignoreRainSensor = 0;

        sscanf(val, "%f,%d,%f,%d,%f,%d,%f,%f", &on, &ignoreRainSensor, &off, &repeats, &period, &dInterval, &dhBegin, &dhEnd);

        buf_idx += sprintf(buf + strlen(buf), "\"dur%d.%d\":\"%d,%d,%d,%d,%d,%d,%d,%d\",", zone, timer_number, (int) (on * ONE_MINUTE), ignoreRainSensor, (int) (off * ONE_MINUTE), repeats,
            (int) (period * ONE_HOUR), dInterval, (int)(dhBegin * ONE_HOUR), (int)(dhEnd * ONE_HOUR));

        hasCmdLine = true;
      } else {
        float duration = 0;
        sscanf(val, "%f", &duration);
        if (0 <= zone && zone < ZONE_COUNT && 0 <= duration && duration <= MAX_DURATION) {
          buf_idx += sprintf(buf + strlen(buf), "\"dur%d.%d\":%d,", zone, timer_number, (int)(duration * 60));
          hasCmdLine = true;
        } else {
          db_printf("error: out of range");
        }
      }
    } else {
      cli_warning_optionUnknown(td,key);
    }
  }

  if (hasCmdLine) {
    buf[strlen(buf) - 1] = '\0';
    strcat(buf, "}}\n");
    D(db_logi(logtag, "to_stm32: <%s>", buf));
    stm32_write(buf, strlen(buf));
  }

  return 0;
}





