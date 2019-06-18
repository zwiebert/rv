/*
 * cli_status.c
 *
 *  Created on: 15.06.2019
 *      Author: bertw
 */

#include "user_config.h"

#include <string.h>
#include <stdbool.h>

#include "userio/inout.h"
#include "userio/mqtt.h"
#include "user_config.h"
#include "cli_imp.h"
#include "userio/status_output.h"
#include "userio/status_json.h"
#include "debug/debug.h"
#include "misc/int_macros.h"
#include <stdio.h>

#include <stm32.h>

#define D(x) x

#define STM32_STATUS "status "
#define STM32_STATUS_LEN (sizeof (STM32_STATUS) - 1)
#define KEY_VALVE_BITS "valve-bits"
#define KEY_VALVE_CHANGE_MASK "valve-change-mask"
#define KEY_RAIN_SENSOR "rain"
#define KEY_EVENT "event"
#define KEY_PUMP "pump"

#define BUF_SIZE 128
#define ZONE_COUNT 14
#define MAX_DURATION 60

int ICACHE_FLASH_ATTR
process_parmStatus(clpar p[], int len) {
  int arg_idx;

  bool hasValveBits = false, hasValveChangeMask = false, hasRainSensor = false;
  uint32_t valveBits = 0, valveChangeMask = 0;
  bool rainSensor = 0;


  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;
    D(db_printf("key=%s, val=%s\n", key, val));

    if (key == NULL) {
      return -1;
    } else if (strcmp(key, KEY_VALVE_BITS) == 0) {
      hasValveBits = true;
      valveBits = strtol(val, 0, 16);
    } else if (strcmp(key, KEY_VALVE_CHANGE_MASK) == 0) {
      hasValveChangeMask = true;
      valveChangeMask = strtol(val, 0, 16);
    } else if (strcmp(key, KEY_RAIN_SENSOR) == 0) {
      hasRainSensor = true;
      rainSensor = *val != '0';
    } else if (strcmp(key, KEY_EVENT) == 0) {
      io_mqtt_publish_stm32_event(val);
    } else if (strcmp(key, KEY_PUMP) == 0) {
      bool state = *val != '0';
      io_mqtt_publish_pump_status(state);
    } else {
      warning_unknown_option(key);
    }
  }

  if (hasValveBits && hasValveChangeMask) {
    uint32_t mask = valveChangeMask;
    for (int i=0; mask; ++i, (mask >>= 1)) {
      if (mask & 1) {
        io_mqtt_publish_valve_status(i, GET_BIT(valveBits, i));
      }
    }
  }

  if (hasRainSensor) {
    io_mqtt_publish_rain_sensor_status(rainSensor);
  }

  return 0;
}


