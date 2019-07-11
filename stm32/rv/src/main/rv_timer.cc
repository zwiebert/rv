#include "rv_timer.hh"

#include "water_pump.h"


int Lph[RV_VALVE_COUNT] = {
    1000, //0
    1000, //1
    1000, //2
    1000, //3
    1000, //4
    1000, //5
    1000, //6
    1000, //7
    1000, //8
    1000, //9
    1000, //10
    1000, //11
    };

uint16_t RvTimers::valve_bits;
uint16_t RvTimers::valve_mask;


RvTimerPause RvTimer::rvtp;

void RvTimer::changeState(state_T state) {
  state_T oldState = mState;
  if (oldState == state)
    return;


  if (oldState == STATE_ON) {
    switch_valve(false);
    rvtp.lphChange(-Lph[getValveNumber()]);
  }

  switch (state) {
  case STATE_ON:
    switch_valve(true);
    rvtp.lphChange(+Lph[getValveNumber()]);
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
}

void RvTimer::changeOnOff() {

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
  changeState(STATE_PAUSED);
}

void RvTimer::unpause() {
  changeState(STATE_RUN);
}

void RvTimer::stop() {

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

void *p;

void RvTimers::loop() {



  for (RvTimer *t = mRvTimers.mUsedTimers.getNext(); t; t = t->getNext()) {

#if 0
    char json[256];
    t->toJSON(json, sizeof json);
     RvTimerData test = RvTimerData(json);
     p = &test;
#endif

    if (t->isDone()) {
      RvTimer *pred = t->pred;
      mRvTimers.delete_timer(t);
      t = pred;
      continue;
    }

    RvTimer &vt = *t;

    switch (vt.checkState()) {

    case RvTimer::SCR_ON_OFF:
      if (vt.isOff()) { // ready to turn on
        if (RvTimer::rvtp.getLph() + Lph[vt.getValveNumber()] < RV_MAX_LPH) {
          vt.changeOnOff();
        }
      } else { // ready to turn off
        vt.changeOnOff();
      }
      break;

    case RvTimer::SCR_RAIN:
      vt.changeOnOff();
      break;

    case RvTimer::SCR_RUN:
      vt.run();
      break;

    case RvTimer::SCR_PAUSE:
      vt.pause();
      break;

    case RvTimer::SCR_UNPAUSE:
      vt.unpause();
      break;

    case RvTimer::SCR_NONE:
      break;
    }

  }

  if (valve_mask && mSvsCb) {
    mSvsCb(valve_bits, valve_mask);
    valve_bits = valve_mask = 0;
  }
}

