/*
 * config.c
 *
 *  Created on: 10.09.2018
 *      Author: bertw
 */


/*
 * config_defaults.c
 *
 *  Created on: 16.09.2017
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "config/config.h"
#include "config/config_defaults.h"

#define D(x) 

config C = {

};

#ifdef USE_NETWORK
enum nwConnection config_read_network_connection() {
  return config_read_item_i8(CB_NETWORK_CONNECTION, MY_NETWORK_CONNECTION);
}
#endif

#ifdef POSIX_TIME
const char* config_read_tz(char *d, unsigned d_size) {
  return config_read_item_s(CB_TZ, d, d_size, MY_GEO_TZ);
}
#endif

bool read_config_stm32_inv_bootpin() {
  return !!config_read_item_i8(CB_STM32_INV_BOOTPIN, MY_STM32_INV_BOOTPIN);
}
