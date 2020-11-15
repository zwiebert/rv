#include "parm_config.h"

#include "app_config/proj_app_cfg.h"

#include "app_cli/cli_config.h"
#include "app_misc/opt_map.hh"
#include <string.h>


#include "app_uout/status_output.h"
#include "app_config/proj_app_cfg.h"
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
#include "app_misc/rtc.h"
#include "utils_misc/bcd.h"
#include "cli_imp.h"
#include "utils_misc/stof.h"
#include "utils_misc/cstring_utils.h"
#ifdef USE_MQTT
#include "app_mqtt/mqtt.h"
#endif
#ifdef USE_HTTP
#include "net_http_server/http_server_setup.h"
#endif
#ifdef USE_NTP
#include "net/ntp_client_setup.h"
#endif


#include "utils_misc/int_types.h"
#include "utils_misc/int_types.h"
#include <utils_misc/mutex.hh>
#include <utils_misc/int_macros.h>

#include <stdlib.h>
#include <iterator>
#include <algorithm>


bool process_parmConfig_get_app(otok kt, const char *val, const struct TargetDesc &td) {
  switch (kt) {

  case otok::k_stm32_bootgpio_inv:
    soCfg_STM32_BOOTGPIO_INV(td);
    return true;

  default:
    return false;
  }
}

bool process_parmConfig_app(otok kt, const char *key, const char *val, const struct TargetDesc &td, int &errors, u32 &changed_mask) {
  switch (kt) {

  case otok::NONE:
    return false;
  default:
    return false;
  }
  return true;
}


void parmConfig_reconfig_app(uint32_t changed_mask) {
}
