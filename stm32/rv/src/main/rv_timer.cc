#include "rv_timer.hh"


int Lph[RV_VALVE_COUNT] = {
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
                           1000, //
};

uint16_t RvTimers::valve_bits;
uint16_t RvTimers::valve_mask;

void RvTimers::loop() {

	valve_bits = valve_mask = 0;

    for (RvTimer *t=mUsedTimers.succ; t != &mUsedTimers; t = t->succ) {


      if (t->isDone()) {
        RvTimer *pred = t->pred;
        timer_to_list(&mFreeTimers, t);
        t = pred;
        continue;
      }

      RvTimer &vt = *t;

      if (vt.checkState()) {
      if (vt.isReadyToOnOff()) {
        if (vt.isOff()) { // ready to turn on
          if (lph + Lph[vt.getValveNumber()] < RV_MAX_LPH) {
            lph +=  Lph[vt.getValveNumber()];
            vt.changeOnOff();
          }
        } else { // ready to turn off
          vt.changeOnOff();
          lph -=  Lph[vt.getValveNumber()];
        }
      } else if (vt.isReadyToRun()) {
        vt.run();
      }
      }
    }

    	if (valve_mask && mSvsCb) {
    		mSvsCb(valve_bits, valve_mask);
    	}

  }

void RvTimers::timer_to_list(RvTimer *list, RvTimer *timer) {
    // unlink from current list
	timer->unlinkThis();

    // append to list
    RvTimer *tail = list->pred;
    tail->succ = timer;
    timer->pred = tail;

    list->pred = timer; // timer is new list tail
    timer->succ = list;
  }
