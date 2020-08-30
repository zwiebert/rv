/*
 * config_comps.c
 *
 *  Created on: 21.04.2020
 *      Author: bertw
 */

#include "config/config.h"
#include "config/config_defaults.h"
#include "config_kvs.h"

#include "misc/int_macros.h"
#include "key_value_store/kvs_wrapper.h"
#include "misc/int_types.h"




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
#include "net/mqtt/app/mqtt.h"
void config_setup_mqttAppClient() {
  cfg_mqtt_app cfg = { "rv/" };
  io_mqttApp_setup(&cfg);
  config_setup_mqttClient();
}
#endif
