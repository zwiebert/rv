/*
 * test_rvTimer.cc
 *
 *  Created on: 06.07.2019
 *      Author: bertw
 */

#include "test.h"
#include "app_cxx.h"
#include "rv_timer.hh"

static int ts; //test state counter

static RvTimer *rts[20];

extern "C" testRes_T testRvTimer_loop() {
  switch (ts) {
  case 0:
    for (int i= 0; i < RV_VALVE_COUNT; ++i) {
      rts[i] = rvt.set(i, 1200, 0);
      rts[i]->scheduleRun();
    }
    ++ts;
    break;


  default:
    return TR_NOT_IMPLEMENTED;
  }
  return TR_RUNNING;
}

extern "C" void testRvTimer_setup() {

}
