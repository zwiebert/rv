/*
 * config.h
 *
 *  Created on: 15.05.2019
 *      Author: bertw
 */

#ifndef MAIN_CONFIG_CONFIG_H_
#define MAIN_CONFIG_CONFIG_H_

#include "app_config/proj_app_cfg.h"
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
	enum verbosity app_verboseOutput;
	char app_configPassword[16];
#ifdef USE_SERIAL
	uint32_t mcu_serialBaud;
#endif
#ifdef USE_WLAN
  struct cfg_wlan wifi;
#endif
#ifdef USE_MQTT
  struct cfg_mqtt mqtt;
#endif
#ifdef USE_HTTP
  struct cfg_http http;
#endif
#ifdef USE_POSIX_TIME
  char geo_tz[32];
#endif
#ifdef USE_NTP
  struct cfg_ntp ntp;
#endif
#ifdef USE_NETWORK
  enum nwConnection network;
#endif
#ifdef USE_LAN
  struct cfg_lan lan;
#endif
  uint8_t stm32_inv_bootpin;

} config;

extern config C;

#define cfg_getWlan() &C.wifi
#define cfg_getLan() &C.lan
#define cfg_getMqttClient() &C.mqtt
#define cfg_getHttpServer() &C.http
#define cfg_getNtpClient() &C.ntp
#define cfg_getTxtio() (struct cfg_txtio *)&C.app_verboseOutput

enum configItem {
  CB_VERBOSE,
#ifdef USE_WLAN
  CB_WIFI_SSID, CB_WIFI_PASSWD,
#endif
  CB_CFG_PASSWD, CB_LONGITUDE, CB_LATITUDE,
#ifndef USE_POSIX_TIME
CB_TIZO,
#else
  CB_TZ,
#endif
#ifdef USE_MQTT
  CB_MQTT_URL, CB_MQTT_USER, CB_MQTT_PASSWD, CB_MQTT_ENABLE,
#endif
#ifdef USE_HTTP
  CB_HTTP_USER, CB_HTTP_PASSWD, CB_HTTP_ENABLE,
#endif
#ifdef USE_NTP
  CB_NTP_SERVER,
#endif
#ifdef USE_NETWORK
  CB_NETWORK_CONNECTION, CB_LAN_PHY, CB_LAN_PWR_GPIO,
#endif
  CB_STM32_INV_BOOTPIN,

//-----------
  CB_size
};

// save C to persistent storage
void save_config_all();
void save_config_item(enum configItem item);
void save_config(uint32_t mask);

// restore C from persistent storage
void read_config_all();
void read_config_item(enum configItem item);
void read_config(uint32_t mask);

void config_setup();

#endif /* MAIN_CONFIG_CONFIG_H_ */
