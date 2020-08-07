#ifndef RV_TIMER_HH
#define RV_TIMER_HH

#include "rain_sensor.hh"
#include "app_cxx.hh"
#include "user_config.h"
#include "misc/int_macros.h"
#include "real_time_clock.h"
#include "water_pump.h"
#include "list.hh"

#include <time.h>
#include <stdio.h>
#include <string.h>
#ifdef MOD_TEST
#include <iostream>
volatile time_t start_time;
volatile int x;
void breaker() {
  // start_time = time(0);
}

#endif

const int RV_TIMER_COUNT = 20;
const int RV_VALVE_COUNT = 12;
const int RV_MAX_LPH = 1600;

extern int Lph[RV_VALVE_COUNT];

typedef void (*switch_valve_cb)(int valve_number, bool state);
typedef void (*switch_valves_cb)(uint16_t valve_bits, uint16_t valve_mask);

class RvTimer;
class RvTimers;

#define IGNORE_RAIN_SENSOR 1
#define IGNORE_PUMP_PAUSE 2


class RvTimerPause {
#define PAUSE_SECS_PER_LITER 8
#define PAUSE_AFTER_LITER 100
  int mLitersBeforePause = 0;
  unsigned mLph = 0;
  run_time_T mLastLphChange = 0;

  unsigned pauseDuration() { return mLitersBeforePause * PAUSE_SECS_PER_LITER; }
public:
  unsigned getLph()  { return mLph; }
  void lphChange(int lph) {
    if (!lph)
      return;


    run_time_T now = runTime();
    run_time_T dur = now - mLastLphChange;

    if (mLph) {
      mLitersBeforePause += (int)(((double)mLph / 3600.0) * (double)dur);
    }

    mLph += lph;
    mLastLphChange = now;
  }

