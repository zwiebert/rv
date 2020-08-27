#include <rv/rv_timer_pause.hh>
#include <rv/rv_zones.hh>




  //unsigned RvTimerPause::getLph()  { return mLph; }
void RvTimerPause::lphUpdate(int lph) {
  if (!lph)
    return;

  run_time_T now = runTime();
  run_time_T dur = now - mLastLphUpdate;

  if (mLph) {
    mLitersBeforePause += (int) (((double) mLph / 3600.0) * (double) dur);
  }

  mLph += lph;
  mLastLphUpdate = now;
}

bool RvTimerPause::needsPause(int zone) {
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

