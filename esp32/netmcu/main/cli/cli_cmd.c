#include "user_config.h"

#include <string.h>


#include "userio/inout.h"
#include "user_config.h"
#include "cli_imp.h"
#include "userio/status_output.h"
#include "debug/debug.h"
#include <stdio.h>

#include "uart.h"

#define FSB_PLAIN_REPEATS 2  // send plain commands 1+N times (if 0, send only once without repeating)

const char help_parmCmd[]  =
    "zone=[0-13]      zone number\n"
    "duration=[0-60]  how long to irrigate\n"
;

#define KEY_DURATION_PREFIX "dur"
#define KEY_DURATION_PREFIX_LEN ((sizeof KEY_DURATION_PREFIX) - 1)
#define CMD_ASK_DURATIONS "cmd dur=?;"
#define CMD_ASK_DURATIONS_LEN (sizeof CMD_ASK_DURATIONS - 1)

#define KEY_REMAINING_PREFIX "rem"
#define KEY_REMAINING_TIME_PREFIX_LEN ((sizeof KEY_REMAINING_TIME_PREFIX) - 1)
#define CMD_ASK_REMAINING_TIMES "cmd rem=?;"
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

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;
    db_printf("key=%s, val=%s\n", key, val);

    if (key == NULL) {
      return -1;

    } else if (strcmp(key, "dur") == 0 && *val == '?') {
		stm32_write(CMD_ASK_DURATIONS, CMD_ASK_DURATIONS_LEN);

    } else if (strcmp(key, "rem") == 0 && *val == '?') {
		stm32_write(CMD_ASK_REMAINING_TIMES, CMD_ASK_REMAINING_TIMES_LEN);

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
	}

  return 0;
}
