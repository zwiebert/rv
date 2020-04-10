#include "user_config.h"
#include <real_time_clock.h>
#include <string.h>
#include "cli_imp.h"
#include <stdio.h>
#include "peri/uart.h"
#include "water_pump.h"
#include "rv_timer.hh"
#include "app_cxx.hh"
#include "rain_sensor.hh"

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

#define JSON_PREFIX "\"data\":{"
#define JSON_PREFIX_LEN ((sizeof JSON_PREFIX) - 1)
#define JSON_SUFFIX "}\n"
#define JSON_SUFFIX_LEN ((sizeof JSON_SUFFIX) - 1)

#define KEY_VERSION "version"

#define BUF_SIZE 2048

const char help_parmCmd[] = "zone=[0-13]      zone number\n"
    "duration=[0-60]  how long to irrigate\n";

int ICACHE_FLASH_ATTR
process_parmCmd(clpar p[], int len) {
  int arg_idx;

  char *buf = (char*) malloc(BUF_SIZE);
  if (!buf)
    return -1;
  *buf = '\0';

  bool wantsDurations = false, wantsRemainingTimes = false, wantsReply = false, hasDuration = false, wantsRelayPump = false, wantsRelayPC = false,
      wantsRainSensor = false, wantsTime = false, wantsTimers = false, wantsPumpRunTime = false, wantsVersion = false;

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL) {
      return -1;
    } else if (strcmp(key, KEY_DURATION_PREFIX) == 0 && *val == '?') {
      wantsReply = wantsDurations = true;

    } else if (strcmp(key, KEY_REMAINING_PREFIX) == 0 && *val == '?') {
      wantsReply = wantsRemainingTimes = true;

    } else if (strcmp(key, KEY_STATUS_PREFIX) == 0 && *val == '?') {
      wantsReply = wantsDurations = wantsRemainingTimes = wantsRelayPC = wantsRelayPump = wantsTime = wantsRainSensor = wantsPumpRunTime = true;

    } else if (strncmp(key, KEY_DURATION_PREFIX, KEY_DURATION_PREFIX_LEN) == 0) {
      int channel = -1, timer_number = 0;
      sscanf((key + KEY_DURATION_PREFIX_LEN), "%d.%d", &channel, &timer_number);
      if (strchr(val, ',')) {
        RvTimer::SetArgs args;
        sscanf(val, "%d,%d,%d,%d,%d,%d,%d,%d", &args.on_duration, &args.mIgnoreRainSensor, &args.off_duration, &args.repeats, &args.period, &args.mDaysInterval, &args.mTodSpanBegin,
            &args.mTodSpanEnd);
        rvt.set(args, channel, timer_number)->scheduleRun();
        hasDuration = true;
      } else {
        int duration = atoi(val);
        rvt.set(channel, duration, timer_number)->scheduleRun();
        hasDuration = true;
      }

    } else if (strcmp(key, KEY_VERSION) == 0 && *val == '?') {
      wantsReply = wantsVersion = true;

    } else {
      warning_unknown_option(key);
    }
  }

  if (hasDuration) {
    rvt.loop(); //XXX
    esp32_write("dr\n", 3);
  }

  if (wantsReply) {
    esp32_write(JSON_PREFIX, JSON_PREFIX_LEN);

    for (RvTimer *t = rvt.getTimerList()->getNext(); t; t = t->getNext()) {
      if (wantsDurations) {
        int secs = t->get_duration();
        if (secs) {
          snprintf(buf + strlen(buf), BUF_SIZE - strlen(buf), "\"%s%d.%d\":%d,", KEY_DURATION_PREFIX, t->getValveNumber(), t->getTimerNumber(), secs);
        }
      }

      if (wantsRemainingTimes) {
        int secs = t->get_remaining();
        if (secs) {
          snprintf(buf + strlen(buf), BUF_SIZE - strlen(buf), "\"%s%d.%d\":%d,", KEY_REMAINING_PREFIX, t->getValveNumber(), t->getTimerNumber(), secs);
        }
      }

      if (wantsTimers) {
        t->argsToJSON(buf + strlen(buf), BUF_SIZE - strlen(buf));
        strcat(buf, ",");
      }

    }

    if (wantsRelayPC && wp_isPressControlOn(0)) {
      strcat(buf, "\"pc\":1,");
    }

    if (wantsRelayPump && wp_isPumpOn()) {
      strcat(buf, "\"pump\":1,");
    }

    if (wantsPumpRunTime) {
      if (wp_isPumpOn()) {
        snprintf(buf + strlen(buf), BUF_SIZE - strlen(buf), "\"p1d\":%lu,", wp_getPumpOnDuration());
      } else {
        snprintf(buf + strlen(buf), BUF_SIZE - strlen(buf), "\"p0d\":%lu,", wp_getPumpOffDuration());
      }
    }

    if (wantsRainSensor && rs.getState()) {
      strcat(buf, "\"rain\":1,");
    }

    if (wantsTime) {
      time_t timer = time(NULL);
      struct tm t;
      localtime_r(&timer, &t);
      strftime(buf + strlen(buf), BUF_SIZE - strlen(buf), "\"time\":\"%FT%H:%M:%S\",", &t);
    }

    if (wantsVersion) {
      snprintf(buf + strlen(buf), BUF_SIZE - strlen(buf), "\"version\":\"%s\",", VERSION);
    }

    if (*buf)
      esp32_write(buf, strlen(buf) - 1); // no terminating comma

    esp32_write(JSON_SUFFIX, JSON_SUFFIX_LEN);
    *buf = '\0';
  }

  free(buf);
  return 0;
}
