#include <cli/cli.h>
#include <loop/loop.hh>
#include <utils_misc/int_macros.h>
#include <peri/uart.h>
#include <peri/i2c.h>
#include <rv/rain_sensor.hh>
#include <rv/report.h>
#include <rv/rv.hh>
#include <rv/rv_timers.hh>
#include <water_pump/water_pump_logic.h>
#include <time/systick_1ms.h>
#include <watch_dog/watch_dog.hh>

#include <cstdio>

extern RainSensor rs;

volatile int loop_flags_once;
#ifndef loop_flags_periodic
volatile int loop_flags_periodic;
#endif

typedef void (*lfa_funT)(void);

static void i2c2_check_loop() {
  if (!i2c2_check()) {
    report_event("i2c:reset");
    puts("I2C had crashed. Reset");
  }
}

bool got_zoneData;
static void fetchData_loop() {
  if (!got_zoneData) {
    // esp32_puts("{\"to\":\"cli\",\"pbuf\":{\"zd\":\"?\"}}\n");
    esp32_puts("{\"to\":\"cli\",\"kvs\":{\"lph\":\"?\"}};\n");
    //esp32_puts("{\"to\":\"cli\",\"kvs\":{\"zn\":\"?\"}}\n");
  }
}

static void interval_1s_loop() {
#ifdef USE_WDG
  watchDog_loop();
#endif
  i2c2_check_loop();
  fetchData_loop();
}

static void interval_512ms_loop() {
  rs.loop();
  rvt.loop();
}

static void interval_64ms_loop() {
  wpl_loop();
}

static const lfa_funT lfa_table[lf_Len] = { interval_1s_loop, interval_512ms_loop, interval_64ms_loop, cli_loop, wpl_loop, fetchData_loop, };

extern "C" void lf_loop() {
  for (int i = 0; i < 4500 && !loop_flags_once; ++i) {
    __asm__("nop");
  }
  // XXX: interrupt should be disabled here
  int loop_flags = loop_flags_periodic | loop_flags_once;
  loop_flags_once = 0;
  // XXX: Interrupt should be re-enabled here

  for (int i = 0; i < lf_Len; ++i) {
    enum loop_flagbits fb = (enum loop_flagbits) i;
    if (!GET_BIT(loop_flags, fb))
      continue;

    (lfa_table[fb])();

  }

}
