#include "rv_timers.hh"
#include "rv.hh"
#include "water_pump.h"
#include "report.h"
#include <algorithm>
#include "rv_zones.hh"


uint16_t RvTimers::valve_bits;
uint16_t RvTimers::valve_mask;

void RvTimers::loop() {

  mActiveTimers.remove_if([](RvTimer &vt) -> bool {
    return (vt.isDone());
  });

  for (RvTimer &vt : mActiveTimers) {
    switch (vt.checkState()) {

    case RvTimer::SCR_ON_OFF:
      if (vt.isOff()) { // ready to turn on
        if (RvTimer::rvtp.getLph() + rvz[vt.getValveNumber()].getLph() < RV_MAX_LPH) {
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

