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

void SysTick_Handler(void)
{
  ++run_time_ms;
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
