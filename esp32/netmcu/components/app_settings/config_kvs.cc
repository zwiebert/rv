/*
 * config.c
 *
 *  Created on: 10.09.2018
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"


#include "app_settings/config.h"
#include "utils_misc/int_macros.h"
#include "key_value_store/kvs_wrapper.h"
#include "utils_misc/int_types.h"

#define D(x) 


#define CFG_KEY "global.C"

extern const char *const config_keys[] = {
    "C_CFG_PW", "C_TZ",
#ifdef USE_NETWORK
    "C_NW_CONN",
#endif
    "C_STM_INV_BP",
#ifdef USE_LPH
    "C_LPH",
#endif
    };

bool config_item_modified(enum configItem item) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    switch (item) {
    default:
      break;
    }
    kvs_close(h);
  }
  return true;
}

