#pragma once

#include <stddef.h>

typedef struct {
  char *first, *second;
} cstr_pair;

struct RvTimerData {
public:
  struct SetArgs {
    int8_t valve_number = -1, timer_number = 0;
    int16_t on_duration = 0, off_duration = 0, mTodSpanBegin = 0, mTodSpanEnd = 0, period = 0;


    int8_t  repeats = 0, mDaysInterval = 0;

    bool ignoreRainSensor :1 = false;
    bool ignorePumpPause :1 = false;

    char* toJSON(char *buf, size_t buf_size, bool no_brackets = false) const;
    SetArgs() {
    }
    SetArgs(cstr_pair kvs[], int len);

  };
protected:
  SetArgs mArgs;
  time_t mNextRun = 0;
  time_t mLastRun = 0;

public:

  char* toJSON(char *buf, size_t buf_size, bool no_brackets = false) const;
  RvTimerData() {
  }
};

static_assert(sizeof (RvTimerData::SetArgs) <= 16);
