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
#include "user_config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_err.h"
#include "config/config.h"
#include "../sensitive/defaults.h" //XXX


#define D(x) 

#define CFG_NAMESPACE "Tronferno"
#define CFG_KEY "global.C"

config C = {
#ifdef USE_WLAN
  .wifi_SSID = MY_WIFI_SSID,
  .wifi_password = MY_WIFI_PASSWORD,
#endif
#ifdef USE_MQTT
  .mqtt_url = MY_MQTT_URL,
  .mqtt_user = MY_MQTT_USER,
  .mqtt_password = MY_MQTT_PASSWORD,
  MY_MQTT_ENABLE,
#endif
#ifdef USE_HTTP
  .http_user = MY_HTTP_USER,
  .http_password = MY_HTTP_PASSWORD,
  .http_enable = MY_HTTP_ENABLE,
#endif
#ifdef USE_NTP
  .ntp_server = MY_NTP_SERVER,
#endif
};

#ifdef CONFIG_DICT

void read_config(uint32_t mask) {
  esp_err_t err = 0;
  nvs_handle handle;
  size_t len;

  if ((err = nvs_open(CFG_NAMESPACE, NVS_READONLY, &handle)) == ESP_OK) {



    if (mask & CONFIG_VERBOSE) {
      int8_t temp;
      if (ESP_OK == nvs_get_i8(handle, "C_VERBOSE", &temp))
        C.app_verboseOutput = temp;
    }
#ifdef USE_SERIAL
    if (mask & CONFIG_BAUD) {
      nvs_get_u32(handle, "C_BAUD", &C.mcu_serialBaud);
    }
#endif
#ifdef USE_POSIX_TIME
    if (mask & CONFIG_TZ) {
      (len = sizeof C.geo_tz), nvs_get_str(handle, "C_TZ", C.geo_tz, &len);
}
#endif
#ifdef USE_WLAN
    if (mask & CONFIG_WIFI_SSID) {
      (len = sizeof C.wifi_SSID), err = nvs_get_str(handle, "C_WIFI_SSID", C.wifi_SSID, &len);
      D(ets_printf("read wifi-ssid err=%x\n", err));
    }

    if (mask & CONFIG_WIFI_PASSWD) {
      (len = sizeof C.wifi_password), nvs_get_str(handle, "C_WIFI_PASSWD", C.wifi_password, &len);
    }
#endif

#ifdef USE_NTP
    if (mask & CONFIG_NTP_SERVER) {
      (len = sizeof C.ntp_server), err = nvs_get_str(handle, "C_NTP_SERVER", C.ntp_server, &len);
    }
#endif
#ifdef USE_MQTT
    if (mask & CONFIG_MQTT_URL) {
      (len = sizeof C.mqtt_url), nvs_get_str(handle, "C_MQTT_URL", C.mqtt_url, &len);
    }
    if (mask & CONFIG_MQTT_USER) {
      (len = sizeof C.mqtt_user), nvs_get_str(handle, "C_MQTT_USER", C.mqtt_user, &len);
    }
    if (mask & CONFIG_MQTT_PASSWD) {
      (len = sizeof C.mqtt_password), nvs_get_str(handle, "C_MQTT_PASSWD", C.mqtt_password, &len);
    }
    if (mask & CONFIG_MQTT_ENABLE) {
      int8_t temp;
      if (ESP_OK == nvs_get_i8(handle, "C_MQTT_ENABLE", &temp))
        C.mqtt_enable = temp;
    }
#endif
#ifdef USE_HTTP
    if (mask & CONFIG_HTTP_USER) {
      (len = sizeof C.http_user), nvs_get_str(handle, "C_HTTP_USER", C.http_user, &len);
    }
    if (mask & CONFIG_HTTP_PASSWD) {
      (len = sizeof C.http_password), nvs_get_str(handle, "C_HTTP_PASSWD", C.http_password, &len);
    }
    if (mask & CONFIG_HTTP_ENABLE) {
      int8_t temp;
      if (ESP_OK == nvs_get_i8(handle, "C_HTTP_ENABLE", &temp))
        C.http_enable = temp;
    }
#endif

    nvs_close(handle);
  }

}

void save_config(uint32_t mask) {
  esp_err_t err = 0;
  nvs_handle handle;
  size_t len;
  
  if ((err = nvs_open(CFG_NAMESPACE, NVS_READWRITE, &handle)) == ESP_OK) {

    if (mask & CONFIG_VERBOSE) {
      nvs_set_i8(handle, "C_VERBOSE", C.app_verboseOutput);
    }

#ifdef USE_SERIAL
    if (mask & CONFIG_BAUD) {
      nvs_set_u32(handle, "C_BAUD", C.mcu_serialBaud);
    }
#endif
#ifdef USE_WLAN
    if (mask & CONFIG_WIFI_SSID) {
      err = nvs_set_str(handle, "C_WIFI_SSID", C.wifi_SSID);
    }

    if (mask & CONFIG_WIFI_PASSWD) {
      nvs_set_str(handle, "C_WIFI_PASSWD", C.wifi_password);
    }
#endif

#ifdef USE_NTP
    if (mask & CONFIG_NTP_SERVER) {
      err = nvs_set_str(handle, "C_NTP_SERVER", C.ntp_server);
    }
#endif

#ifdef USE_POSIX_TIME
    if (mask & CONFIG_TZ) {
      nvs_set_str(handle, "C_TZ", C.geo_tz);
    }
#endif

#ifdef USE_MQTT
    if (mask & CONFIG_MQTT_URL) {
      nvs_set_str(handle, "C_MQTT_URL", C.mqtt_url);
    }
    if (mask & CONFIG_MQTT_USER) {
      nvs_set_str(handle, "C_MQTT_USER", C.mqtt_user);
    }
    if (mask & CONFIG_MQTT_PASSWD) {
      nvs_set_str(handle, "C_MQTT_PASSWD", C.mqtt_password);
    }
    if (mask & CONFIG_MQTT_ENABLE) {
      nvs_set_i8(handle, "C_MQTT_ENABLE", C.mqtt_enable);
    }
#endif
#ifdef USE_HTTP
    if (mask & CONFIG_HTTP_USER) {
      nvs_set_str(handle, "C_HTTP_USER", C.http_user);
    }
    if (mask & CONFIG_HTTP_PASSWD) {
      nvs_set_str(handle, "C_HTTP_PASSWD", C.http_password);
    }
    if (mask & CONFIG_HTTP_ENABLE) {
      nvs_set_i8(handle, "C_HTTP_ENABLE", C.http_enable);
    }
#endif
    nvs_commit(handle);
    nvs_close(handle);
  } else {
    D(ets_printf("error: cannot open config in nvs (err=%x)\n", err));
  }
}

#endif
