/*
 * status_output.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */
#include "user_config.h"


#include "status_output.h"
#ifdef USE_JSON
#include "status_json.h"
#else
#define so_json_config_reply(a,b,c)
#define so_jco false
#endif
#define so_jto so_jco

#include <string.h>
#include <stdio.h>

#include "cli/cli_imp.h" // FIXME?
#include "config/config.h"
#include "main/rtc.h"
#include "misc/int_macros.h"
#include "userio/inout.h"
#include "userio/mqtt.h"
#include "userio/status_json.h"
#include "userio/ipnet.h"

#define D(x)

uint8_t so_target;

//key strings used for parsing and printing config commands by CLI/HTTP/MQTT
//keys must be in same order as their SO_CFG_xxx counterparts in so_msg_t
const char * const cfg_keys[] = {
    "cu", "baud", "rtc",
    "wlan-ssid", "wlan-password", "ntp-server",
    "longitude", "latitude", "timezone", "dst", "tz", "verbose",
    "mqtt-enable", "mqtt-url", "mqtt-user", "mqtt-password",
    "http-enable", "http-user", "http-password",
    "gm-used",
};

bool out_cli = true;
bool out_js = true;

#define so_cco out_cli
#define so_cto so_cco
#define so_jco out_js

char *ICACHE_FLASH_ATTR ftoa(float f, char *buf, int n) {
  int i;
  int32_t mult;
  uint32_t rop;
  int16_t lop = (int16_t) f;
  char *s = buf;

  itoa(lop, s, 10);
  strcat(s, ".");
  s += strlen(s);

  f -= lop;

  mult = (lop < 0) ? -1 : 1;

  for (i = 0; i < n; ++i)
    mult *= 10;

  rop = (uint32_t) (f * mult);
  ltoa(rop, s, 10);

  return buf;
}

// get the so_msg_t for a config key string (or SO_NONE)
so_msg_t ICACHE_FLASH_ATTR
so_parse_config_key(const char *k) {
  int i;
  for (i = 0; i < (sizeof cfg_keys / sizeof cfg_keys[0]); ++i) {
    if (0 == strcmp(k, cfg_keys[i]))
      return i + SO_CFG_begin + 1;
  }
  return SO_NONE;
}

// get config key string for a so_msgt_t (or NULL)
static const char *
gk(so_msg_t so_key) {
  if (SO_CFG_begin < so_key && so_key < SO_CFG_end)
    return cfg_keys[so_key - (SO_CFG_begin + 1)];
  return NULL;
}

void so_out_config_reply_entry2(const char *key, const char *val) {
  if (so_cco) cli_out_config_reply_entry(key, val, 0);
}

void so_out_config_reply_entry(so_msg_t key, const char *val) {
  if (so_cco) so_out_config_reply_entry2(gk(key), val);
}

void so_out_config_reply_entry_s(so_msg_t key, const char *val) {
  if (so_cco) so_out_config_reply_entry2(gk(key), val);
  if (so_jco) so_json_config_reply(gk(key), val, false);
}

void so_out_config_reply_entry_d(so_msg_t key, int val) {
  char buf[20];
  itoa(val, buf, 10);
  if (so_cco) so_out_config_reply_entry2(gk(key), buf);
  if (so_jco) so_json_config_reply(gk(key), buf, true);
}

void so_out_config_reply_entry_l(so_msg_t key, int val) {
  char buf[20];
  ltoa(val, buf, 10);
  if (so_cco) so_out_config_reply_entry2(gk(key), buf);
  if (so_jco) so_json_config_reply(gk(key), buf, true);
}

void so_out_config_reply_entry_lx(so_msg_t key, int val) {
  char buf[20];
  ltoa(val, buf, 16);
  if (so_cco) so_out_config_reply_entry2(gk(key), buf);
  if (so_jco) so_json_config_reply(gk(key), buf, false); //no hex in jason. use string
}

void so_out_config_reply_entry_f(so_msg_t key, float val, int n) {
  char buf[20];
  ftoa(val, buf, 5);
  if (so_cco) so_out_config_reply_entry2(gk(key), buf);
  if (so_jco) so_json_config_reply(gk(key), buf, true);
}


static void so_print_startup_info(void);

