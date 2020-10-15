/*
 * config_comps.c
 *
 *  Created on: 21.04.2020
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "app_settings/config.h"
#include "app_settings/config_defaults.h"
#include "config_kvs.h"
#include "utils_misc/int_macros.h"
#include "key_value_store/kvs_wrapper.h"
#include "utils_misc/int_types.h"




#if 1
void config_setup_global() {
  kvshT h;
  C = config {
    .network = MY_NETWORK_CONNECTION,
  };
if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
  C.network = static_cast<nwConnection>(kvs_get_i8(h, cfg_key(CB_NETWORK_CONNECTION), C.network, 0));
  kvs_close(h);
}

}
#endif



#ifdef USE_MQTT
#include "app_mqtt/mqtt.h"
void config_setup_mqttAppClient() {
  io_mqttApp_setup("rv/");
  config_setup_mqttClient();
}
#endif
