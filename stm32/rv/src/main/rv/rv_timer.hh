/**
 * \file   rv/rv_timer.hh
 * \brief  a zone timer
 */

#pragma once

#include <rv/rain_sensor.hh>
#include <rv/rv.hh>
#include <rv/rv_timer_data.hh>
#include <rv/rv_timer_pause.hh>
#ifndef MCU_HOST
#include <sys/_timeval.h>
#endif
#include <time/real_time_clock.h>
#include <time.h>

#include <cstdio>
#include <cstring>


typedef void (*switch_valve_cb)(int valve_number, bool state);




#define IGNORE_RAIN_SENSOR 1
#define IGNORE_PUMP_PAUSE 2


/// \brief zone timer class
class RvTimer: public RvTimerData  {
  friend class RvTimers;
  static RvTimerPause rvtp;
private:
  time_t mNextOnOff = 0; ///< time of next state change on to off or vice versa
  //bool mIsRunning = false, mIsOn = false;
  int mDoneOn = 0;
  switch_valve_cb mSwitchValveCb = 0; ///< callback to actuate the related valve
public:
  /// \brief events
  typedef enum { SCR_NONE, SCR_RUN, SCR_ON_OFF, SCR_RAIN, SCR_PAUSE, SCR_UNPAUSE } sc_req_T;
  /// \brief states
  typedef enum { STATE_OFF, STATE_RUN, STATE_PAUSED, STATE_ON, STATE_DONE } state_T;
  state_T mState = STATE_OFF; ///< holding the current state of this timer

private:
  void changeState(state_T state);  ///< change state to STATE

  /// \brief    test if timer needs to run once or repeatedly
  /// \return   true if timer needs or needed to run only once
  bool isRunOnce() const {
    return mArgs.on_duration && !mArgs.off_duration && !mArgs.repeats && !mArgs.period;
  }
  /// \brief   test if timer has received an time of day span argument
  /// \return  true if a time of day span is defined for this timer
  bool hasTodSpan() const {
    return mArgs.mTodSpanBegin != mArgs.mTodSpanEnd;
  }

  /// \brief    test if timer is today disabled because of day interval
  /// \return   true if this timer is disabled
  bool isDisabledByInterval(time_t now = time(0)) const {

    if (mLastRun && mArgs.mDaysInterval && mLastRun + mArgs.mDaysInterval * ONE_DAY < now)
      return true;

    return false;
  }

  /// \brief   test if timer is currently disabled by wet rain sensor
  /// \return  true if this timer is disabled
  bool isDisabledByRain() const {
    if (mArgs.ignoreRainSensor)
      return false;
    if (rs.getState())
      return true; // raining now or was raining (adjust sensor directly)

    return false;
  }

  /// \brief   test if timer is currently disabled by time of day span
  /// \return  true if this timer is disabled
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

  bool operator==(const RvTimer &rhs) const { return mArgs.valve_number == rhs.mArgs.valve_number && mArgs.timer_number == rhs.mArgs.timer_number; }

#if 1
  /// \brief                 serialize this object to JSON
  /// \param buf,buf_size    destination buffer provided by caller
  /// \return                json (== buf) or NULL
  char* argsToJSON(char *buf, int buf_size) const {
    constexpr char prefix[] = "\"timer\":";
    if (!strncat(buf, prefix, buf_size))
      return 0;
    if (!mArgs.toJSON(buf + (sizeof prefix - 1), buf_size - (sizeof prefix - 1)))
      return 0;
    return buf;
  }

#else
  char *argsToJSON(char *buf, int buf_size) const {
    if (0 <= std::snprintf(buf + std::strlen(buf), buf_size - std::strlen(buf), "\"timer%d.%d\":", getValveNumber(), getTimerNumber())) {
      return mArgs.toJSON(buf + std::strlen(buf), buf_size - std::strlen(buf));
    }
    return 0;
  }

  char *dataToJSON(char *buf, int buf_size) const {
    if (0 <= std::snprintf(buf + std::strlen(buf), buf_size - std::strlen(buf), "\"timer%d.%d\":", getValveNumber(), getTimerNumber())) {
      return toJSON(buf + std::strlen(buf), buf_size - std::strlen(buf));
    }
    return 0;
  }
#endif

  /// \brief    get the one related valve number
  /// \return   valve number or -1
  int getValveNumber() const {
    return mArgs.valve_number;
  }

  /// \brief   get the timer number
  /// \return  timer number 0...x
  int getTimerNumber() const {
    return mArgs.timer_number;
  }

