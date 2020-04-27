/*
 * config.c
 *
 *  Created on: 10.09.2018
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"


#include "config/config.h"
#include "misc/int_macros.h"
#include "key_value_store/kvs_wrapper.h"
#include "misc/int_types.h"

#define D(x) 

#define CFG_NAMESPACE "Tronferno"
#define CFG_KEY "global.C"

bool config_item_modified(enum configItem item) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    switch (item) {
    case CB_VERBOSE:
      config_setup_txtio();
      break;
    default:
      break;
    }
    kvs_close(h);
  }
  return true;
}

