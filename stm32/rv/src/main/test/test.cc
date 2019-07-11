/*
 * test.cc
 *
 *  Created on: 06.07.2019
 *      Author: bertw
 */

#include "test.h"

typedef testRes_T (*testLoop_T)();

typedef struct {
  testLoop_T tl;
  testRes_T tr;
} testData_T;

enum {TD_RV_TIMER, TD_RV_count};

testData_T tds[TD_RV_count] = {
    {testRvTimer_loop, },
};

extern "C" testRes_T tr_failed() {
  return TR_FAILED; // set debugger breakpoint here
}

extern "C" enum test_res test_loop() {
  int countFailed = 0;
  int countSucceeded = 0;
  int countRunning = 0;
  int countNotImplemented = 0;

  for (int i = 0; i < TD_RV_count; ++i) {
    testData_T td = tds[i];
    if (td.tr == TR_RUNNING) {
      if (!td.tl) {
        td.tr = TR_NOT_IMPLEMENTED;
      } else {
        td.tr = td.tl();
      }

    }

    switch (td.tr) {
    case TR_RUNNING:
      ++countRunning;
      break;
    case TR_FAILED:
      ++countFailed;
      break;
    case TR_SUCCEEDED:
      ++countSucceeded;
      break;
    case TR_NOT_IMPLEMENTED:
      ++countNotImplemented;
      break;
    }
  }

  if (countRunning > 0) {
      return TR_RUNNING;
  }

  if (countFailed > 0) {
    return TR_FAILED;
  }

  if (countSucceeded > 0) {
    return TR_SUCCEEDED;
  }

  return TR_NOT_IMPLEMENTED;
}

extern "C" void test_setup() {

}