  /// \brief                match our timer by VALVE_NUMBER and TIMER_NUMBER
  /// \param valve_number   a valve number
  /// \param id             a timer number
  /// \return               true if VALVE_NUMBER and ID match with this timers valve_number and timer_number
  bool match(int valve_number, int id = 0) const {
    return valve_number == mArgs.valve_number && id == mArgs.timer_number;
  }

  /// \brief               register a callback to actuate the actual valve and set the valve_number
  /// \param cb            callback to actuate the valve
  /// \param valve_number  will set (overwrite) this timers valve_number
  void register_callback(switch_valve_cb cb, int valve_number) {
    mSwitchValveCb = cb;
    mArgs.valve_number = valve_number;
  }

  /// \brief               open or close valve
  /// \param open_valve    true: opens valve, false: closes valve
  void switch_valve(bool open_valve) {
    if (mSwitchValveCb) {
      mSwitchValveCb(mArgs.valve_number, open_valve);
    }
  }

  /// \brief      set state to STATE_RUN, if not STATE_DONE already
  void run() {
    if (mState == STATE_DONE)
      return;
    mNextOnOff = time(0);
    changeState(STATE_RUN);
    mDoneOn = 0;
  }

  /// \brief     mark this timer as ready to run, if not STATE_DONE already
  RvTimer *scheduleRun() {
    if (mState == STATE_DONE)
      return this;

    mNextRun = time(0);
    return this;
  }

  /// \brief stop this timer and prepare the following actions like next run, last run or done.
  void stop();
  /// \brief    enter state STATE_PAUSED
  void pause();
  //// \brief   enter state STATE_RUN
  void unpause();

  /// \brief   Get on-duration.
  /// \return  duration in s
  int get_duration() const {
    return mArgs.on_duration;
  }

  /// \brief    Get remaining time of on-duration.
  /// \return   duration in s
  int get_remaining() const {
    return isOn() ? mNextOnOff - time(0) : mArgs.on_duration;
  }

  /// \brief        Set the timer options from UI.
  ///
  ///  If timer is currently running then stop it.
  ///
  /// \param args   parameter struct
  void set(SetArgs &args) {
    mArgs = args;
    if (rs.getState() && !(args.period || args.mDaysInterval || args.ignoreRainSensor)) {
      mArgs.on_duration = 0; // force timer to stop
    }
  }

  /// \brief                Set the timer options by UI, using mostly defaults
  /// \param on_duration    on_duration in seconds
  /// \param id             timer_number
  void set(int on_duration, int id) {
    SetArgs args;
    args.on_duration = on_duration;
    args.timer_number = id;
    set(args);
  }

  /// \brief      Test if this timer could run now and is not disabled by anything
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

  /// \brief      Test if this timer is ready to change state from on to off or vice versa
  bool isReadyToOnOff() const {
    return (mState == STATE_RUN || mState == STATE_ON) && mNextOnOff && mNextOnOff < time(0);
  }

  /// \brief     Test if this timer is running and should be stopped because of wet rain sensor
  bool shouldStopBecauseRain() const {
    if (mArgs.ignoreRainSensor)
      return false;
    if (!isRunning())
      return false;
    if (!rs.getState())
      return false;
    return true;
  }

  /// \brief    Test if this timer (should be removed because) its done
  bool isDone() const {
    if (mState == STATE_DONE)
      return true;
    if (mState != STATE_RUN && mDoneOn >= mArgs.repeats && !mNextRun)
      return true;
    return false;
  }
  /// \brief   Test if this timer is currently running
  bool isRunning() const {
    return mState != STATE_OFF && mState != STATE_DONE;
  }
  /// \brief    Test if state is STATE_ON
  bool isOn() const {
    return mState == STATE_ON;
  }
  /// \brief    Test if state is not STATE_ON
  bool isOff() const {
    return mState != STATE_ON;
  }

  /// \brief Try to change STATE_ON (???)
  void changeOnOff();

  /// \brief  Test if this timer runs and should enter STATE_PAUSED
  bool isNeedingPause() {
    if (mState != STATE_RUN) // XXX: allow other states? (this needs calculation of paused times, and saving old state)
      return false;
    if (mState == STATE_PAUSED)
      return false;

    if (mArgs.ignorePumpPause) {
      return false;
    }

    if (rvtp.needsPause(mArgs.valve_number)) {
      return true;
    }

    return false;
  }

  /// \brief  Test if this timer should leave STATE_PAUSED now
  bool canUnpause() {
    if (mState != STATE_PAUSED)
      return false;

    if (mArgs.ignorePumpPause) {
      return true;
    }

    if (rvtp.needsPause(mArgs.valve_number))
      return false;

    return true;
  }


  /// \brief  Generate an event depending on current state
  /// \return an event which could then be passed to an event handler
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


