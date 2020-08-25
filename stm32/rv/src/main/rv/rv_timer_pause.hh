#pragma once

#include "user_config.h"
#include "rv.hh"

#include "rv_zones.hh"
#include "rain_sensor.hh"
#include "setup/app_cxx.hh"

#include "misc/int_macros.h"
#include "time/real_time_clock.h"
#include "water_pump.h"


#include <time.h>
#include <stdio.h>
#include <string.h>


class RvTimerPause {


  int mLitersBeforePause = 0;
  unsigned mLph = 0;
  run_time_T mLastLphUpdate = 0;

  unsigned pauseDuration() { return mLitersBeforePause * PAUSE_SECS_PER_LITER; }
public:
  unsigned getLph()  { return mLph; }
  void lphUpdate(int lph) {
    if (!lph)
      return;


    run_time_T now = runTime();
    run_time_T dur = now - mLastLphUpdate;

    if (mLph) {
      mLitersBeforePause += (int)(((double)mLph / 3600.0) * (double)dur);
    }

    mLph += lph;
    mLastLphUpdate = now;
  }

  bool needsPause(int zone = -1) {
    if (mLitersBeforePause == 0)
      return false;
    if (zone >= 0 && rvz[zone].getLph() < PAUSE_LPH_THRESHOLD)
      return false;

    run_time_T dur = pauseDuration();

    run_time_T sinceLastLphUpdate = (runTime() - mLastLphUpdate);
    if (dur > sinceLastLphUpdate)
      return true;

#if 0
    run_time_T sinceLastPumpOff = wp_getPumpOffDuration();
    if (dur > sinceLastPumpOff)
      return true;
#endif

    mLitersBeforePause = 0;
    return false;
  }
};
