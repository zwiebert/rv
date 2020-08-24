#pragma once

#include "user_config.h"
#include "rain_sensor.hh"
#include "setup/app_cxx.hh"

#include "misc/int_macros.h"
#include "time/real_time_clock.h"
#include "water_pump.h"

#include "rv_timer_pause.hh"

#include <time.h>
#include <stdio.h>
#include <string.h>

struct RvTimerData {
public:
  struct SetArgs {
    int on_duration = 0, off_duration = 0, repeats = 0, period = 0;
    int mDaysInterval = 0, mTodSpanBegin = 0, mTodSpanEnd = 0;
    int mIgnoreRainSensor = 0;
#define SA_JSON_FMT "{\"d1\":%d,\"ir\":%d,\"d0\":%d,\"r\":%d,\"per\":%d,\"di\":%d,\"sb\":%d,\"se\":%d}"
    char *toJSON(char *buf, int buf_size) {
      if (0
          <= snprintf(buf, buf_size, SA_JSON_FMT, on_duration, mIgnoreRainSensor,
              off_duration, repeats, period, mDaysInterval, mTodSpanBegin, mTodSpanEnd))
        return buf;

      return 0;
    }
    SetArgs() {}
    SetArgs(const char *json) {
      json = strstr(json, "{\"d1\":");
      if (json) {
        sscanf(json, SA_JSON_FMT, &on_duration, &mIgnoreRainSensor, &off_duration, &repeats, &period, &mDaysInterval, &mTodSpanBegin, &mTodSpanEnd);
      }
    }
  };
protected:
  SetArgs mArgs;
  int mValveNumber = -1;
  int mTimerNumber = 0; // multiple time per valve with different numbers
  time_t mNextRun = 0;
  time_t mLastRun = 0;

public:
#define TD_JSON_PF_FMT "{\"vn\":%d,\"tn\":%d,\"nr\":%ld,\"lr\":%ld,\"args\":%s}"
#define TD_JSON_SF_FMT "{\"vn\":%d,\"tn\":%d,\"nr\":%ld,\"lr\":%ld,"
  char *toJSON(char *buf, int buf_size) {
    char aBuf[128] = "";
    const char *aJson = mArgs.toJSON(aBuf, sizeof aBuf);

    if (0
        <= snprintf(buf, buf_size, TD_JSON_PF_FMT, mValveNumber, mTimerNumber, (uint32_t)mNextRun, (uint32_t)mLastRun, aJson))
      return buf;

    return 0;
  }

  RvTimerData() {}
  RvTimerData(const char *json): mArgs(json) {
    uint32_t nextRun = 0, lastRun = 0; // long long currently not working with printf/scanf
    sscanf(json, TD_JSON_SF_FMT, &mValveNumber, &mTimerNumber, &nextRun, &lastRun);
    mNextRun = nextRun;
    mLastRun = lastRun;
  }
};
