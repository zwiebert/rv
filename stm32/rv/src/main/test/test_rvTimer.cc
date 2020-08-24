/*
 * test_rvTimer.cc
 *
 *  Created on: 06.07.2019
 *      Author: bertw
 */

#include "user_config.h"
#include "test.h"
#include "setup/app_cxx.h"
#include "rv/rv_timers.hh"
#include "rv/valve_relays.h"


extern bool rs_forceRain;

static RvTimer *rts[20];

enum ts {
  T_OW_SET_AND_RUN, T_OW_OVERWRITE_AND_RUN, T_OW_CHECK, T_OW_DELETE, T_OW_DONE,

  T_N_SET_AND_RUN, T_N_CHECK_STATS, T_N_DONE,

  T_A_SET_AND_RUN, T_A_CHECK_STATES, T_A_RAIN_ON, T_A_DONE,

  T_012_SET_AND_RUN, T_012_CHECK_0, T_012_CHECK_1P, T_012_CHECK_1, T_012_CHECK_2, T_012_DONE,


  T_DONE
};
inline enum ts operator++(enum ts &ts) { return (ts = (enum ts)((int)ts + 1)); }
static enum ts ts; //test state counter

static uint64_t  last_ms;
#define DO_WAIT(ms) (last_ms = runTime()+(ms))

extern "C" testRes_T testRvTimer_loop() {
  if (last_ms > runTime())
    return TR_RUNNING; // wait before continue;

  switch (ts) {

  case T_OW_SET_AND_RUN:
    for (int i = 0; i < 3; ++i) {
      rts[i] = rvt.set(i, 60, 0)->scheduleRun();
    }
    ++ts;
    DO_WAIT(10);
    break;
  case T_OW_OVERWRITE_AND_RUN:
    for (int i = 0; i < 3; ++i) {
      rts[i] = rvt.set(i, 40, 0)->scheduleRun();
    }
    ++ts;
    DO_WAIT(10);
    break;
  case T_OW_CHECK:
    if (rvt.getTimerList()->size() != 3)
      tr_failed();
    ++ts;
    break;
  case T_OW_DELETE:
    for (int i = 0; i < 3; ++i) {
      rts[i] = rvt.set(i, 0, 0)->scheduleRun();
    }
    ++ts;
    DO_WAIT(10);
    break;
  case T_OW_DONE:
    if (rvt.getTimerList()->size() != 0)
      tr_failed();
    if (valveRelay_getActiveValves())
      tr_failed();
    ++ts;
    break;


  case T_N_SET_AND_RUN:
    for (int i = 0; i < 3; ++i) {
      rts[i] = rvt.set(i, 0, 0)->scheduleRun();
    }
    ++ts;
    DO_WAIT(10);
    break;
  case T_N_CHECK_STATS:
    if (rvt.getTimerList()->size() > 0)
      break;
    if (valveRelay_getActiveValves())
      tr_failed();
    ++ts;
    break;
  case T_N_DONE:
    ++ts;
    break;


  case T_A_SET_AND_RUN:
    for (int i = 0; i < RV_VALVE_COUNT; ++i) {
      rts[i] = rvt.set(i, 1200, 0)->scheduleRun();
    }
    ++ts;
    break;
  case T_A_CHECK_STATES:
    if (rts[0]->mState != RvTimer::STATE_ON)
      break;
    if (rvt.getTimerList()->size() != RV_VALVE_COUNT)
      return tr_failed();
    for (int i = 1; i < RV_VALVE_COUNT; ++i) {
      if (rts[i]->mState != RvTimer::STATE_RUN)
        return tr_failed();
    }
    ++ts;
    break;
  case T_A_RAIN_ON:
    rs_forceRain = true;
   ++ts;
    break;
  case T_A_DONE:
    if (!rvt.getTimerList()->empty())
      break;
    rs_forceRain = false;
    ++ts;
    break;


  case T_012_SET_AND_RUN:
    rts[0] = rvt.set(0, 600, 0)->scheduleRun();
    rts[1] = rvt.set(1, 15, 0)->scheduleRun();
    rts[2] = rvt.set(2, 15, 0)->scheduleRun();
    ++ts;
    break;
  case T_012_CHECK_0:
    if(rvt.getTimerList()->size() != 3)
      return tr_failed();
    if (rts[0]->mState == RvTimer::STATE_RUN || rts[0]->mState == RvTimer::STATE_OFF)
      break;
    if (!(rts[0]->mState == RvTimer::STATE_ON && rts[1]->mState == RvTimer::STATE_RUN && rts[2]->mState == RvTimer::STATE_RUN))
      return tr_failed();
    ++ts;
    break;

  case T_012_CHECK_1P:
    if(rvt.getTimerList()->size() != 2)
      break;
    if (rts[1]->mState == RvTimer::STATE_RUN)
      break;
    if (&rvt.getTimerList()->front() != rts[1])
      return tr_failed();
    if (!(rts[1]->mState == RvTimer::STATE_PAUSED && rts[2]->mState == RvTimer::STATE_PAUSED))
      return tr_failed();
    ++ts;
    break;

  case T_012_CHECK_1:
    if(rvt.getTimerList()->size() != 2)
      break;
    if (rts[1]->mState == RvTimer::STATE_PAUSED)
      break;
    if (rts[1]->mState == RvTimer::STATE_RUN)
      break;
    if (&rvt.getTimerList()->front() != rts[1])
      return tr_failed();
    if (!(rts[1]->mState == RvTimer::STATE_ON && rts[2]->mState == RvTimer::STATE_RUN))
      return tr_failed();
    ++ts;
    break;

  case T_012_CHECK_2:
    if(rvt.getTimerList()->size() != 1)
      break;
    if (rts[2]->mState == RvTimer::STATE_RUN || rts[2]->mState == RvTimer::STATE_PAUSED)
      break;
    if (&rvt.getTimerList()->front() != rts[2])
      return tr_failed();
    if (!(rts[2]->mState == RvTimer::STATE_ON))
      return tr_failed();
    ++ts;
    break;

  case T_012_DONE:
    if (!rvt.getTimerList()->empty())
      break;
    ++ts;
    break;

  case T_DONE:
    return TR_SUCCEEDED;
  default:
    return TR_NOT_IMPLEMENTED;
  }
  return TR_RUNNING;
}

extern "C" void testRvTimer_setup() {

}
