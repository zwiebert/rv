#include "user_config.h"
#include <time/real_time_clock.h>
#include "cli_imp.h"
#include "peri/uart.h"
#include "water_pump/water_pump.h"
#include "rv/rv_timers.hh"
#include "rv/rain_sensor.hh"
#include "debug/dbg.h"

#include <cstdio>
#include <stdlib.h>
#include <cstring>

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
#define JSON_PREFIX_UPD "{\"update\":{"
#define JSON_PREFIX_UPD_LEN ((sizeof JSON_PREFIX_UPD) - 1)
#define JSON_SUFFIX "}}\n"
#define JSON_SUFFIX_LEN ((sizeof JSON_SUFFIX) - 1)

#define KEY_VERSION "version"

#define BUF_SIZE 2048

const char help_parmCmd[] = "zone=[0-13]      zone number\n"
    "duration=[0-60]  how long to irrigate\n";

int process_parmCmd(clpar p[], int len) {
  int arg_idx;
  int errors = 0;
  int res = 0;

  bool wantsDurations = false, wantsRemainingTimes = false, wantsReply = false, hasDuration = false, wantsRelayPump = false, wantsRelayPC = false,
      wantsRainSensor = false, wantsTime = false, wantsTimers = false, wantsPumpRunTime = false, wantsVersion = false;

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL) {
      return -1;
    } else if (std::strcmp(key, "timer") == 0 && *val == '?') {
      wantsReply = wantsTimers = true;

    } else if (std::strcmp(key, KEY_DURATION_PREFIX) == 0 && *val == '?') {
      wantsReply = wantsDurations = true;

    } else if (std::strcmp(key, KEY_REMAINING_PREFIX) == 0 && *val == '?') {
      wantsReply = wantsRemainingTimes = true;

    } else if (std::strcmp(key, KEY_STATUS_PREFIX) == 0 && *val == '?') {
      wantsReply = wantsDurations = wantsRemainingTimes = wantsRelayPC = wantsRelayPump = wantsTime = wantsRainSensor = wantsPumpRunTime = true;

    } else if (std::strncmp(key, KEY_DURATION_PREFIX, KEY_DURATION_PREFIX_LEN) == 0) {
      RvTimer::SetArgs args;
      int vn = -1, tn = 0;  // XXX: read int because %hhd sometimes fails (why?)
      if ((res = sscanf(key, KEY_DURATION_PREFIX "%d.%d", &vn, &tn)) >= 1) {
        args.valve_number = vn;
        args.timer_number = tn;
        //if ((res = sscanf(key, KEY_DURATION_PREFIX "%hhd.%hhd", &args.valve_number, &args.timer_number)) >= 1) {
        if (std::strchr(val, ',')) {
          int irs = 0;
          // XXX: this may also fail for %hhd like above?
          if ((res = sscanf(val, "%hd,%d,%hd,%hhd,%hd,%hhd,%hd,%hd", &args.on_duration, &irs, &args.off_duration, &args.repeats, &args.period,
              &args.mDaysInterval, &args.mTodSpanBegin, &args.mTodSpanEnd)) >= 2) {
            args.ignoreRainSensor = !!(irs & 1);
            args.ignorePumpPause = !!(irs & 2);
          } else {
            ++errors;
            if (errno)
              perror("error:parm_cmd:sscanf(val)");
          }
        } else {
          args.on_duration = atoi(val);
        }

        if (RvTimer *timer = rvt.set(args)) {
          if (timer->scheduleRun()) {
            hasDuration = true;
          }
        }
      } else {
        if (errno)
          perror("error:parm_cmd:sscanf(key)");
        ++errors;
      }
    } else if (std::strcmp(key, KEY_VERSION) == 0 && *val == '?') {
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
          std::snprintf(buf + std::strlen(buf), BUF_SIZE - std::strlen(buf), "\"%s%d.%d\":%d,", KEY_DURATION_PREFIX, vt.getValveNumber(), vt.getTimerNumber(),
              secs);
        }
      }

      if (wantsRemainingTimes) {
        int secs = vt.get_remaining();
        if (secs) {
          std::snprintf(buf + std::strlen(buf), BUF_SIZE - std::strlen(buf), "\"%s%d.%d\":%d,", KEY_REMAINING_PREFIX, vt.getValveNumber(), vt.getTimerNumber(),
              secs);
        }
      }
    }

    if (wantsRelayPC) {
      std::strcat(buf, wp_isPressControlOn(0) ? "\"pc\":1," : "\"pc\":0,");
    }

    if (wantsRelayPump) {
      std::strcat(buf, wp_isPumpOn() ? "\"pump\":1," : "\"pump\":0,");
    }

    if (wantsPumpRunTime) {
      if (wp_isPumpOn()) {
        std::snprintf(buf + std::strlen(buf), BUF_SIZE - std::strlen(buf), "\"p1d\":%lu,", wp_getPumpOnDuration());
      } else {
        std::snprintf(buf + std::strlen(buf), BUF_SIZE - std::strlen(buf), "\"p0d\":%lu,", wp_getPumpOffDuration());
      }
    }

    if (wantsRainSensor) {
      std::strcat(buf, rs.getState() ? "\"rain\":1," : "\"rain\":0,");
    }

    if (wantsTime) {
      time_t timer = time(NULL);
      struct tm t;
      localtime_r(&timer, &t);
      strftime(buf + std::strlen(buf), BUF_SIZE - std::strlen(buf), "\"time\":\"%FT%H:%M:%S\",", &t);
    }

    if (wantsVersion) {
      std::snprintf(buf + std::strlen(buf), BUF_SIZE - std::strlen(buf), "\"version\":\"%s\",", VERSION);
    }

    if (*buf)
      esp32_write(buf, std::strlen(buf) - 1); // no terminating comma

    esp32_write(JSON_SUFFIX, JSON_SUFFIX_LEN);

    if (wantsTimers)
      for (const RvTimer &vt : *rvt.getTimerList()) {
        char *json = vt.argsToJSON(buf + std::strlen(buf), BUF_SIZE - std::strlen(buf));
        esp32_write(JSON_PREFIX, JSON_PREFIX_LEN);
        esp32_puts(json);
        esp32_write(JSON_SUFFIX, JSON_SUFFIX_LEN);
      }

    free(buf);
  }

  if (errors) {
    db_printf("error:errno=%d\n", errno);
    esp32_puts("error: error in processing cli cmd\n");
  }
  return 0;
}

void timers_was_modified(int vn, int tn, bool removed) {
  char buf[128];

  if (removed) {
    std::snprintf(buf, sizeof buf, "\"timer\":{\"vn\":%d,\"tn\":%d}", vn, tn);
    esp32_write(JSON_PREFIX_UPD, JSON_PREFIX_UPD_LEN);
    esp32_puts(buf);
    esp32_write(JSON_SUFFIX, JSON_SUFFIX_LEN);
    return;
  }

  for (const RvTimer &vt : *rvt.getTimerList()) {
    if (!vt.match(vn, tn))
      continue;
    char *json = vt.argsToJSON(buf + std::strlen(buf), BUF_SIZE - std::strlen(buf));
    esp32_write(JSON_PREFIX_UPD, JSON_PREFIX_UPD_LEN);
    esp32_puts(json);
    esp32_write(JSON_SUFFIX, JSON_SUFFIX_LEN);
    return;
  }

}
