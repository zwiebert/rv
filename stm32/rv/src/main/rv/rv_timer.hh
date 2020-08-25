#pragma once

#include "user_config.h"
#include "rv.hh"
#include "rain_sensor.hh"
#include "setup/app_cxx.hh"

#include "misc/int_macros.h"
#include "time/real_time_clock.h"
#include "water_pump.h"
#include "rv_timer_data.hh"

#include <time.h>
#include <stdio.h>
#include <string.h>


typedef void (*switch_valve_cb)(int valve_number, bool state);
typedef void (*switch_valves_cb)(uint16_t valve_bits, uint16_t valve_mask);



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

  bool isRunOnce() {
    return mArgs.on_duration && !mArgs.off_duration && !mArgs.repeats && !mArgs.period;
  }
  bool hasTodSpan() {
    return mArgs.mTodSpanBegin || mArgs.mTodSpanEnd;
  }


  bool isDisabledByInterval(time_t now = time(0)) {

    if (mLastRun && mArgs.mDaysInterval && mLastRun + mArgs.mDaysInterval * ONE_DAY < now)
      return true;

    return false;
  }

  bool isDisabledByRain() {
    if (mArgs.mIgnoreRainSensor & IGNORE_RAIN_SENSOR)
      return false;
    if (rs.getState())
      return true; // raining now or was raining (adjust sensor directly)

    return false;
  }

  bool isDisabledByTimeOfDay(time_t now = time(0)) {
    if (mArgs.mTodSpanBegin == mArgs.mTodSpanEnd)
      return false;

    struct tm *tm_now = localtime(&now);
    int dsecs = tm_now->tm_hour * ONE_HOUR + tm_now->tm_min * ONE_MINUTE + tm_now->tm_sec;
    if (!(mArgs.mTodSpanBegin <= dsecs && dsecs <= mArgs.mTodSpanEnd))
      return true;

    return false;
  }




public:
  RvTimer() {
  }
  char *argsToJSON(char *buf, int buf_size) {
    if (0 <= snprintf(buf + strlen(buf), buf_size - strlen(buf), "\"timer%d.%d\":", getValveNumber(), getTimerNumber())) {
      return mArgs.toJSON(buf + strlen(buf), buf_size - strlen(buf));
    }
    return 0;
  }

  char *dataToJSON(char *buf, int buf_size) {
    if (0 <= snprintf(buf + strlen(buf), buf_size - strlen(buf), "\"timer%d.%d\":", getValveNumber(), getTimerNumber())) {
      return toJSON(buf + strlen(buf), buf_size - strlen(buf));
    }
    return 0;
  }

  int getValveNumber() {
    return mValveNumber;
  }
  int getTimerNumber() {
    return mTimerNumber;
  }
  bool match(int valve_number, int id = 0) {
    return valve_number == mValveNumber && id == mTimerNumber;
  }

  void register_callback(switch_valve_cb cb, int valve_number) {
    mSwitchValveCb = cb;
    mValveNumber = valve_number;
  }

  void switch_valve(bool state) {
    if (mSwitchValveCb) {
      mSwitchValveCb(mValveNumber, state);
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


  int get_duration() {
    return mArgs.on_duration;
  }
  int get_remaining() {
    return isOn() ? mNextOnOff - time(0) : mArgs.on_duration;
  }

  void set(SetArgs &args, int id) {
    mTimerNumber = id;
    mArgs = args;
    if (rs.getState() && !(args.period || args.mDaysInterval || args.mIgnoreRainSensor & IGNORE_RAIN_SENSOR)) {
      mArgs.on_duration = 0; // force timer to stop
    }
  }

  void set(int on_duration, int id) {
    SetArgs args;
    args.on_duration = on_duration;
    set(args, id);
  }

  bool isReadyToRun() {
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

  bool isReadyToOnOff() {
    return (mState == STATE_RUN || mState == STATE_ON) && mNextOnOff && mNextOnOff < time(0);
  }

  bool shouldStopBecauseRain() {
    if (mArgs.mIgnoreRainSensor & IGNORE_RAIN_SENSOR)
      return false;
    if (!isRunning())
      return false;
    if (!rs.getState())
      return false;
    return true;
  }

  bool isDone() {
    if (mState == STATE_DONE)
      return true;
    if (mState != STATE_RUN && mDoneOn >= mArgs.repeats && !mNextRun)
      return true;
    return false;
  }
  bool isRunning() {
    return mState != STATE_OFF && mState != STATE_DONE;
  }
  bool isOn() {
    return mState == STATE_ON;
  }
  bool isOff() {
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

    if (rvtp.needsPause(mValveNumber)) {
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

    if (rvtp.needsPause(mValveNumber))
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


