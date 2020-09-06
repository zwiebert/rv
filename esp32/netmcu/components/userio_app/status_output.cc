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
#include "cli_app/cli_app.h" // FIXME?
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
#include "misc/base64.h"

#include <string.h>
#include <stdio.h>

#define D(x)

bool so_output_message2(so_msg_t mt, const void *arg);

void so_output_message(so_msg_t mt, const void *arg) {
  char buf[64];
  int i;

  if (so_output_message2(mt, arg))
    return;

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

    so_out_x_reply_entry_ss("firmware", strcat(strcpy(buf, "rv-netmcu-"), APP_VERSION));

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
    so_out_x_reply_entry_ss("ota-url", static_cast<const char *>(arg));
#endif
    break;
  case SO_MCU_OTA_STATE:
#ifdef USE_OTA
    so_out_x_reply_entry_sd("ota-state", ota_getState());
#endif
    break;

  case SO_MCU_STM32OTA:
#ifdef USE_OTA
    so_out_x_reply_entry_ss("stm32ota-url", static_cast<const char *>(arg));
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
    const char *key = static_cast<const char *>(arg);
    if (kvs_get_string(key, buf, sizeof buf)) {
      so_out_x_reply_entry_ss(key, buf);
    }
  }
  break;
  case  SO_KVS_ZN_ALL: {
    const char *keyBase = static_cast<const char *>(arg);
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
      so_output_message(static_cast<so_msg_t>(i), nullptr);
    }
  }
    break;

  case SO_CFG_RTC:
    if (rtc_get_by_string(buf)) {
      so_out_x_reply_entry_s(mt, buf);
    }
    break;
  case SO_CFG_NETWORK:
#ifdef USE_NETWORK
    so_out_x_reply_entry_s(mt, cfg_args_network[config_read_network_connection()]);
#endif
    break;
  case SO_CFG_TZ:
#ifdef POSIX_TIME
    so_out_x_reply_entry_s(mt, config_read_tz(buf, sizeof buf));
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
      so_out_x_reply_entry_d(mt, config_read_stm32_inv_bootpin());
    }
    break;

  case SO_CFG_begin:
    so_out_x_open("config");
    break;

  case SO_CFG_end:
    so_out_x_close();
    break;

    /////////////////////////////////////////////////////////////////////////////////
  case SO_RVE_begin: {
    so_out_x_open("rve");
  }
    break;

  case  SO_RVE_PUMP: {
    const so_arg_on_t *state = static_cast<const so_arg_on_t *>(arg);
    io_mqtt_publish_pump_status(state->on);
    so_out_x_reply_entry_sd("pump", state->on);
  }
  break;

  case  SO_RVE_RAIN: {
    const so_arg_on_t *state = static_cast<const so_arg_on_t *>(arg);
    io_mqtt_publish_rain_sensor_status(state->on);
    so_out_x_reply_entry_sd("rain", state->on);
  }
  break;

  case  SO_RVE_PRESS_CTL: {
    const so_arg_on_t *state = static_cast<const so_arg_on_t *>(arg);
    so_out_x_reply_entry_sd("pc", state->on);
  }
  break;

  case  SO_RVE_VALVES: {
    const so_arg_valves_t *valves = static_cast<const so_arg_valves_t *>(arg);
    so_out_x_reply_entry_sl("valve_state", valves->state_bits);
    so_out_x_reply_entry_sl("valve_change", valves->changed_bits);

    u32 mask = valves->changed_bits;
    for (int i=0; mask; ++i, (mask >>= 1)) {
      if (mask & 1) {
        io_mqtt_publish_valve_status(i, GET_BIT(valves->state_bits, i));
      }
    }
  }
  break;

  case SO_RVE_end:
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
    //char buf[20];
    ipnet_addr_as_string(buf, sizeof buf);
    io_puts("tf: ipaddr: "), io_puts(buf), io_puts(";\n");
#endif
  }
  break;

  case SO_PBUF_begin:
    so_out_x_open("pbuf");
    break;

  case SO_PBUF_KV64: {
    const so_arg_pbuf_t *pba = static_cast<const so_arg_pbuf_t *>(arg);
    size_t b64Len = 0;
    int err = mbedtls_base64_encode((uint8_t *) buf, sizeof buf, &b64Len, pba->buf, pba->buf_len);
    if (!err) {
      buf[b64Len] = '\0';
      so_out_x_reply_entry_ss(pba->key, buf);
    }
  }
    break;

  case SO_PBUF_end:
    so_out_x_close();
    break;

  default:
#ifndef DISTRIBUTION
    io_puts("internal_error:so_output_message() unhandled message: "), io_putd(mt), io_putlf();
#endif
    break;
  }

}


