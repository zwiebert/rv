#include "user_config.h"
#include <time/real_time_clock.h>
#include <string.h>
#include "cli_imp.h"
#include <stdio.h>
#include "peri/uart.h"
#include "water_pump/water_pump.h"
#include "rv/rv_timer.hh"
#include "rv/rain_sensor.hh"

#define warning_unknown_option(x)

#define KEY_VERSION "version"


extern "C" int
process_parmMcu(clpar p[], int len) {

  for (int arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;
    char buf[128];

    if (key == NULL) {
      return -1;
    } else if (std::strcmp(key, KEY_VERSION) == 0 && *val == '?') {
      std::snprintf(buf, sizeof buf, "{ \"from\":\"rv\", \"mcu\":{ \"version\"=\"%s\"}}", VERSION);
      esp32_puts(buf);
    } else {
      warning_unknown_option(key);
    }
  }

  return 0;
}
