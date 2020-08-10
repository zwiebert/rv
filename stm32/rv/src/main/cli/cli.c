/*
 * cli.c
 *
 * Created: 24.08.2017 21:45:48
 *  Author: bertw
 */
#include "user_config.h"
#include "cli_imp.h"
#include <stdio.h>
#include "debug/debug.h"

#define ENABLE_RESTART 1 // allow software reset

typedef void (*void_fun_ptr)(void);
const int OUT_MAX_LEN = 80;


int
asc2bool(const char *s) {
  if (!s)
    return 1; // default value for key without value

  switch (*s) {
  case '0':
    return 0;
  case '1':
    return 1;
  default:
    return -1;
  }

}

void
reply_success() {
	puts("ok:");
  //reply_message(0, "ok");
}

int
reply_failure() {
 puts("error:"); // reply_message(0, "error");
  return -1;
}





bool
reply(bool success) {
  if (success)
    reply_success();
  else
    reply_failure();
  return success;
}

struct {
  const char *parm;
  int (*process_parmX)(clpar p[], int len);
  const char *help;
} parm_handlers[] = {
        { "config", process_parmConfig, 0 },
        { "cmd", process_parmCmd, 0 },
        { "mcu", process_parmMcu, 0 },
        { "pbuf", process_parmProtoBuf, 0 },
  };

int
process_parm(clpar p[], int len) {
  int i;

  for (i = 0; i < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++i) {
    if (strcmp(p[0].key, parm_handlers[i].parm) == 0)
      return parm_handlers[i].process_parmX(p, len);
  }
  return 0;
}

void
cli_process_cmdline(char *line) {
  int n = parse_commandline(line);
  if (n < 0) {
    reply_failure();
  } else {
    process_parm(par, n);
  }
}


void cli_loop(void) {
  char *cmdline;
  if ((cmdline = get_commandline())) {
    db_printf("cmdline: %s\n", cmdline);
#ifdef USE_WDG
	  extern bool watchDog_checkCommandLine(const char *cmdLine);
	  if (watchDog_checkCommandLine(cmdline)) {

	  } else
#endif
#ifdef USE_JSON
    if (cmdline[0] == '{') {
     cli_process_json(cmdline, process_parm);
    } else
#endif
    {
      cli_process_cmdline(cmdline);
    }
  }
}


