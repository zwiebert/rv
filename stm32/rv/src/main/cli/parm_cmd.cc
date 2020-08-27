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

const char help_parmCmd[] = "zone=[0-13]      zone number\n"
    "duration=[0-60]  how long to irrigate\n";

int
process_parmCmd(clpar p[], int len) {
  int arg_idx;



  bool wantsDurations = false, wantsRemainingTimes = false, wantsReply = false, hasDuration = false, wantsRelayPump = false, wantsRelayPC = false,
      wantsRainSensor = false, wantsTime = false, wantsTimers = false, wantsPumpRunTime = false, wantsVersion = false;

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL) {
      return -1;
    } else if (strcmp(key, "timer") == 0 && *val == '?') {
      wantsReply = wantsTimers = true;

    } else if (strcmp(key, KEY_DURATION_PREFIX) == 0 && *val == '?') {
      wantsReply = wantsDurations = true;

    } else if (strcmp(key, KEY_REMAINING_PREFIX) == 0 && *val == '?') {
      wantsReply = wantsRemainingTimes = true;

    } else if (strcmp(key, KEY_STATUS_PREFIX) == 0 && *val == '?') {
      wantsReply = wantsDurations = wantsRemainingTimes = wantsRelayPC = wantsRelayPump = wantsTime = wantsRainSensor = wantsPumpRunTime = true;

    } else if (strncmp(key, KEY_DURATION_PREFIX, KEY_DURATION_PREFIX_LEN) == 0) {
      RvTimer::SetArgs args;
      sscanf((key + KEY_DURATION_PREFIX_LEN), "%hhd.%hhd", &args.valve_number, &args.timer_number);
      if (strchr(val, ',')) {
        int irs = 0;
        sscanf(val, "%hhd,%d,%hhd,%hhd,%hd,%hhd,%hd,%hd", &args.on_duration, &irs, &args.off_duration, &args.repeats, &args.period, &args.mDaysInterval, &args.mTodSpanBegin,
            &args.mTodSpanEnd);
        args.ignoreRainSensor = !!(irs & 1);
        args.ignorePumpPause = !!(irs & 2);
      } else {
        args.on_duration = atoi(val);
      }
        if (rvt.set(args)->scheduleRun()) {
          hasDuration = true;
        } else {
          // XXX: error
        }

    } else if (strcmp(key, KEY_VERSION) == 0 && *val == '?') {
      wantsReply = wantsVersion = true;

    } else {
      warning_unknown_option(key);
    }
  }

  if (hasDuration) {
    rvt.loop(); //XXX
  }

  if (wantsReply) {
    char *buf = (char*) malloc(BUF_SIZE);
    if (!buf)
      return -1;
    *buf = '\0';

    esp32_write(JSON_PREFIX, JSON_PREFIX_LEN);

      for (const RvTimer &vt : *rvt.getTimerList()) {
      if (wantsDurations) {
        int secs = vt.get_duration();
        if (secs) {
          snprintf(buf + strlen(buf), BUF_SIZE - strlen(buf), "\"%s%d.%d\":%d,", KEY_DURATION_PREFIX, vt.getValveNumber(), vt.getTimerNumber(), secs);
        }
      }

      if (wantsRemainingTimes) {
        int secs = vt.get_remaining();
        if (secs) {
          snprintf(buf + strlen(buf), BUF_SIZE - strlen(buf), "\"%s%d.%d\":%d,", KEY_REMAINING_PREFIX, vt.getValveNumber(), vt.getTimerNumber(), secs);
        }
      }
    }

    if (wantsRelayPC) {
      strcat(buf, wp_isPressControlOn(0) ? "\"pc\":1," : "\"pc\":0,");
    }

    if (wantsRelayPump) {
      strcat(buf,  wp_isPumpOn() ? "\"pump\":1," : "\"pump\":0,");
    }

    if (wantsPumpRunTime) {
      if (wp_isPumpOn()) {
        snprintf(buf + strlen(buf), BUF_SIZE - strlen(buf), "\"p1d\":%lu,", wp_getPumpOnDuration());
      } else {
        snprintf(buf + strlen(buf), BUF_SIZE - strlen(buf), "\"p0d\":%lu,", wp_getPumpOffDuration());
      }
    }

    if (wantsRainSensor) {
      strcat(buf, rs.getState() ? "\"rain\":1," :  "\"rain\":0,");
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

    if (wantsTimers)
      for (const RvTimer &vt : *rvt.getTimerList()) {
        char *json = vt.argsToJSON(buf + strlen(buf), BUF_SIZE - strlen(buf));
        esp32_write(JSON_PREFIX, JSON_PREFIX_LEN);
        esp32_puts(json);
        esp32_write(JSON_SUFFIX, JSON_SUFFIX_LEN);
      }

    free(buf);
  }


  return 0;
}

void timers_was_modified(int vn, int tn, bool removed) {
  char buf[128];

  if (removed) {
    snprintf(buf, sizeof buf, "\"timer%d.%d\":{}", vn, tn);
    esp32_write(JSON_PREFIX, JSON_PREFIX_LEN);
    esp32_puts(buf);
    esp32_write(JSON_SUFFIX, JSON_SUFFIX_LEN);
    return;
  }

  for (const RvTimer &vt : *rvt.getTimerList()) {
    if (!vt.match(vn, tn))
      continue;
    char *json = vt.argsToJSON(buf + strlen(buf), BUF_SIZE - strlen(buf));
    esp32_write(JSON_PREFIX, JSON_PREFIX_LEN);
    esp32_puts(json);
    esp32_write(JSON_SUFFIX, JSON_SUFFIX_LEN);
    return;
  }

}
