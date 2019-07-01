#include "rv_timer.hh"

template <class T>
  void List<T>::append(T *timer) {
    // unlink from current list
    timer->unlinkThis();

    // append to list
    RvTimer *tail = this->pred;
    tail->succ = timer;
    timer->pred = tail;

    this->pred = timer; // timer is new list tail
    timer->succ = (T*)this;
  }


int Lph[RV_VALVE_COUNT] = { 1000, //
    1000, //
    1000, //
    1000, //
    1000, //
    1000, //
    1000, //
    1000, //
    1000, //
    1000, //
    1000, //
    1000, //
    };

uint16_t RvTimers::valve_bits;
uint16_t RvTimers::valve_mask;





void RvTimer::changeOnOff() {
    if (mIsOn) {
      ++mDoneOn;
      if (mDoneOn >= mArgs.repeats) {
        mNextOnOff = 0;
        stop();
        return;
      }
    }

    mIsOn = !mIsOn;
    switch_valve(mIsOn);
    if (mIsOn && mArgs.on_duration) {
      mNextOnOff = time(0) + mArgs.on_duration;
    } else if (!mIsOn && mArgs.off_duration) {
      mNextOnOff = time(0) + mArgs.off_duration;
    }
  }

void RvTimer::stop() {
  mIsRunning = false;
  time_t now = time(0);
  mNextOnOff = 0;
  if (mIsOn) {
    switch_valve(false);
    mIsOn = false;
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

}

void *p;

void RvTimers::loop() {

  valve_bits = valve_mask = 0;

  for (RvTimer *t = mRvTimers.mUsedTimers.getNext(); t; t = t->getNext()) {

#if 0
    char json[256];
    t->toJSON(json, sizeof json);
     RvTimerData test = RvTimerData(json);
     p = &test;
#endif

    if (t->isDone()) {
      RvTimer *pred = t->pred;
      mRvTimers.mFreeTimers.append(t);
      t = pred;
      continue;
    }

    RvTimer &vt = *t;

    if (vt.checkState()) {
      if (vt.isReadyToOnOff()) {
        if (vt.isOff()) { // ready to turn on
          if (lph + Lph[vt.getValveNumber()] < RV_MAX_LPH) {
            lph += Lph[vt.getValveNumber()];
            vt.changeOnOff();
          }
        } else { // ready to turn off
          vt.changeOnOff();
          lph -= Lph[vt.getValveNumber()];
        }
      } else if (vt.isReadyToRun()) {
        vt.run();
      } else if (vt.shouldStopBecauseRain()) {
        vt.changeOnOff();
        lph -= Lph[vt.getValveNumber()];
      }
    }
  }

  if (valve_mask && mSvsCb) {
    mSvsCb(valve_bits, valve_mask);
  }
}

