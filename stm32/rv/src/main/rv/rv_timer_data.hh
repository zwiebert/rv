#pragma once

#include "user_config.h"
#include "rv.hh"
#include "rain_sensor.hh"
#include "setup/app_cxx.hh"

#include "misc/int_macros.h"
#include "time/real_time_clock.h"
#include "water_pump.h"

#include "rv_timer_pause.hh"

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <utility>

//typedef std::pair<const char*, const char*> cstr_pair;
typedef struct { char *first, *second; } cstr_pair;

struct RvTimerData {
public:
  struct SetArgs {
    int valve_number = -1, timer_number = 0;
    int on_duration = 0, off_duration = 0, repeats = 0, period = 0;
    int mDaysInterval = 0, mTodSpanBegin = 0, mTodSpanEnd = 0;
    int mIgnoreRainSensor = 0;

    char *toJSON(char *buf, size_t buf_size, bool no_brackets = false) const;
    SetArgs() {}
    SetArgs(cstr_pair kvs[], int len);

  };
protected:
  SetArgs mArgs;
  time_t mNextRun = 0;
  time_t mLastRun = 0;

public:

  char *toJSON(char *buf, size_t buf_size, bool no_brackets = false) const;
  RvTimerData() {}
};
