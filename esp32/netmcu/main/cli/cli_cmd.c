#include "user_config.h"

#include <string.h>


#include "userio/inout.h"
#include "userio/mqtt.h"
#include "user_config.h"
#include "cli_imp.h"
#include "userio/status_output.h"
#include "userio/status_json.h"
#include "debug/debug.h"
#include "misc/int_macros.h"
#include <stdio.h>

#include <stm32/stm32.h>

#ifndef DISTRIBUTION
#define D(x) x
#else
#define D(x)
#endif

void cliCmd_waitForResponse();


#define ONE_MINUTE (60)
#define ONE_HOUR (ONE_MINUTE * 60)
#define ONE_DAY (ONE_HOUR * 24)

const char help_parmCmd[]  =
    "durN=[0-60]      activate zone N for up to 60 minutes (0=off). Example: cmd dur3=45;\n"
    "dur=?            request durations of all activated zones\n"
    "rem=?            request remaining times of all activated zones\n"
;

#define KEY_DURATION_PREFIX "dur"
#define KEY_DURATION_PREFIX_LEN ((sizeof KEY_DURATION_PREFIX) - 1)
#define CMD_ASK_DURATIONS " dur=?"
#define CMD_ASK_DURATIONS_LEN (sizeof CMD_ASK_DURATIONS - 1)

#define KEY_REMAINING_PREFIX "rem"
#define KEY_REMAINING_TIME_PREFIX_LEN ((sizeof KEY_REMAINING_TIME_PREFIX) - 1)
#define CMD_ASK_REMAINING_TIMES " rem=?"
#define CMD_ASK_REMAINING_TIMES_LEN (sizeof CMD_ASK_REMAINING_TIMES - 1)

#define KEY_STATUS_PREFIX "status"
#define KEY_STATUS_PREFIX_LEN ((sizeof KEY_STATUS_PREFIX) - 1)
#define CMD_ASK_STATUS " status=?"
#define CMD_ASK_STATUS_LEN (sizeof CMD_ASK_STATUS - 1)

#define RV_VERSION "rv-version"
#define CMD_ASK_VERSION " version=?"

#define BUF_SIZE 128
#define ZONE_COUNT 14
#define MAX_DURATION 60

int ICACHE_FLASH_ATTR
process_parmCmd(clpar p[], int len) {
  int arg_idx;
  char buf[BUF_SIZE] = "cmd";
  int buf_idx = sizeof(buf);

  bool hasCmdLine = false;
  bool reqResponse = false, reqDurations = false, reqRemainingTimes = false, reqVersion = false;

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;
    db_printf("key=%s, val=%s\n", key, val);

    if (key == NULL) {
      return -1;

    } else if (strcmp(key, "dur") == 0 && *val == '?') {
      strcat(buf, CMD_ASK_DURATIONS);
      reqResponse = true;
      hasCmdLine = true;

    } else if (strcmp(key, RV_VERSION) == 0 && *val == '?') {
      strcat(buf, CMD_ASK_VERSION);
      reqResponse = true;
      reqVersion = true;
      hasCmdLine = true;

    } else if (strcmp(key, "rem") == 0 && *val == '?') {
      strcat(buf, CMD_ASK_REMAINING_TIMES);
      reqResponse = true;
      hasCmdLine = true;
    } else if (strcmp(key, "status") == 0 && *val == '?') {
      strcat(buf, CMD_ASK_STATUS);
      reqResponse = true;
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

        buf_idx += sprintf(buf + strlen(buf), " dur%d.%d=%d,%d,%d,%d,%d,%d,%d,%d", zone, timer_number, (int) (on * ONE_MINUTE), ignoreRainSensor, (int) (off * ONE_MINUTE), repeats,
            (int) (period * (60 * ONE_HOUR)), dInterval, (int)(dhBegin * ONE_HOUR), (int)(dhEnd * ONE_HOUR));

        hasCmdLine = true;
      } else {
        float duration = 0;
        sscanf(val, "%f", &duration);
        if (0 <= zone && zone < ZONE_COUNT && 0 <= duration && duration <= MAX_DURATION) {
          buf_idx += sprintf(buf + strlen(buf), " dur%d.%d=%d", zone, timer_number, (int)(duration * 60));
          hasCmdLine = true;
        } else {
          db_printf("error: out of range");
        }
      }
    } else {
      warning_unknown_option(key);
    }
  }

	if (hasCmdLine) {
		strcat(buf, ";\n");
		db_printf("cmd2stm32: <%s>\n", buf);
		stm32_write(buf, strlen(buf));
                if (reqResponse) {
                  cliCmd_waitForResponse();
                }
	}

  return 0;
}

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// wait for response
// data will be written to ext_buf from status_json.c which is allocated by the http_handler
void cliCmd_waitForResponse() {
#define WFR_TOTAL_MS 2000
#define WFR_INTERVAL_MS 50
    precond(ext_buf);

    *ext_buf = '\0';
    int n = 0;
    D(db_printf("wait for response\n"));
    for (int i = 0; i < (WFR_TOTAL_MS / WFR_INTERVAL_MS); ++i) {
        vTaskDelay(WFR_INTERVAL_MS / portTICK_PERIOD_MS);
        D(db_printf(":"));
        n += stm32_read(ext_buf + n, ext_buf_size - 1 - n);
        if (n > 0) {
            ext_buf[n] = '\0';
            char *sc = strchr(ext_buf, ';');
            if (sc) {
                *sc = '\0';
                D(db_printf("cli_cmd(from stm32): <%s>\n", ext_buf));
                break;
            }
        }

    }
    D(db_printf("stop wait for response\n"));
}

#define STM32_STATUS "status "
#define STM32_STATUS_LEN (sizeof (STM32_STATUS) - 1)




