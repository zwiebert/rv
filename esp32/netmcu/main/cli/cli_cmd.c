#include "user_config.h"

#include <string.h>


#include "userio/inout.h"
#include "user_config.h"
#include "cli_imp.h"
#include "userio/status_output.h"
#include "userio/status_json.h"
#include "debug/debug.h"
#include <stdio.h>

#include "uart.h"

#define D(x) x

void cliCmd_waitForResponse();


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

#define BUF_SIZE 128
#define ZONE_COUNT 14
#define MAX_DURATION 60

int ICACHE_FLASH_ATTR
process_parmCmd(clpar p[], int len) {
  int arg_idx;
  char buf[BUF_SIZE] = "cmd";
  int buf_idx = sizeof(buf);

  bool hasCmdLine = false;
  bool reqResponse = false, reqDurations = false, reqRemainingTimes = false;

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;
    db_printf("key=%s, val=%s\n", key, val);

    if (key == NULL) {
      return -1;

    } else if (strcmp(key, "dur") == 0 && *val == '?') {
      strcat(buf, CMD_ASK_DURATIONS);
      reqResponse = true;
      hasCmdLine = true;

    } else if (strcmp(key, "rem") == 0 && *val == '?') {
      strcat(buf, CMD_ASK_REMAINING_TIMES);
      reqResponse = true;
      hasCmdLine = true;
    } else if (strncmp(key, KEY_DURATION_PREFIX, KEY_DURATION_PREFIX_LEN) == 0) {
      int zone = atoi(key + KEY_DURATION_PREFIX_LEN);
      int duration = atoi(val);
      if (0 <= zone && zone < ZONE_COUNT  && 0 <= duration && duration <= MAX_DURATION) {
    	  buf_idx += sprintf(buf + strlen(buf), " dur%d=%d", zone, duration);
    	  hasCmdLine = true;
      } else {
    	  db_printf("error: out of range");
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
// data wil be writte to ext_buf from status_json.c which is allocated by the http_handler
void cliCmd_waitForResponse() {
#define WFR_TOTAL_MS 2000
#define WFR_INTERVAL_MS 50
  *ext_buf = '\0';
  int n = 0;
  while(1) {
	extern void *pxCurrentTCB;
	db_printf("(%p)\n", pxCurrentTCB);
  D(db_printf("wait for response\n"));
  for (int i=0; i < (WFR_TOTAL_MS / WFR_INTERVAL_MS); ++i) {
	  db_printf("-");
    vTaskDelay(WFR_INTERVAL_MS / portTICK_PERIOD_MS);
	  db_printf(":");
    n += stm32_read(ext_buf + n, ext_buf_size-1 - n);
    if (n > 0) {
      ext_buf[n] = '\0';
      if (strchr(ext_buf, ';')) {
        D(db_printf("from-cli: <%s>\n", ext_buf));
        break;
      }
    }
  }
  }
  D(db_printf("stop wait for response\n"));
}
