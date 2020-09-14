/*
 * cli_kvs.c
 *
 *  Created on: 12.06.2019
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#include "uout_app/status_output.h"
#include "config/config.h"
#include "app/rtc.h"
#include "cli_imp.h"
#include "cli_app/cli_app.h"
#include "net/mqtt/app/mqtt.h"
#include "app/kvstore.h"
#include "debug/dbg.h"
#include <ctype.h>
#include <string.h>

#define ENABLE_RESTART 1 // allow software reset

const char cli_help_parmKvs[] = "'kvs' sets or gets key/value pairs\n\n";

static const char *zoneKeysN[] = { "zn", "lph", 0 };

static bool match_zoneKeyN(const char *key) {
  for (int i = 0; zoneKeysN[i]; ++i) {
    if (key == strstr(key, zoneKeysN[i]))
      return true;
  }
  return false;
}

static bool match_kvsKey(const char *key) {
  return true; // XXX: allow any keys for now
}



int process_parmKvs(clpar p[], int len) {
  int arg_idx;
  int errors = 0;

  dbg_vpf(db_printf("process_parmKvs()\n"));

  so_output_message(SO_KVS_begin, NULL);

  int needSecondPass = 0;

  // first pass: input parameters
  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;
    if (key == NULL || val == NULL) {  // don't allow any default values
      ++errors;
      continue;
    }

    if (*val == '?') {
      ++needSecondPass;
      continue; // handle this key/val pair in second pass
    }

    if (match_zoneKeyN(key)) {
      if (!kvs_store_string(key, val)) {
        ++errors;
      }
      continue;
    }

    if (match_kvsKey(key)) {
      if (!kvs_store_string(key, val)) {
        ++errors;
        continue;
      }
    }

  }

  // second pass: output parameters
  for (arg_idx = 1; arg_idx < len && needSecondPass > 0; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;
    if (key == NULL || val == NULL)  // don't allow any default values
      continue;

    if (*val != '?')
      continue; // was handled in first pass

    --needSecondPass;

    if (match_zoneKeyN(key)) {
      if (!isdigit(key[strlen(key) - 1])) {

        so_output_message(SO_KVS_ZN_ALL, key);
      } else {
        so_output_message(SO_KVS_ZN_SINGLE, key);
      }
    }

    if (match_kvsKey(key)) {
      so_output_message(SO_KVS_ZN_SINGLE, key);
    }

  }

  so_output_message(SO_KVS_end, NULL);
  cli_replyResult(errors == 0);
  return 0;
}

