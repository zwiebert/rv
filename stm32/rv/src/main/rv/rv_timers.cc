#include <rv/rv.hh>
#include <rv/rv_timer_pause.hh>
#include <rv/rv_timers.hh>
#include <rv/rv_zone.hh>
#include <rv/rv_zones.hh>

#include <algorithm>

#include "water_pump/water_pump.h"


void RvTimers::loop() {

  for (auto it = mActiveTimers.begin(); it != mActiveTimers.end();) {
    if (!it->isDone()) {
      ++it;
      continue;
    }
    rvt.destroy_timer(it++);
  }

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
}

