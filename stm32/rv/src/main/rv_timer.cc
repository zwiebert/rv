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

void RvTimers::loop() {


    for (RvTimer *t=mUsedTimers.succ; t; t = t->succ) {


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
  }

void RvTimers::timer_to_list(RvTimer *list, RvTimer *timer) {
    // unlink from current list
    if (timer->pred)
    timer->pred->succ = timer->succ;
    if (timer->succ)
      timer->succ->pred = timer->pred;

    // append to list
    RvTimer *tail = list->pred;
    tail->succ = timer;
    timer->pred = tail;

    list->pred = timer; // timer is new list tail
    timer->succ = 0;
  }
