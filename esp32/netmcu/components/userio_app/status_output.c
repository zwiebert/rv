/*
 * status_output.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "so_out.h"
#include "so_print.h"

#include "app/common.h"
#include "app/ota.h"
#include "stm32/stm32_ota.h"
#include "app/rtc.h"
#include "cli_app/cli_config.h"
#include "cli_app/cli_imp.h" // FIXME?
#include "config/config.h"
#include "app/kvstore.h"
#include "net/ipnet.h"
#include "net/mqtt/app/mqtt.h"
#include "txtio/inout.h"
#include "userio/status_json.h"
#include "userio/status_json.h"
#include "userio_app/status_output.h"

#include "misc/int_macros.h"
#include "misc/int_types.h"

#include <string.h>
#include <stdio.h>

#define D(x)

u8 so_target;



void so_output_message(so_msg_t mt, const void *arg) {
  char buf[64];
  int i;

  switch (mt) {
  case SO_FW_START_MSG_PRINT:
    so_print_startup_info();
    break;

  case SO_STATUS_OK:
    so_out_x_reply_entry_ss("status", "ok");
    break;

  case SO_STATUS_ERROR:
    so_out_x_reply_entry_ss("status", "error");
    break;

  case SO_MCU_begin: {
    so_out_x_open("mcu");
  }
    break;

  case SO_MCU_RUN_TIME: {
    so_out_x_reply_entry_sl("run-time", run_time());

  }
    break;

  case SO_MCU_VERSION: {
    so_out_x_reply_entry_ss("chip", MCU_TYPE);

    so_out_x_reply_entry_ss("firmware", strcat(strcpy(buf, "tronferno-mcu-"), APP_VERSION));

    char *p = strcpy(buf, ISO_BUILD_TIME);
    do
      if (*p == ' ')
        *p = '-';
    while (*++p);
    so_out_x_reply_entry_ss("build-time", buf);
  }
    break;

  case SO_MCU_OTA:
#ifdef USE_OTA
    so_out_x_reply_entry_ss("ota-url", arg);
#endif
    break;
  case SO_MCU_OTA_STATE:
#ifdef USE_OTA
    so_out_x_reply_entry_sd("ota-state", ota_getState());
#endif
    break;

  case SO_MCU_STM32OTA:
#ifdef USE_OTA
    so_out_x_reply_entry_ss("stm32ota-url", arg);
#endif
    break;
  case SO_MCU_STM32OTA_STATE:
#ifdef USE_OTA
    so_out_x_reply_entry_sd("stm32ota-state", stm32ota_getState());
#endif
    break;

  case SO_MCU_BOOT_COUNT: {
    extern i32 boot_counter;
    so_out_x_reply_entry_sl("boot-count", boot_counter);
  }
    break;


  case SO_MCU_end:
    so_out_x_close();
    break;



    /////////////////////////////////////////////////////////////////////////////////


  case SO_KVS_begin: {
    so_out_x_open("kvs");
  }
    break;

  case  SO_KVS_ZN_SINGLE: {
    const char *key = arg;
    if (kvs_get_string(key, buf, sizeof buf)) {
      so_out_x_reply_entry_ss(key, buf);
    }
  }
  break;
  case  SO_KVS_ZN_ALL: {
    const char *keyBase = arg;
    for (int i=0; i < 14; ++i) {
      char key[16];
      snprintf(key, sizeof key, "%s%d", keyBase, i);
      if (kvs_get_string(key, buf, sizeof buf)) {
        so_out_x_reply_entry_ss(key, buf);
      }
    }
  }
  break;

  case SO_KVS_end:
    so_out_x_close();
    break;

  /////////////////////////////////////////////////////////////////////////////////
  case SO_CFG_all: {
    for (i = SO_CFG_begin + 1; i < SO_CFG_end; ++i) {
      so_output_message(i, NULL);
    }
  }
    break;

  case SO_CFG_RTC:
    if (rtc_get_by_string(buf)) {
      so_out_x_reply_entry_s(mt, buf);
    }
    break;
#ifdef USE_NETWORK
  case SO_CFG_NETWORK:
    so_out_x_reply_entry_s(mt, cfg_args_network[C.network]);
    break;
#endif
#ifdef USE_LAN
  case SO_CFG_LAN_PHY:
    so_out_x_reply_entry_s(mt, cfg_args_lanPhy[C.lan.phy]);
    break;
  case SO_CFG_LAN_PWR_GPIO:
    so_out_x_reply_entry_d(mt, C.lan.pwr_gpio);
    break;
#else
  case SO_CFG_LAN_PHY:
  case SO_CFG_LAN_PWR_GPIO:
    break;
#endif
#ifdef USE_WLAN
  case SO_CFG_WLAN_SSID:
    so_out_x_reply_entry_s(mt, C.wifi.SSID);
    break;
  case SO_CFG_WLAN_PASSWORD:
    so_out_x_reply_entry_s(mt, *C.wifi.password ? "*" : "");
    break;
#endif
#ifdef USE_NTP
  case SO_CFG_NTP_SERVER:
    so_out_x_reply_entry_s(mt, C.ntp.server);
    break;
#endif
#ifdef USE_MQTT
  case SO_CFG_MQTT_ENABLE:
    so_out_x_reply_entry_d(mt, C.mqtt.enable ? 1 : 0);
    break;
  case SO_CFG_MQTT_URL:
    so_out_x_reply_entry_s(mt, C.mqtt.url);
    break;
  case SO_CFG_MQTT_USER:
    so_out_x_reply_entry_s(mt, C.mqtt.user);
    break;
  case SO_CFG_MQTT_PASSWORD:
    so_out_x_reply_entry_s(mt, *C.mqtt.password ? "*" : "");
    break;
#else
  case SO_CFG_MQTT_ENABLE:
  case SO_CFG_MQTT_URL:
  case SO_CFG_MQTT_USER:
  case SO_CFG_MQTT_PASSWORD:
    break;
#endif

#ifdef USE_HTTP
  case SO_CFG_HTTP_ENABLE:
    so_out_x_reply_entry_d(mt, C.http.enable ? 1 : 0);
    break;
  case SO_CFG_HTTP_USER:
    so_out_x_reply_entry_s(mt, C.http.user);
    break;
  case SO_CFG_HTTP_PASSWORD:
    so_out_x_reply_entry_s(mt, *C.http.password ? "*" : "");
    break;
#else
  case SO_CFG_HTTP_ENABLE:
  case SO_CFG_HTTP_USER:
  case SO_CFG_HTTP_PASSWORD:
    break;
#endif

  case SO_CFG_VERBOSE:
    so_out_x_reply_entry_d(mt, C.app_verboseOutput);
    break;
    case SO_CFG_TZ:
#ifdef POSIX_TIME
      so_out_x_reply_entry_s(mt, C.geo_tz);
#endif
    break;


    case SO_CFG_GPIO_PIN:
#ifdef ACCESS_GPIO
      {
      int gpio_number = *(int *)arg;
      char key[10] = "gpio";
      strcat(key, itoa(gpio_number, buf, 10));
      char ps[2] = "x";
      if (is_gpio_number_usable(gpio_number, true)) {
        ps[0] = pin_state_args[C.gpio[gpio_number]];
      }
      so_out_x_reply_entry_ss(key, ps);
    }
#endif
    break;

    case SO_CFG_STM32_BOOTGPIO_INV: {
      so_out_x_reply_entry_d(mt, C.stm32_inv_bootpin ? 1 : 0);
    }
    break;

  case SO_CFG_begin:
    so_out_x_open("config");
    break;

  case SO_CFG_end:
    so_out_x_close();
    break;

    /////////////////////////////////////////////////////////////////////////////////
  case SO_CFGPASSWD_OK:
    // io_puts("password ok\n");
    break;
  case SO_CFGPASSWD_WRONG:
    io_puts("wrong config password\n");
    break;
  case SO_CFGPASSWD_MISSING:
    io_puts("missing config password\n");
    break;


    /////////////////////////////////////////////////////////////////////////////////

  case SO_INET_PRINT_ADDRESS: {
#if defined USE_LAN || defined USE_WLAN
    char buf[20];
    ipnet_addr_as_string(buf, 20);
    io_puts("tf: ipaddr: "), io_puts(buf), io_puts(";\n");
#endif
  }
  break;

  default:
#ifndef DISTRIBUTION
    io_puts("internal_error:so_output_message() unhandled message: "), io_putd(mt), io_putlf();
#endif
    break;
  }

}


