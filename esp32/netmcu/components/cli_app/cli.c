/*
 * cli.c
 *
 * Created: 24.08.2017 21:45:48
 *  Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#include <string.h>
#include "cli_imp.h"
#include "cli_app.h"
#include "userio/status_json.h"

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
  };

const struct parm_handlers parm_handlers = {
    .handlers = handlers,
    .count = sizeof(handlers) / sizeof(handlers[0]),
};







bool cli_checkStm32CommandLine(char *line) {
  char *terminator = &line[strlen(line)-1];
  if (*terminator != ';') {
    return false;
  }
  *terminator = '\0';
  cli_process_cmdline(line, SO_TGT_ANY); //XXX: which target?
  return true;
}


