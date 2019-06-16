/*
 * config.h
 *
 *  Created on: 15.05.2019
 *      Author: bertw
 */

#ifndef MAIN_CONFIG_CONFIG_H_
#define MAIN_CONFIG_CONFIG_H_

#include "user_config.h"

#if __has_include("../../sensitive/defaults.h")
#define HAVE_USER_DEFAULTS
#include "../../sensitive/defaults.h"
#else
#error
#endif

enum verbosity {
	vrbNone, vrb1, vrb2, vrb3, vrb4, vrbDebug
};

typedef struct {
	enum verbosity app_verboseOutput;
	char app_configPassword[16];
#ifdef USE_SERIAL
	uint32_t mcu_serialBaud;
#endif
#ifdef USE_WLAN
    char wifi_SSID[32];
    char wifi_password[64];
#endif
#ifdef USE_HTTP
  char http_user[16];
  char http_password[31];
  int8_t http_enable;
#endif
#ifdef USE_MQTT
  char mqtt_url[64];
  char mqtt_user[16];
  char mqtt_password[31];
  int8_t mqtt_enable;
#endif
#ifdef USE_POSIX_TIME
  char geo_tz[32];
#endif
#ifdef USE_NTP
  char ntp_server[64];
#endif

} config;

extern config C;

void read_config(uint32_t mask);  // restore C from persistent storage
void save_config(uint32_t mask);  // save C to persistent storage

enum {
CB_RECV,
CB_TRANSM,
CB_CUID,
CB_USED_MEMBERS,
CB_BAUD,
CB_GPIO,
CB_VERBOSE,
CB_WIFI_SSID,
CB_WIFI_PASSWD,
CB_CFG_PASSWD,
CB_LONGITUDE,
CB_LATITUDE,
CB_TIZO,
CB_TZ,
CB_DST,
CB_MQTT_URL,
CB_MQTT_USER,
CB_MQTT_PASSWD,
CB_MQTT_ENABLE,
CB_HTTP_USER,
CB_HTTP_PASSWD,
CB_HTTP_ENABLE,
CB_NTP_SERVER,
};

#define CONFIG_ALL (~0UL)
#define CONFIG_RECV (1UL << CB_RECV)
#define CONFIG_TRANSM (1UL << CB_TRANSM)
#define CONFIG_CUID (1UL << CB_CUID)
#define CONFIG_USED_MEMBERS (1UL << CB_USED_MEMBERS)
#define CONFIG_BAUD (1UL << CB_BAUD)
#define CONFIG_GPIO (1UL << CB_GPIO)
#define CONFIG_VERBOSE (1UL << CB_VERBOSE)
#define CONFIG_WIFI_SSID (1UL << CB_WIFI_SSID)
#define CONFIG_WIFI_PASSWD (1UL << CB_WIFI_PASSWD)
#define CONFIG_CFG_PASSWD (1UL << CB_CFG_PASSWD)
#define CONFIG_LONGITUDE (1UL << CB_LONGITUDE)
#define CONFIG_LATITUDE (1UL << CB_LATITUDE)
#define CONFIG_TIZO (1UL << CB_TIZO)
#define CONFIG_TZ (1UL << CB_TZ)
#define CONFIG_DST (1UL << CB_DST)
#define CONFIG_MQTT_URL (1UL << CB_MQTT_URL)
#define CONFIG_MQTT_USER (1UL << CB_MQTT_USER)
#define CONFIG_MQTT_PASSWD (1UL << CB_MQTT_PASSWD)
#define CONFIG_MQTT_ENABLE (1UL << CB_MQTT_ENABLE)
#define CONFIG_HTTP_USER (1UL << CB_HTTP_USER)
#define CONFIG_HTTP_PASSWD (1UL << CB_HTTP_PASSWD)
#define CONFIG_HTTP_ENABLE (1UL << CB_HTTP_ENABLE)
#define CONFIG_NTP_SERVER (1UL << CB_NTP_SERVER)

#endif /* MAIN_CONFIG_CONFIG_H_ */
