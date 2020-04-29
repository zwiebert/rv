/*
 * config.h
 *
 *  Created on: 15.05.2019
 *      Author: bertw
 */

#ifndef MAIN_CONFIG_CONFIG_H_
#define MAIN_CONFIG_CONFIG_H_

#include "app_config/proj_app_cfg.h"
#include "config_kvs/config.h"


#ifdef USE_LAN
#include "net/ethernet.h"
#endif
#ifdef USE_MQTT
#include "net/mqtt/mqtt.h"
#endif
#ifdef USE_WLAN
#include "net/wifistation.h"
#endif
#ifdef USE_HTTP
#include "net/http/server/http_server.h"
#endif
#ifdef USE_NTP
#include "net/ntp.h"
#endif
#include <stdint.h>

#ifdef USE_NETWORK
enum nwConnection {
  nwNone, nwWlanSta, nwWlanAp, nwLan, nwLEN,
};
#endif

typedef struct {
	char app_configPassword[16];
#ifdef USE_SERIAL
	uint32_t mcu_serialBaud;
#endif
#ifdef USE_NETWORK
  enum nwConnection network;
#endif

  uint8_t stm32_inv_bootpin;

} config;

extern config C;

enum configAppItem {
  CBA_start = CB_size - 1,
  CB_CFG_PASSWD,  CB_TZ,
#ifdef USE_NETWORK
  CB_NETWORK_CONNECTION,
#endif
  CB_STM32_INV_BOOTPIN,

//-----------
  CBA_size
};

// save C to persistent storage
void save_config_all();
void save_config_item(enum configItem item);
void save_config(uint32_t mask);

// restore C from persistent storage
void read_config_all();
void read_config_item(enum configItem item);
void read_config(uint32_t mask);

void config_setup_global();
void config_setup_mqttAppClient();
bool config_item_modified(enum configItem item);

const char *config_read_tz(char *d, unsigned d_size);
enum nwConnection  config_read_network_connection();
bool read_config_stm32_inv_bootpin();


#endif /* MAIN_CONFIG_CONFIG_H_ */