void ICACHE_FLASH_ATTR so_output_message(so_msg_t mt, void *arg) {
  static uint16_t pras_msgid, cuas_msgid;
  char buf[20];
  int i;

  switch (mt) {
  case SO_FW_START_MSG_PRINT:
    so_print_startup_info();
    break;

    /////////////////////////////////////////////////////////////////////////////////
  case SO_SEP_ENABLE:
    D(io_puts("sep enable\n"));
    break;
  case SO_SEP_DISABLE:
    D(io_puts("sep disable\n"));
    break;
  case SO_SEP_BUTTON_PRESSED_ERROR:
    io_puts("error: hardware button is pressed\n");
    break;

  /////////////////////////////////////////////////////////////////////////////////
  case SO_CFG_all: {
    bool old_out_js = out_js, old_out_cli = out_cli;
    char *json_buf = NULL;

    if (arg) {
      const char *f;

      out_js = out_cli = false;
      for (f = arg; *f; ++f)
        switch (*f) {
        case 'j':
          out_js = true;
          break;
        case 'c':
          out_cli = true;
          break;
        }

    }
    if (!out_js || ((json_buf = malloc(256)))) {
      for (i = SO_CFG_begin; i <= SO_CFG_end; ++i) {
        so_output_message(i, NULL);
      }
      free(json_buf);
    }
    out_js = old_out_js;
    out_cli = old_out_cli;
  }
    break;

  case SO_CFG_BAUD:
    so_out_config_reply_entry_l(mt, C.mcu_serialBaud);
    break;
  case SO_CFG_RTC:
    if (rtc_get_by_string(buf)) {
      so_out_config_reply_entry_s(mt, buf);
    }
    break;
#ifdef USE_WLAN
  case SO_CFG_WLAN_SSID:
    so_out_config_reply_entry_s(mt, C.wifi_SSID);
    break;
  case SO_CFG_WLAN_PASSWORD:
    so_out_config_reply_entry_s(mt, *C.wifi_password ? "*" : "");
    break;
#endif
#ifdef USE_NTP
  case SO_CFG_NTP_SERVER:
    so_out_config_reply_entry_s(mt, C.ntp_server);
    break;
#endif
#ifdef USE_MQTT
  case SO_CFG_MQTT_ENABLE:
    so_out_config_reply_entry_d(mt, C.mqtt_enable ? 1 : 0);
    break;
  case SO_CFG_MQTT_URL:
    so_out_config_reply_entry_s(mt, C.mqtt_url);
    break;
  case SO_CFG_MQTT_USER:
    so_out_config_reply_entry_s(mt, C.mqtt_user);
    break;
  case SO_CFG_MQTT_PASSWORD:
    so_out_config_reply_entry_s(mt, *C.mqtt_password ? "*" : "");
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
    so_out_config_reply_entry_d(mt, C.http_enable ? 1 : 0);
    break;
  case SO_CFG_HTTP_USER:
    so_out_config_reply_entry_s(mt, C.http_user);
    break;
  case SO_CFG_HTTP_PASSWORD:
    so_out_config_reply_entry_s(mt, *C.http_password ? "*" : "");
    break;
#else
  case SO_CFG_HTTP_ENABLE:
  case SO_CFG_HTTP_USER:
  case SO_CFG_HTTP_PASSWORD:
    break;
#endif

  case SO_CFG_VERBOSE:
    so_out_config_reply_entry_d(mt, C.app_verboseOutput);
    break;
    case SO_CFG_TZ:
#ifdef POSIX_TIME
      so_out_config_reply_entry_s(mt, C.geo_tz);
#endif
    break;
  case SO_CFG_DST:
#ifdef MDR_TIME
  {
    const char *dst = (C.geo_dST == dstEU ? "eu" : (C.geo_dST == dstNone ? "0" : "1"));
    so_out_config_reply_entry_s(mt, dst);
  }
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
      so_out_config_reply_entry2(key, ps);
    }
#endif
    break;

  case SO_CFG_begin:
    break;

  case SO_CFG_end:
    so_out_config_reply_entry_s(SO_NONE, NULL);
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
  case SO_CUAS_START:
    cuas_msgid = *(uint16_t *) arg;
    io_puts("U: Press Stop on the Fernotron central unit\n");
    break;
  case SO_CUAS_TIMEOUT:
    io_puts("U: Nothing received\n");
    reply_id_message(cuas_msgid, "cuas=time-out", 0);
    break;

  case SO_CUAS_DONE:
    io_puts("U: Central Unit received and stored\n");
    reply_message("cuas=ok", 0);
    break;

    /////////////////////////////////////////////////////////////////////////////////
  case SO_PRAS_START_LISTENING:
    pras_msgid = *(uint16_t *) arg;
    io_puts("U:pras: start listening at RF\n");
    break;
  case SO_PRAS_STOP_LISTENING:
    if (arg && *(uint16_t *) arg) {
      io_puts("U:pras: success\n");
    } else {
      io_puts("U:pras: failure\n");
    }
    break;

    /////////////////////////////////////////////////////////////////////////////////
  case SO_TIMER_EVENT_PRINT: {
    so_arg_gm_t *a = arg;

  }
    break;

  case SO_TIMER_PRINT: {
    so_arg_gm_t *a = arg;

  }
    break;

    /////////////////////////////////////////////////////////////////////////////////

  case SO_POS_begin:
    io_puts("U:position:start;\n");
    break;

  case SO_POS_end:
    io_puts("U:position:end;\n");
    break;

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


static void ICACHE_FLASH_ATTR
so_print_startup_info(void) {
  static const char msg_starting[] = "\n\n" "tf: info: start: tronferno-mcu\n" "tf: info: build-date: " __DATE__ " " __TIME__ "\n";
  static const char msg_hint[] = "tf: hint: type 'help;' to get a command list\n";

  if (C.app_verboseOutput >= vrbNone) {
    io_puts(msg_starting);
    io_puts(msg_hint);
  }
}

