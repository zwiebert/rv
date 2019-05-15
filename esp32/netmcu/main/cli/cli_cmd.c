#include "user_config.h"

#include <string.h>


#include "userio/inout.h"
#include "user_config.h"
#include "cli_imp.h"
#include "userio/status_output.h"
#include <stdio.h>

#include "uart.h"

#define FSB_PLAIN_REPEATS 2  // send plain commands 1+N times (if 0, send only once without repeating)

const char help_parmCmd[]  =
    "zone=[0-13]      zone number\n"
    "duration=[0-60]  how long to irrigate\n"
;

int ICACHE_FLASH_ATTR
process_parmCmd(clpar p[], int len) {
  int arg_idx;

  struct cmd {
	  uint8_t zone, duration;


  } cmd;

  bool hasZone = false, hasDuration = false;

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL) {
      return -1;
    } else if (strcmp(key, "zone") == 0) {
    	cmd.zone = atoi(val);
    	hasZone = true;
    } else if (strcmp(key, "duration") == 0) {
    	cmd.duration = atoi(val);
    	hasDuration = true;
    } else {
      warning_unknown_option(key);
    }
  }

  if (hasZone && hasDuration) {
	  char buf[128];
	  sprintf(buf, "%d:%d;\n", cmd.zone, cmd.duration);
	  stm32_write(buf, strlen(buf));
  }
  return 0;
}
