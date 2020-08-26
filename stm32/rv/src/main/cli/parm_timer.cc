#include "user_config.h"
#include <time/real_time_clock.h>
#include <string.h>
#include "cli_imp.h"
#include <stdio.h>
#include "peri/uart.h"
#include "rv/water_pump.h"
#include "rv/rv_timers.hh"
#include "setup/app_cxx.hh"
#include "rv/rain_sensor.hh"

#define warning_unknown_option(x)
extern "C" void timer_set(int8_t channel);

#define KEY_DURATION_PREFIX "dur"
#define KEY_DURATION_PREFIX_LEN ((sizeof KEY_DURATION_PREFIX) - 1)
#define CMD_ASK_DURATIONS " dur=?"
#define CMD_ASK_DURATIONS_LEN (sizeof CMD_ASK_DURATIONS - 1)

#define KEY_REMAINING_PREFIX "rem"
#define KEY_REMAINING_TIME_PREFIX_LEN ((sizeof KEY_REMAINING_TIME_PREFIX) - 1)
#define CMD_ASK_REMAINING_TIMES " rem=?"
#define CMD_ASK_REMAINING_TIMES_LEN (sizeof CMD_ASK_REMAINING_TIMES - 1)

#define KEY_STATUS_PREFIX "status"
#define KEY_STATUS_PREFIX_LEN ((sizeof KEY_STATUS_PREFIX) - 1)
#define CMD_ASK_STATUS " status=?;"
#define CMD_ASK_STATUS_LEN (sizeof CMD_ASK_STATUS - 1)

#define JSON_PREFIX "{\"data\":{"
#define JSON_PREFIX_LEN ((sizeof JSON_PREFIX) - 1)
#define JSON_SUFFIX "}}\n"
#define JSON_SUFFIX_LEN ((sizeof JSON_SUFFIX) - 1)

#define KEY_VERSION "version"

#define BUF_SIZE 2048

const char help_parmTimer[] = "zone=[0-13]      zone number\n"
    "duration=[0-60]  how long to irrigate\n";

int process_parmTimer(clpar p[], int len) {
  int arg_idx;
  RvTimer::SetArgs args = { };
  int zoneNumber = -1;
  int timerNumber = 0;

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL) {
      return -1;
    } else if (strcmp(key, "vn") == 0) {
      zoneNumber = atoi(val);
    } else if (strcmp(key, "tn") == 0) {
      timerNumber = atoi(val);

    } else if (strcmp(key, "d1") == 0) {
      args.on_duration = atoi(val);
    } else if (strcmp(key, "d0") == 0) {
      args.off_duration = atoi(val);
    } else if (strcmp(key, "r") == 0) {
      args.repeats = atoi(val);
    } else if (strcmp(key, "per") == 0) {
      args.period = atoi(val);
    } else if (strcmp(key, "di") == 0) {
      args.mDaysInterval = atoi(val);
    } else if (strcmp(key, "sb") == 0) {
      args.mTodSpanBegin = atoi(val);
    } else if (strcmp(key, "se") == 0) {
      args.mTodSpanEnd = atoi(val);
    } else if (strcmp(key, "ir") == 0) {
      args.mIgnoreRainSensor |= strcmp("true",val) == 0 ? 1 : 0;
    } else if (strcmp(key, "ip") == 0) {
      args.mIgnoreRainSensor |= strcmp("true",val) == 0 ? 2 : 0;
    } else {
      warning_unknown_option(key);
    }
  }

  if (zoneNumber >= 0 && rvt.set(args, zoneNumber, timerNumber)->scheduleRun()) {
    rvt.loop(); // XXX
  } else {
    // XXX: error
  }

  if constexpr (0) {
    char *buf = (char*) malloc(BUF_SIZE);
    if (!buf)
      return -1;
    *buf = '\0';

    esp32_write(JSON_PREFIX, JSON_PREFIX_LEN);

    if (*buf)
      esp32_write(buf, strlen(buf) - 1); // no terminating comma

    esp32_write(JSON_SUFFIX, JSON_SUFFIX_LEN);

    free(buf);
  }

  return 0;
}
