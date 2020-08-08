/*
 * cli_kvs.c
 *
 *  Created on: 12.06.2019
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#include "userio_app/status_output.h"
#include "config/config.h"
#include "app/rtc.h"
#include "cli_imp.h"
#include "net/mqtt/app/mqtt.h"
#include "app/kvstore.h"
#include "debug/debug.h"

#define ENABLE_RESTART 1 // allow software reset

const char cli_help_parmKvs[]  =
    "'kvs' sets or gets key/value pairs\n\n"

;

#define ZONE_NAME_KEY "zn"
#define ZONE_NAME_KEY_LEN ((sizeof ZONE_NAME_KEY) - 1)

int
process_parmKvs(clpar p[], int len) {
  int arg_idx;
  int errors = 0;

  dbg_vpf(db_printf("process_parmKvs()\n"));

  so_output_message(SO_KVS_begin, NULL);

  bool pw_ok = strlen(C.app_configPassword) == 0;
  int needSecondPass = 0;

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL || val == NULL) {  // don't allow any default values
      ++errors;
      continue;
    }

    if (strncmp(key, ZONE_NAME_KEY,ZONE_NAME_KEY_LEN) == 0) {
      //int zone_number = atoi(key + ZONE_NAME_KEY_LEN);
      if (*val == '?') {
       ++needSecondPass;// handle this on second pass below
      } else {
        if (!kvs_store_string(key, val)) {
          ++errors;
        }
      }
    } else {
      ++errors;
      cli_warning_optionUnknown(key);
    }
  }

  for (arg_idx = 1; arg_idx < len && needSecondPass > 0; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (*val != '?')  // handle '?' here only
      continue;
    if (key == NULL || val == NULL)  // don't allow any default values
      continue;

    if (strncmp(key, ZONE_NAME_KEY,ZONE_NAME_KEY_LEN) == 0) {
      //int zone_number = atoi(key + ZONE_NAME_KEY_LEN);
      if (*val == '?') {
        if (strlen(key) == ZONE_NAME_KEY_LEN) {
          so_output_message(SO_KVS_ZN_ALL, key);
        } else {
          so_output_message(SO_KVS_ZN_SINGLE, key);
        }
        --needSecondPass;
      }
    }
  }

  so_output_message(SO_KVS_end, NULL);
  cli_replyResult(errors==0);
  return 0;
}

