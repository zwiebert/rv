/*
 * status_output.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "app_config/options.hh"
#include "so_out.h"
#include "so_print.h"
//#include "app_uout/callbacks.h"
#include "app_misc/common.h"
#include "app_misc/firmware.h"
#include "app_misc/rtc.h"
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
#include "net/ipnet.h"
#include "txtio/inout.h"
#include "uout/status_json.hh"
#include "app_uout/status_output.h"
#include <app_uout/so_config.h>
#include <utils_time/run_time.h>

//#include "utils_misc/int_macros.h"

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define D(x)

void soCfg_BAUD(const struct TargetDesc &td) {
#ifndef MCU_ESP32
  td.so().print(settings_get_optKeyStr(CB_BAUD), config_read_baud());
#endif
}

void soCfg_RTC(const struct TargetDesc &td) {
  char buf[64];
  if (rtc_get_by_string(buf)) {
    td.so().print(otok_strings[(int)otok::k_rtc], buf);
  }
}

void soCfg_NETWORK(const struct TargetDesc &td) {
#ifdef USE_NETWORK
  td.so().print(settings_get_optKeyStr(CB_NETWORK_CONNECTION),cfg_args_network[config_read_network_connection()]);
#endif
}

void soCfg_TZ(const struct TargetDesc &td) {
#ifdef USE_POSIX_TIME
  char buf[64];
  td.so().print(settings_get_optKeyStr(CB_TZ),config_read_tz(buf, sizeof buf));
#endif
}

void soCfg_GPIO_PIN(const struct TargetDesc &td, const int gpio_number) {
#ifdef ACCESS_GPIO
  {
    char buf[64];
    char key[10] = "gpio";
    STRCPY(key + 4, itoa(gpio_number, buf, 10));
    char ps[2] = "x";
    if (is_gpio_number_usable(gpio_number, true)) {
      enum mcu_pin_mode mps = pin_getPinMode(gpio_number);
      ps[0] = pin_state_args[mps];
    }
    td.so().print(key, ps);
  }
#endif
}

void soCfg_GPIO_MODES(const struct TargetDesc &td) {
#ifdef ACCESS_GPIO
  {
    char buf[64];
    int gpio_number;
    char key[10] = "gpio";
    char pin_level_args[] = "mhl";
    for (gpio_number = 0; gpio_number < CONFIG_GPIO_SIZE; ++gpio_number) {
      STRCPY(key + 4, itoa(gpio_number, buf, 10));
      char ps[3] = "x";
      if (is_gpio_number_usable(gpio_number, true)) {
        enum mcu_pin_mode mps = pin_getPinMode(gpio_number);
        if (mps != PIN_DEFAULT) {
          enum mcu_pin_level mpl = pin_getPinLevel(gpio_number);
          ps[0] = pin_mode_args[mps];
          ps[1] = pin_level_args[3 - mpl];
          td.so().print(key, ps);
        }
      }

    }
  }
#endif
}

void soCfg_GPIO_MODES_AS_STRING(const struct TargetDesc &td) {
#ifdef ACCESS_GPIO
  {
    int gpio_number;
    char val[CONFIG_GPIO_SIZE + 1];
    val[CONFIG_GPIO_SIZE] = '\0';
    for (gpio_number = 0; gpio_number < CONFIG_GPIO_SIZE; ++gpio_number) {
      if (is_gpio_number_usable(gpio_number, true)) {
        enum mcu_pin_mode mps = pin_getPinMode(gpio_number);
        val[gpio_number] = pin_mode_args[mps];
      } else {
        val[gpio_number] = 'x';
      }
    }
    td.so().print("gpio", val);
  }
#endif
}


void soCfg_STM32_BOOTGPIO_INV(const struct TargetDesc &td) {
  td.so().print(settings_get_optKeyStr(CB_STM32_INV_BOOTPIN), config_read_stm32_inv_bootpin());
}


/*
 * status_output.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "app_config/options.hh"
#include <app_uout/so_config.h>
#include "so_out.h"
#include "so_print.h"
#include "utils_misc/int_types.h"
#include "app_misc/firmware.h"
#include "app_misc/rtc.h"
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
#include "net/ipnet.h"
#include "txtio/inout.h"
#include "uout/status_json.hh"
#include "app_uout/status_output.h"

#include <app_uout/so_config.h>

//#include "utils_misc/int_macros.h"

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define D(x)

void soCfg_begin(const struct TargetDesc &td) {
  td.so().x_open("config");
}

void soCfg_end(const struct TargetDesc &td) {
  td.so().x_close();
}

void soCfg_all_net(const struct TargetDesc &td) {
  soCfg_NETWORK(td);
#ifdef USE_WLAN
  soCfg_WLAN_SSID(td);
  soCfg_WLAN_PASSWORD(td);
#endif
#ifdef USE_NTP
  soCfg_NTP_SERVER(td);
#endif
#ifdef USE_LAN
  soCfg_LAN_PHY(td);
  soCfg_LAN_PWR_GPIO(td);
#endif
#ifdef USE_MQTT
  soCfg_MQTT_ENABLE(td);
  soCfg_MQTT_URL(td);
  soCfg_MQTT_USER(td);
  soCfg_MQTT_PASSWORD(td);
  soCfg_MQTT_CLIENT_ID(td);
  soCfg_MQTT_ROOT_TOPIC(td);
#endif
#ifdef USE_HTTP
  soCfg_HTTP_ENABLE(td);
  soCfg_HTTP_USER(td);
  soCfg_HTTP_PASSWORD(td);
#endif
}

void soCfg_all_rv(const struct TargetDesc &td) {
  soCfg_STM32_BOOTGPIO_INV(td);
}

void soCfg_all_time(const struct TargetDesc &td) {
  soCfg_RTC(td);
#ifdef USE_POSIX_TIME
  soCfg_TZ(td);
#else
  soCfg_TIMEZONE(td);
  soCfg_DST(td);
#endif
}
void soCfg_all(const struct TargetDesc &td) {
  soCfg_VERBOSE(td);

  soCfg_all_time(td);
  soCfg_all_net(td);
  soCfg_all_rv(td);
}

