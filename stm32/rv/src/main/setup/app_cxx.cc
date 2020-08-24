/*
 * app.cc
 *
 *  Created on: 04.06.2019
 *      Author: bertw
 */

#include "user_config.h"
#include "rv/rv_timers.hh"
#include "setup/app_cxx.h"
#include "watch_dog/watch_dog.hh"
#include "time/systick_1ms.h"
#include "rv/valve_relays.h"

RvTimers rvt = RvTimers(0, app_switch_valves);
RainSensor rs;

#define RVT_LOOP_MS  500
#define WD_LOOP_MS 1000

#ifdef USE_MALLOC_IN_NEW_GLOBAL
void* operator new(size_t nbytes) {
  return malloc(nbytes);
}

void* operator new[](size_t nbytes) {
  return malloc(nbytes);
}

void operator delete(void *p) {
  free(p);
}
#endif


extern "C" void cxx_setup() {
#ifdef USE_WDG
	watchDog_setup();
#endif
	// rvt.set(0,2,4,3,60)->run();
	// rvt.set(1,3,5,4,130)->run();
	// rvt.set(2,3)->run();
	//rvt.set(6, 30, 0, 0, 60 * 2)->run();
	//rvt.set(7, 30, 0, 0, 60 * 2)->run();
}


