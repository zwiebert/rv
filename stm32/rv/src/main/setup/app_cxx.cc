/*
 * app.cc
 *
 *  Created on: 04.06.2019
 *      Author: bertw
 */

#include <malloc.h>
#include <rv/rv.hh>
#include <rv/rv_timers.hh>
#include <stddef.h>
#include <watch_dog/watch_dog.hh>
#include "setup/app_cxx.h"


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
void operator delete(void *p, size_t ) {
  free(p);
}
#endif

void timers_was_modified(int vn, int tn, bool removed);

extern "C" void cxx_setup() {
#ifdef USE_WDG
	watchDog_setup();
#endif
	rvt.register_callback(timers_was_modified);
	// rvt.set(0,2,4,3,60)->run();
	// rvt.set(1,3,5,4,130)->run();
	// rvt.set(2,3)->run();
	//rvt.set(6, 30, 0, 0, 60 * 2)->run();
	//rvt.set(7, 30, 0, 0, 60 * 2)->run();
}

