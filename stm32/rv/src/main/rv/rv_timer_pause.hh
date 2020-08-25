#pragma once

#include <rv/rv.hh>
#include <time/real_time_clock.h>


class RvTimerPause {


  int mLitersBeforePause = 0;
  int mLph = 0;
  run_time_T mLastLphUpdate = 0;

  unsigned pauseDuration() { return mLitersBeforePause * PAUSE_SECS_PER_LITER; }

public:

  int getLph() const  { return mLph; }
  void lphUpdate(int lph);
  bool needsPause(int zone = -1);
};
