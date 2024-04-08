#include "rv_timer.hh"
#include "rv.hh"
#include "rv_zones.hh"
#include "water_pump/water_pump.h"
#include "report.h"
#include "debug/dbg.h"
#include <algorithm>
#include <debug/log.h>

#ifdef CONFIG_RV_DEBUG
#define DEBUG
#define D(x) x
#define L(x) x
#else
#define D(x)
#define L(x) x
#endif
#define logtag "rv"


RvTimerPause RvTimer::rvtp;

void RvTimer::changeState(state_T state) {
  D(db_logi(logtag, "%s(%d) mState=%d", __func__, state, mState));
  state_T oldState = mState;
  if (oldState == state)
    return;


  if (oldState == STATE_ON) {
    switch_valve(false);
    rvtp.lphUpdate(-rvz[getValveNumber()].getLph());
  }

  switch (state) {
  case STATE_ON:
    switch_valve(true);
    rvtp.lphUpdate(+rvz[getValveNumber()].getLph());
    break;
  case STATE_PAUSED:
    break;

  case STATE_OFF:
    break;
  case STATE_RUN:
    break;
  case STATE_DONE:
    break;
  }

  mState = state;
  report_state_change(this->mArgs.valve_number, state);
}

void RvTimer::changeOnOff() {
  D(db_logi(logtag, "%s()", __func__));

  if (mState == STATE_ON) {

    if (++mDoneOn >= mArgs.repeats) {
      mNextOnOff = 0;
      stop();
      return;
    } else {
      mNextOnOff = time(0) + mArgs.off_duration;
      changeState(STATE_RUN);
    }

  } else {
    mNextOnOff = time(0) + mArgs.on_duration;
    changeState(STATE_ON);
  }
}

void RvTimer::pause() {
  D(db_logi(logtag, "%s()", __func__));
  changeState(STATE_PAUSED);
}

void RvTimer::unpause() {
  D(db_logi(logtag, "%s()", __func__));
  precond(mState == STATE_PAUSED);
  changeState(STATE_RUN);
}

void RvTimer::stop() {
  D(db_logi(logtag, "%s()", __func__));

  time_t now = time(0);
  mNextOnOff = 0;
  if (mState == STATE_ON) {
    mLastRun = now;
  }

  mNextRun = 0;

  time_t nextPeriod = mArgs.period ? now + mArgs.period : 0;
  if (nextPeriod)
    mNextRun = nextPeriod;

  if (mArgs.mDaysInterval) {
    struct tm *tm = localtime(&now);
    tm->tm_hour = mArgs.mTodSpanBegin / ONE_HOUR;
    tm->tm_min = (mArgs.mTodSpanBegin % ONE_HOUR) / ONE_MINUTE;
    tm->tm_sec = (mArgs.mTodSpanBegin % ONE_MINUTE);
    time_t beginToday = mktime(tm);

    time_t nextDay = mArgs.mDaysInterval ? beginToday + mArgs.mDaysInterval * ONE_DAY : 0;

    if (nextDay && (!mNextRun || nextDay < mNextRun))
      mNextRun = nextDay;
  }

  changeState(mNextRun ? STATE_OFF : STATE_DONE);
}

