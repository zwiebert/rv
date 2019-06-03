#if 0
/*
 * cli.c
 *
 *  Created on: 15.05.2019
 *      Author: bertw
 */

#include "cli.h"
#include "cli_imp.h"
#include "debug/debug.h"



int ICACHE_FLASH_ATTR
process_parmTimer(clpar p[], int len) {
	for (int i = 0; i < len; ++i) {
		db_printf("key=%s, val=%s\n", p[i].key, p[i].val);
	}

return 0;
}

struct {
  const char *parm;
  int (*process_parmX)(clpar p[], int len);
  const char *help;
} parm_handlers[] = {

                      { "timer", process_parmTimer, 0 },
  };




int ICACHE_FLASH_ATTR
process_parm(clpar p[], int len) {
  int i;

db_puts("process parm");
  for (i = 0; i < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++i) {
    if (strcmp(p[0].key, parm_handlers[i].parm) == 0)
      return parm_handlers[i].process_parmX(p, len);
  }
  return 0;
}


int reply_failure(void) { return 0;}
uint16_t msgid;


#else

/*
 * cli.c
 *
 * Created: 24.08.2017 21:45:48
 *  Author: bertw
 */
#include "user_config.h"
#include "cli_imp.h"
#include <stdio.h>


#define ENABLE_RESTART 1 // allow software reset

typedef void (*void_fun_ptr)(void);
const int OUT_MAX_LEN = 80;






int ICACHE_FLASH_ATTR
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

void ICACHE_FLASH_ATTR
reply_success() {
	puts("ok:");
  //reply_message(0, "ok");
}

int ICACHE_FLASH_ATTR
reply_failure() {
 puts("error:"); // reply_message(0, "error");
  return -1;
}





bool ICACHE_FLASH_ATTR
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
  };

int ICACHE_FLASH_ATTR
process_parm(clpar p[], int len) {
  int i;

  for (i = 0; i < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++i) {
    if (strcmp(p[0].key, parm_handlers[i].parm) == 0)
      return parm_handlers[i].process_parmX(p, len);
  }
  return 0;
}

void ICACHE_FLASH_ATTR
cli_process_cmdline(char *line) {
  int n = parse_commandline(line);
  if (n < 0) {
    reply_failure();
  } else {
    process_parm(par, n);
  }
}


void ICACHE_FLASH_ATTR cli_loop(void) {
  char *cmdline;
  if ((cmdline = get_commandline())) {
#ifdef USE_JSON
    if (cmdline[0] == '{') {
     cli_process_json(cmdline);
    } else
#endif
    {
      cli_process_cmdline(cmdline);
    }
  }
}

#if TEST_MODULE_CLI
bool ICACHE_FLASH_ATTR
testModule_cli()
{
  char cl[] = "timer g=5 astro=0;";  //"timer g=2 m=2 weekly=08222000++++10552134+";
  int n = parse_commandline(cl);
  if (n > 0)
  process_parm(par, n);

  return n != -1;
}
#endif
#endif
