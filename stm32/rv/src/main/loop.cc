#include "loop.hh"
#include "misc/int_macros.h"

#include "water_pump_logic.h"
#include "cli/cli.h"
#include "app_cxx.h"
#include "report.h"
#include <stdio.h>
#include "watch_dog.hh"
#include "rv_timer.hh"

extern RvTimers rvt;
extern RainSensor rs;

volatile int loop_flags_once;
volatile int loop_flags_periodic = BIT(lf_wpl) | BIT(lf_ic2c_check);

typedef void (*lfa_funT)(void);

extern "C" bool i2c2_check();

static void i2c2_check_loop() {
  if (!i2c2_check()) {
    report_event("i2c:reset");
    puts("I2C had crashed. Reset");
  }
}

static void interval_1s_loop() {
#ifdef USE_WDG
    watchDog_loop();
#endif
}

static void interval_500ms_loop() {
  rs.loop();
  rvt.loop();
}

static const lfa_funT lfa_table[lf_Len] = {
    interval_1s_loop,
    interval_500ms_loop,
    cli_loop,
    wpl_loop,
    i2c2_check_loop,
};




extern "C" void lf_loop() {
  for (unsigned long i = 0; i < 4500 && !loop_flags_once; ++i) {
    __asm__("nop");
  }
  // XXX: interrupt should be disabled here
  int loop_flags = loop_flags_periodic | loop_flags_once;
  loop_flags_once = 0;
  // XXX: Interrupt should be re-enabled here

  for (int i = 0; i < lf_Len; ++i) {
    enum loop_flagbits fb = (enum loop_flagbits)i;
    if (!GET_BIT(loop_flags, fb))
      continue;

    (lfa_table[fb])();

  }

}
