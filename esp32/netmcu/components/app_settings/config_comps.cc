/*
 * config_comps.c
 *
 *  Created on: 21.04.2020
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "app_settings/config.h"
#include "app_settings/config_defaults.h"
#include "app_uout/callbacks.h"
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
  C.network = static_cast<nwConnection>(kvs_get_i8(h, settings_get_kvsKey(CB_NETWORK_CONNECTION), C.network, 0));
  kvs_close(h);
}

}
#endif



#ifdef CONFIG_APP_USE_MQTT
#include "app_mqtt/mqtt.h"
void config_setup_mqttAppClient() {
  struct cfg_mqtt c;
  config_read_mqttClient(&c);
  io_mqttApp_setup(&c);
}
#endif

void config_ext_setup_txtio() {
  uo_flagsT flags;
  flags.evt.uo_evt_flag_valveChange = true;
  config_setup_txtio(&flags);

}

void config_ext_setup_cliTcpServer() {
  uo_flagsT flags;
  flags.evt.uo_evt_flag_valveChange = true;
#ifndef TEST_HOST
  config_setup_cliTcpServer(&flags);
#endif
}