  bool needsPause(int zone = -1) {
    if (mLitersBeforePause == 0)
      return false;

    run_time_T dur = pauseDuration();

    run_time_T sinceLastLphChange = (runTime() - mLastLphChange);
    if (dur > sinceLastLphChange)
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

struct RvTimerData {
public:
  struct SetArgs {
    int on_duration = 0, off_duration = 0, repeats = 0, period = 0;
    int mDaysInterval = 0, mTodSpanBegin = 0, mTodSpanEnd = 0;
    int mIgnoreRainSensor = 0;
#define SA_JSON_FMT "{\"d1\":%d,\"ir\":%d,\"d0\":%d,\"r\":%d,\"per\":%d,\"di\":%d,\"sb\":%d,\"se\":%d}"
    char *toJSON(char *buf, int buf_size) {
      if (0
          <= snprintf(buf, buf_size, SA_JSON_FMT, on_duration, mIgnoreRainSensor,
              off_duration, repeats, period, mDaysInterval, mTodSpanBegin, mTodSpanEnd))
        return buf;

      return 0;
    }
    SetArgs() {}
    SetArgs(const char *json) {
      json = strstr(json, "{\"d1\":");
      if (json) {
        sscanf(json, SA_JSON_FMT, &on_duration, &mIgnoreRainSensor, &off_duration, &repeats, &period, &mDaysInterval, &mTodSpanBegin, &mTodSpanEnd);
      }
    }
  };
protected:
  SetArgs mArgs;
  int mValveNumber = -1;
  int mTimerNumber = 0; // multiple time per valve with different numbers
  time_t mNextRun = 0;
  time_t mLastRun = 0;

public:
#define TD_JSON_PF_FMT "{\"vn\":%d,\"tn\":%d,\"nr\":%ld,\"lr\":%ld,\"args\":%s}"
#define TD_JSON_SF_FMT "{\"vn\":%d,\"tn\":%d,\"nr\":%ld,\"lr\":%ld,"
  char *toJSON(char *buf, int buf_size) {
    char aBuf[128] = "";
    const char *aJson = mArgs.toJSON(aBuf, sizeof aBuf);

    if (0
        <= snprintf(buf, buf_size, TD_JSON_PF_FMT, mValveNumber, mTimerNumber, (uint32_t)mNextRun, (uint32_t)mLastRun, aJson))
      return buf;

    return 0;
  }

  RvTimerData() {}
  RvTimerData(const char *json): mArgs(json) {
    uint32_t nextRun = 0, lastRun = 0; // long long currently not working with printf/scanf
    sscanf(json, TD_JSON_SF_FMT, &mValveNumber, &mTimerNumber, &nextRun, &lastRun);
    mNextRun = nextRun;
    mLastRun = lastRun;
  }
};

class RvTimer: public Node<RvTimer>, public RvTimerData  {
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

//////////////////////////////////////////////////////////////////////////////////////////////////

class RvTimers {
  static uint16_t valve_bits, valve_mask;
  static void switch_valve_2(int valve_number, bool state) {
    SET_BIT(valve_mask, valve_number);
    if (state)
      SET_BIT(valve_bits, valve_number);
  }
private:

  class Timers {
  private:
    RvTimer mTimerPool[RV_TIMER_COUNT];
  public:
    List<RvTimer> mFreeTimers, mUsedTimers;
    Timers() {

      for (int i = 0; i < RV_TIMER_COUNT; ++i) {
        mFreeTimers.append(&mTimerPool[i]);
      }
    }

    RvTimer *get_timer() {
      RvTimer *res = mFreeTimers.pop();
      if (res)
        mUsedTimers.append(res);
      return res;
    }

    void delete_timer(RvTimer *t) {
      mUsedTimers.remove(t);
      *t = RvTimer();  // re-init
      mFreeTimers.append(t);
    }

    unsigned get_used_count() { return mUsedTimers.length(); }
    unsigned get_free_count() { return mFreeTimers.length(); }

  } mRvTimers;

  switch_valve_cb mSvCb; // XXX
  switch_valves_cb mSvsCb;

public:

  unsigned get_used_count() { return mRvTimers.get_used_count(); }
  unsigned get_free_count() { return mRvTimers.get_free_count(); }

  RvTimers(switch_valve_cb cb, switch_valves_cb cb2) :
      mSvCb(cb), mSvsCb(cb2) {

  }

  void register_callback(switch_valve_cb cb) {
    mSvCb = cb; // XXX
  }

  RvTimer *set(RvTimer::SetArgs &args, int valve_number, int id) {
    for (RvTimer *t = mRvTimers.mUsedTimers.getNext(); t; t = t->getNext()) {
      if (t->match(valve_number, id)) {
        t->changeState(RvTimer::STATE_DONE);
        break;
      }
    }

    RvTimer *timer = mRvTimers.get_timer();
    if (!timer)
      return 0;

    if (args.on_duration == 0) {
      timer->changeState(RvTimer::STATE_DONE);
    }

    timer->register_callback((mSvCb ? mSvCb : switch_valve_2), valve_number);
    timer->set(args, id);
    return timer;

  }

  RvTimer *set(int valve_number, int on_duration, int id = 0) {
    RvTimer::SetArgs args;
    args.on_duration = on_duration;
    return set(args, valve_number, id);
  }

  void unset(int valve_number, int id) {
    for (RvTimer *t = mRvTimers.mUsedTimers.getNext(); t; t = t->getNext()) {
      if (t->match(valve_number, id)) {
        mRvTimers.delete_timer(t);
      }
    }
  }

  void loop();

public:
  List<RvTimer> *getTimerList() {
    return &mRvTimers.mUsedTimers;
  }
};

#ifdef MOD_TEST
#include <stdlib.h>
#include <iostream>

void switch_valve(int valve_number, bool state) {
  std::cout << (time(0) - start_time) << ": valve " << valve_number << " switched " << (state ? "on" : "off") << std::endl;
  std::cout.flush();
}

extern "C" void app_switch_valve(int valve_number, bool state);

int main() {
  start_time = time(0);
  RvTimers rvt = RvTimers(switch_valve);

  // rvt.set(0,2,4,3,60)->run();
  // rvt.set(1,3,5,4,130)->run();
  // rvt.set(2,3)->run();

  rvt.set(6, 30, 0, 0, 60*2)->run();
  rvt.set(7, 30, 0, 0, 60*2)->run();
  while (1) {
    rvt.loop();
    //sleep(100);
  }
}

#endif

#endif
// Local Variables:
// compile-command: "/c/MinGW/bin/g++ -ggdb rv_timer.cc"
// End:
