/*
 * status_output.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "so_out.h"
#include <app_config/options.hh>
#include <app/opt_map.hh>


#include "app/cli/cli_app.h" // FIXME?
#include "app/cli/cli_config.h"
#include "txtio/inout.h"
#include "uout/status_json.hh"
#include "debug/dbg.h"
#include "utils_misc/int_macros.h"
#include "utils_misc/int_types.h"
#include "utils_misc/ftoa.h"
#include "utils_misc/itoa.h"
#include <string.h>
#include <stdio.h>

#define D(x)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// get the SO_MSG_T for a config key string (or SO_NONE)
SO_MSG_T so_parse_config_key(const char *k) {
  otok kt = optMap_findToken(k);
  if (kt == otok::NONE) {
    return SO_NONE;
  }

  SO_MSG_T result = static_cast<SO_MSG_T>(SO_CFG_begin + 1 + static_cast<otokBaseT>(kt));

  return result;
}

// get config key string for a so_msgt_t (or NULL)
const char * gk(SO_MSG_T so_key) {
  if (SO_CFG_begin < so_key && so_key < SO_CFG_end)
    return otok_strings[so_key - (SO_CFG_begin + 1)];
  return nullptr;
}



