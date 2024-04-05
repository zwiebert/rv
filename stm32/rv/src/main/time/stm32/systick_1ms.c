/*
 * systick_1ms.c
 *
 *  Created on: 19.06.2019
 *      Author: bertw
 */


#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>


#include "../systick_1ms.h"
#include "loop/loop.hh"
#include <utils_misc/int_macros.h>

volatile uint64_t run_time_ms;

uint64_t ms_runTime(void) {
  return run_time_ms;
}

bool ms_timeElapsed(uint64_t *last, int diff) {
  uint64_t now = run_time_ms;
  if (*last + diff >= now)
    return false;

  *last = now;
  return true;
}

void sys_tick_handler(void) {
  uint64_t ms = ++run_time_ms;
  static int countSec;
  if (--countSec <= 0) {
    countSec = 1000;
    lf_setBit(lf_interval_1s);
  }

  if ((ms & 511) == 0) {
    lf_setBit(lf_interval_512ms);
  }

  if ((ms & 63) == 0) {
    lf_setBit(lf_interval_64ms);
  }
}

void systick_setup(void) {

  /* 72MHz / 8 => 9000000 counts per second */
  systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);

  /* 9000000/9000 = 1000 overflows per second - every 1ms one interrupt */
  /* SysTick interrupt every N clock pulses: set reload to N-1 */
  systick_set_reload(8999);

  systick_interrupt_enable();

  /* Start counting. */
  systick_counter_enable();

}
