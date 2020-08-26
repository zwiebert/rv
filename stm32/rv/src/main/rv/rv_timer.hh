#pragma once

#include <rv/rain_sensor.hh>
#include <rv/rv.hh>
#include <rv/rv_timer_data.hh>
#include <rv/rv_timer_pause.hh>
#include <sys/_timeval.h>
#include <time/real_time_clock.h>
#include <time.h>

#include <cstdio>
#include <cstring>


typedef void (*switch_valve_cb)(int valve_number, bool state);




#define IGNORE_RAIN_SENSOR 1
#define IGNORE_PUMP_PAUSE 2



class RvTimer: public RvTimerData  {
  friend class RvTimers;
  static RvTimerPause rvtp;
private:
  time_t mNextOnOff = 0;
  //bool mIsRunning = false, mIsOn = false;
  int mDoneOn = 0;
  switch_valve_cb mSwitchValveCb = 0;
public:
  typedef enum { SCR_NONE, SCR_RUN, SCR_ON_OFF, SCR_RAIN, SCR_PAUSE, SCR_UNPAUSE } sc_req_T;
  typedef enum { STATE_OFF, STATE_RUN, STATE_PAUSED, STATE_ON, STATE_DONE } state_T;
  state_T mState = STATE_OFF;

private:
  void changeState(state_T state);

  bool isRunOnce() const {
    return mArgs.on_duration && !mArgs.off_duration && !mArgs.repeats && !mArgs.period;
  }
  bool hasTodSpan() const {
    return mArgs.mTodSpanBegin != mArgs.mTodSpanEnd;
  }


  bool isDisabledByInterval(time_t now = time(0)) const {

    if (mLastRun && mArgs.mDaysInterval && mLastRun + mArgs.mDaysInterval * ONE_DAY < now)
      return true;

    return false;
  }

  bool isDisabledByRain() const {
    if (mArgs.mIgnoreRainSensor & IGNORE_RAIN_SENSOR)
      return false;
    if (rs.getState())
      return true; // raining now or was raining (adjust sensor directly)

    return false;
  }

  bool isDisabledByTimeOfDay(time_t now = time(0)) const {
    if (!hasTodSpan())
      return false;

    struct tm *tm_now = localtime(&now);
    int dsecs = tm_now->tm_hour * ONE_HOUR + tm_now->tm_min * ONE_MINUTE + tm_now->tm_sec;
    if (!(mArgs.mTodSpanBegin <= dsecs && (dsecs <= mArgs.mTodSpanEnd || mArgs.mTodSpanEnd == 0)))
      return true;

    return false;
  }




public:
  RvTimer() {
  }

  char *argsToJSON(char *buf, int buf_size) const {
    char *result = buf;
    if (0 <= snprintf(buf + strlen(buf), buf_size - strlen(buf), "\"timer%d.%d\":", getValveNumber(), getTimerNumber())) {
      mArgs.toJSON(buf + strlen(buf), buf_size - strlen(buf));
      return result;
    }
    return 0;
  }

  char *dataToJSON(char *buf, int buf_size) const {
    if (0 <= snprintf(buf + strlen(buf), buf_size - strlen(buf), "\"timer%d.%d\":", getValveNumber(), getTimerNumber())) {
      return toJSON(buf + strlen(buf), buf_size - strlen(buf));
    }
    return 0;
  }

  int getValveNumber() const {
    return mArgs.valve_number;
  }
  int getTimerNumber() const {
    return mArgs.timer_number;
  }
  bool match(int valve_number, int id = 0) const {
    return valve_number == mArgs.valve_number && id == mArgs.timer_number;
  }

  void register_callback(switch_valve_cb cb, int valve_number) {
    mSwitchValveCb = cb;
    mArgs.valve_number = valve_number;
  }

  void switch_valve(bool state) {
    if (mSwitchValveCb) {
      mSwitchValveCb(mArgs.valve_number, state);
    }
  }

  void run() {
    if (mState == STATE_DONE)
      return;
    mNextOnOff = time(0);
    changeState(STATE_RUN);
    mDoneOn = 0;
  }

  RvTimer *scheduleRun() {
    if (mState == STATE_DONE)
      return this;

    mNextRun = time(0);
    return this;
  }

  void stop();
  void pause();
  void unpause();


  int get_duration() const {
    return mArgs.on_duration;
  }
  int get_remaining() const {
    return isOn() ? mNextOnOff - time(0) : mArgs.on_duration;
  }

  void set(SetArgs &args) {
    mArgs = args;
    if (rs.getState() && !(args.period || args.mDaysInterval || args.mIgnoreRainSensor & IGNORE_RAIN_SENSOR)) {
      mArgs.on_duration = 0; // force timer to stop
    }
  }

  void set(int on_duration, int id) {
    SetArgs args;
    args.on_duration = on_duration;
    args.timer_number = id;
    set(args);
  }

  bool isReadyToRun() const {
    if (mState != STATE_OFF)
      return false;

    time_t now = time(0);


    if (mArgs.on_duration == 0)
      return true; // run this 0-timer, so it can be stopped and removed from list

    if (!(mNextRun && mNextRun < now))
      return false;
    if (isDisabledByRain())
      return false;
    if (isDisabledByInterval())
      return false;
    if (isDisabledByTimeOfDay())
      return false;

    return true;
   // return !mIsRunning && mNextRun && mNextRun < now && !isDisabledByRain() && !isDisabledByInterval(now) && !isDisabledByTimeOfDay(now);
  }

  bool isReadyToOnOff() const {
    return (mState == STATE_RUN || mState == STATE_ON) && mNextOnOff && mNextOnOff < time(0);
  }

  bool shouldStopBecauseRain() const {
    if (mArgs.mIgnoreRainSensor & IGNORE_RAIN_SENSOR)
      return false;
    if (!isRunning())
      return false;
    if (!rs.getState())
      return false;
    return true;
  }

  bool isDone() const {
    if (mState == STATE_DONE)
      return true;
    if (mState != STATE_RUN && mDoneOn >= mArgs.repeats && !mNextRun)
      return true;
    return false;
  }
  bool isRunning() const {
    return mState != STATE_OFF && mState != STATE_DONE;
  }
  bool isOn() const {
    return mState == STATE_ON;
  }
  bool isOff() const {
    return mState != STATE_ON;
  }

  void changeOnOff();

  bool isNeedingPause() {
    if (mState != STATE_RUN) // XXX: allow other states? (this needs calculation of paused times, and saving old state)
      return false;
    if (mState == STATE_PAUSED)
      return false;

    if (mArgs.mIgnoreRainSensor & IGNORE_PUMP_PAUSE) {
      return false;
    }

    if (rvtp.needsPause(mArgs.valve_number)) {
      return true;
    }

    return false;
  }

  bool canUnpause() {
    if (mState != STATE_PAUSED)
      return false;

    if (mArgs.mIgnoreRainSensor & IGNORE_PUMP_PAUSE) {
      return true;
    }

    if (rvtp.needsPause(mArgs.valve_number))
      return false;

    return true;
  }

  sc_req_T checkState() {
    if (isNeedingPause())
      return SCR_PAUSE;
    if (isReadyToRun())
      return SCR_RUN;
    if (isReadyToOnOff())
      return SCR_ON_OFF;
    if (shouldStopBecauseRain())
      return SCR_RAIN;
    if (canUnpause())
      return SCR_UNPAUSE;

    return SCR_NONE;
  }
};


