#include "loop.hh"
#include "misc/int_macros.h"

#include "water_pump_logic.h"
#include "cli/cli.h"
#include "app_cxx.h"
#include "report.h"
#include <stdio.h>

int loop_flags_once;
int loop_flags_periodic = BIT(lf_wpl) | BIT(lf_cxx) | BIT(lf_ic2c_check);

typedef void (*lfa_funT)(void);

extern "C" bool i2c2_check();

static void i2c2_check_loop() {
  if (!i2c2_check()) {
    report_event("i2c:reset");
    puts("I2C had crashed. Reset");
  }
}


static const lfa_funT lfa_table[lf_Len] = {
    wpl_loop,
    cli_loop,
    cxx_loop,
    i2c2_check_loop,
};




extern "C" void lf_loop() {
  for (unsigned long i = 0; i < 4500 && !loop_flags_once; ++i) {
    __asm__("nop");
  }

  int loop_flags = loop_flags_periodic | loop_flags_once;
  loop_flags_once = 0;

  for (int i = 0; i < lf_Len; ++i) {
    enum loop_flagbits fb = (enum loop_flagbits)i;
    if (!GET_BIT(loop_flags, fb))
      continue;

    (lfa_table[fb])();

  }

}
