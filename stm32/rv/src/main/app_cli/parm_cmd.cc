/**
 * \file cli/parm_cmd.cc
 * \brief handle cli command
 */
#include "user_config.h"
#include <time/real_time_clock.h>
#include "cli_imp.h"
#include "water_pump/water_pump.h"
#include "rv/rv_timers.hh"
#include "rv/rain_sensor.hh"
#include "debug/dbg.h"

#include <cstdio>
#include <stdlib.h>
#include <cstring>
#include <errno.h>
#include <peri/uart.h>

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

int process_parmCmd(clpar p[], int len, class UoutWriter &td) {
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
    char buf[BUF_SIZE] = "";
    auto sj = td.sj();

    sj.open_root_object();

    sj.add_object("data");

    for (const RvTimer &vt : *rvt.getTimerList()) {
      if (wantsDurations) {
        int secs = vt.get_duration();
        if (secs) {
          char key[16];
          std::snprintf(key, sizeof key, "%s%d.%d", KEY_DURATION_PREFIX, vt.getValveNumber(), vt.getTimerNumber());
          sj.put_kv(key, secs);
        }
      }

      if (wantsRemainingTimes) {
        int secs = vt.get_remaining();
        if (secs) {
          char key[16];
          std::snprintf(key, sizeof key, "%s%d.%d", KEY_REMAINING_PREFIX, vt.getValveNumber(), vt.getTimerNumber());
          sj.put_kv(key, secs);
        }
      }
    }

    if (wantsRelayPC) {
          sj.put_kv("pc",  wp_isPressControlOn(0));
    }

    if (wantsRelayPump) {
          sj.put_kv("pc",  wp_isPumpOn());
    }

    if (wantsPumpRunTime) {
      if (wp_isPumpOn()) {
        sj.put_kv("p1d", wp_getPumpOnDuration());
      } else {
        sj.put_kv("p0d", wp_getPumpOffDuration());
      }
    }

    if (wantsRainSensor) {
      sj.put_kv("rain", rs.getState());
    }

    if (wantsTime) {
      char ft[32];
      time_t timer = time(NULL);
      struct tm t;
      localtime_r(&timer, &t);
      strftime(ft, sizeof ft, "%FT%H:%M:%S", &t);
      sj.put_kv("time", ft);
    }

    if (wantsVersion) {
      sj.put_kv("version", VERSION);
    }

    sj.close_object();
    sj.close_root_object();
    sj.writeln_json(false);

    if (wantsTimers)
      buf[0] = '\0';
      for (const RvTimer &vt : *rvt.getTimerList()) {
        sj.open_root_object();
        sj.add_key("data");
        vt.to_json(sj);
        sj.close_root_object();
        sj.writeln_json(false);
      }
  }

  if (errors) {
    db_printf("error:errno=%d\n", errno);
    fputs("error: error in processing cli cmd\n", stderr);
  }
  return 0;
}

void timers_was_modified(int vn, int tn, bool removed) {
  char buf[128];
  UoutBuilderJson sj(buf, sizeof buf);

  if (removed) {
    sj.open_root_object();
    sj.add_object("timer");
    sj.put_kv("vn", vn);
    sj.put_kv("tn", tn);
    sj.close_object();
    sj.close_root_object();
    fputs(sj.get_json(), stdout);
    return;
  }

  *buf = '\0';
  for (const RvTimer &vt : *rvt.getTimerList()) {
    if (!vt.match(vn, tn))
      continue;
    sj.open_root_object();
    sj.add_key("update");
    vt.to_json(sj);
    sj.close_root_object();
    fputs(sj.get_json(), stdout);
    return;
  }
}
