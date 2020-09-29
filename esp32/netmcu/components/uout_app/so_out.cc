/*
 * status_output.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */
#include "app/config/proj_app_cfg.h"
#include "so_out.h"

#include "app/cli/cli_app.h" // FIXME?
#include "app/cli/cli_config.h"
#include "txtio/inout.h"
#include "uout/status_json.hh"
#include "debug/dbg.h"
#include "misc/int_macros.h"
#include "misc/int_types.h"
#include "misc/ftoa.h"
#include "misc/itoa.h"
#include <string.h>
#include <stdio.h>

#define D(x)

u8 so_target;

// get the SO_MSG_T for a config key string (or SO_NONE)
SO_MSG_T
so_parse_config_key(const char *k) {
  const int cfg_len = (SO_CFG_end - SO_CFG_begin - 1);
  const int keys_len = sizeof cfg_keys / sizeof cfg_keys[0];
  precond((cfg_len == keys_len));

  SO_MSG_T result = SO_NONE;

  for (int i = 0; i < keys_len; ++i) {
    if (0 == strcmp(k, cfg_keys[i])) {
      result = static_cast<SO_MSG_T>(SO_CFG_begin + 1 + i);
      break;
    }
  }
  return result;
}


// get config key string for a so_msgt_t (or NULL)
static const char *
gk(SO_MSG_T so_key) {
  if (SO_CFG_begin < so_key && so_key < SO_CFG_end)
    return cfg_keys[so_key - (SO_CFG_begin + 1)];
  return NULL;
}



