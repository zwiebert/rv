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
  C = (config){
    .network = MY_NETWORK_CONNECTION,
};

  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    kvsR(i8, CB_NETWORK_CONNECTION, C.network);
    kvs_close(h);
  }

}
#endif



#ifdef USE_MQTT
#include "net/mqtt/app/mqtt.h"
void config_setup_mqttAppClient() {
  config_setup_mqttClient();
}
#endif
