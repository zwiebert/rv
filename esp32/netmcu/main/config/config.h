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

#define CONFIG_ALL (~0UL)
#define CONFIG_RECV (1UL << 0)
#define CONFIG_TRANSM (1UL << 1)
#define CONFIG_CUID (1UL << 2)
#define CONFIG_USED_MEMBERS (1UL << 3)
#define CONFIG_BAUD (1UL << 4)
#define CONFIG_GPIO (1UL << 5)
#define CONFIG_VERBOSE (1UL << 6)
#define CONFIG_WIFI_SSID (1UL << 7)
#define CONFIG_WIFI_PASSWD (1UL << 8)
#define CONFIG_CFG_PASSWD (1UL << 9)
#define CONFIG_LONGITUDE (1UL << 10)
#define CONFIG_LATITUDE (1UL << 11)
#define CONFIG_TIZO (1UL << 12)
#define CONFIG_TZ (1UL << 13)
#define CONFIG_DST (1UL << 14)
#define CONFIG_MQTT_URL (1UL << 15)
#define CONFIG_MQTT_USER (1UL << 16)
#define CONFIG_MQTT_PASSWD (1UL << 17)
#define CONFIG_MQTT_ENABLE (1UL << 18)
#define CONFIG_HTTP_USER (1UL << 19)
#define CONFIG_HTTP_PASSWD (1UL << 20)
#define CONFIG_HTTP_ENABLE (1UL << 21)
#define CONFIG_NTP_SERVER (1UL << 22)

#endif /* MAIN_CONFIG_CONFIG_H_ */
