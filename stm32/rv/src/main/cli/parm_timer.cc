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
  RvTimer::SetArgs args = RvTimer::SetArgs(reinterpret_cast<cstr_pair *>(&p[1]), len - 1);


#if 0
  for (int arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    break;  // nothing to do here

    if (key == NULL) {
      return -1;
    }
  }
#endif

  if (args.valve_number >= 0 && rvt.set(args)->scheduleRun()) {
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
