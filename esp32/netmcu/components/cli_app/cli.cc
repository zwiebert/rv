/*
 * cli.c
 *
 * Created: 24.08.2017 21:45:48
 *  Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#include <string.h>
#include "cli_imp.h"
#include "cli_app/cli_app.h"
#include "cli/mutex.hh"
#include "cli/cli.h"
#include "userio/status_json.h"
#include "stm32/stm32.h"
#include "userio_app/status_output.h"
#include "config/config.h"

bool cli_isJson;

int process_parmHelp(clpar p[], int len);

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
    { "pbuf", process_parmProtoBuf, cli_help_None},
  };

const struct parm_handlers parm_handlers = {
    .handlers = handlers,
    .count = sizeof(handlers) / sizeof(handlers[0]),
};

bool cli_checkStm32CommandLine(char *line) {
  char *terminator = &line[strlen(line) - 1];
  if (*terminator != ';') {
    return false;
  }
  *terminator = '\0';
  if (auto lock = ThreadLock(cli_mutex)) {
    cli_process_cmdline(line, SO_TGT_ANY); //XXX: which target?
  }
  return true;
}

/////////////// setup //////////////////

static bool cliApp_redirect_to_rv(char *json) {
#define TO_RV "{\"to\":\"rv\""
#define TO_RV_LEN ( sizeof TO_RV - 1)
  if (strncmp(json, TO_RV, TO_RV_LEN) == 0) {
    if (stm32_mutexTake()) {
      stm32_write(json, strlen(json));
      stm32_write(";\n", 2);
      stm32_mutexGive();
    }
    return true;
  }
  return false;
}

static bool cliApp_checkPassword(clpar p[], int len, so_target_bits tgt) {
  if (len < 2)
    return true;

  if (strcmp(p[0].key, "config") != 0)
    return true;

  if (!C.app_configPassword[0])
    return true;
  if (strcmp(p[1].key, "pw") == 0) {
    if (strcmp(p[1].val, C.app_configPassword) == 0) {
      so_output_message(SO_CFGPASSWD_OK, NULL);
      return true;
    } else {
      so_output_message(SO_CFGPASSWD_WRONG, NULL);
    }
  } else {
    so_output_message(SO_CFGPASSWD_MISSING, NULL);
  }

  return false;
}



void cliApp_setup() {
  cli_hook_process_json = cliApp_redirect_to_rv;
  cli_hook_checkPassword = cliApp_checkPassword;
}

