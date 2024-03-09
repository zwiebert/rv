/*
 * cli.c
 *
 * Created: 24.08.2017 21:45:48
 *  Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#include <string.h>
#include "cli_imp.h"
#include "app_cli/cli_app.h"
#include "cli/mutex.hh"
#include "cli/cli.h"
#include "uout/status_json.hh"
#include "stm32/stm32.h"
#include "app_uout/status_output.h"
#include "app_settings/config.h"
#include <app_uout/so_config.h>
#include <app_uout/so_msg.h>
#include <algorithm>
#include <iterator>

bool cli_isJson;

int process_parmHelp(clpar p[], int len, const class UoutWriter &td);

const char cli_help_parmHelp[]  =
"type 'help command;'  or 'help all;'\ncommands are: ";

const char cli_help_None[]  = "none";

static struct parm_handler handlers[] = {
    { "config", process_parmConfig, cli_help_parmConfig },
    { "help", process_parmHelp, cli_help_parmHelp },
    { "cmd", process_parmCmd, cli_help_parmCmd },
    { "mcu", process_parmMcu, cli_help_parmMcu },
    { "kvs", process_parmKvs, cli_help_parmKvs },
    { "status", process_parmStatus, cli_help_None},
#ifdef CONFIG_APP_USE_PROTOBUF
    { "pbuf", process_parmProtoBuf, cli_help_None},
#endif
  };

static const struct parm_handlers our_parm_handlers = { .handlers = handlers, .count = sizeof(handlers) / sizeof(handlers[0]), };

const parm_handler* cli_parmHandler_find(const char *key) {
  auto handler = std::find_if(std::begin(handlers), std::end(handlers), [&key](auto el) {
    return strcmp(key, el.parm) == 0;
  });
  if (std::end(handlers) == handler) {
    if (strcmp("timer", key) == 0)  // alias
      return cli_parmHandler_find("auto");
    if (strcmp("send", key) == 0)  // alias
      return cli_parmHandler_find("cmd");

    return nullptr;
  }

  return handler;
}

bool cli_checkStm32CommandLine(char *line) {
  char *terminator = &line[strlen(line) - 1];
  if (*terminator != ';') {
    return false;
  }
  *terminator = '\0';
  { LockGuard lock(cli_mutex); 
    cli_process_cmdline(line, SO_TGT_ANY); //XXX: which target?
  }
  return true;
}

/////////////// setup //////////////////

static bool cliApp_redirect_to_rv(char *json) {
#define TO_RV "{\"to\":\"rv\""
#define TO_RV_LEN ( sizeof TO_RV - 1)
  if (strncmp(json, TO_RV, TO_RV_LEN) == 0) {
    LockGuard lock(stm32_mutex);
    stm32_write(json, strlen(json));
    stm32_write(";\r\n", 3);
    return true;
  }
  return false;
}

static bool cliApp_checkPassword(clpar p[], int len, const class UoutWriter &td) {
  if (len < 2)
    return true;

  if (strcmp(p[0].key, "config") != 0)
    return true;

  if (!C.app_configPassword[0])
    return true;
  if (strcmp(p[1].key, "pw") == 0) {
    if (strcmp(p[1].val, C.app_configPassword) == 0) {
      soMsg_cfgpasswd_ok(td);
      return true;
    } else {
      soMsg_cfgpasswd_wrong(td);
    }
  } else {
    soMsg_cfgpasswd_missing(td);
  }

  return false;
}





void cliApp_setup() {
  cli_hook_process_json = cliApp_redirect_to_rv;
  cli_hook_checkPassword = cliApp_checkPassword;

  cli_parmHandler_find_cb = cli_parmHandler_find;
  cli_parm_handlers = &our_parm_handlers;
}

