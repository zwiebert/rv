/*
 * cli.c
 *
 * Created: 24.08.2017 21:45:48
 *  Author: bertw
 */
#include <cli/cli.h>
#include <debug/dbg.h>
#include "user_config.h"
#include "cli_imp.h"
#include <stdio.h>
#include <string.h>
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


#define ENABLE_RESTART 1 // allow software reset

typedef void (*void_fun_ptr)(void);
const int OUT_MAX_LEN = 80;



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


static struct parm_handler handlers[] = {
        { "config", process_parmConfig, 0 },
        { "cmd", process_parmCmd, 0 },
        { "timer", process_parmTimer, 0 },
        { "mcu", process_parmMcu, 0 },
        { "pbuf", process_parmProtoBuf, 0 },
        { "kvs", process_parmKvs, 0 },
  };


void
cli_process_cmdline(char *line) {
  int n = parse_commandline(line);
  if (n < 0) {
    reply_failure();
  } else {
    process_parm(par, n);
  }
}



static const struct parm_handlers our_parm_handlers = { .handlers = handlers, .count = sizeof(handlers) / sizeof(handlers[0]), };

const parm_handler* cli_parmHandler_find(const char *key) {
  auto handler = std::find_if(std::begin(handlers), std::end(handlers), [&key](auto el) {
    return strcmp(key, el.parm) == 0;
  });
  if (std::end(handlers) == handler) {
    D(db_logi(logtag, "%s(%s) handler not found", __func__, key));
    return nullptr;
  }
  D(db_logi(logtag, "%s(%s) handler found", __func__, key));
  return handler;
}


void cliApp_setup() {
 // cli_hook_process_json_obj = process_objJson;
 // cli_hook_checkPassword = cliApp_checkPassword;

  cli_hook_process_json = process_json;
  cli_parmHandler_find_cb = cli_parmHandler_find;
  cli_parm_handlers = &our_parm_handlers;
}

