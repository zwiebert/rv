/*
 * app.c
 *
 *  Created on: 29.04.2019
 *      Author: bertw
 */

#include "user_config.h"

#include <loop/loop.hh>
#include <peri/mcp23017.h>
#include <rv/report.h>
#include <water_pump/water_pump.h>
#include <sys/_timeval.h>
#include <sys/errno.h>
#include <time/real_time_clock.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/f1/memorymap.h>

void loop(void);
void setup();

void atFault() {
  mcp23017_atFault();
}

void atFault_setup() {
}

void app() {
  setup();
  report_event("mcu:started");
  wp_clearPcFailure(); //

  for (;;) {
    loop();
  }
}

time_t time(time_t *p) {
  return curr_time;
}

// redirect any output to USART1
int _write(int fd, char *ptr, int len) {
  int i;

  if (fd == 1) {
    for (i = 0; i < len; i++)
      usart_send_blocking(USART1, ptr[i]);
    return i;
  }

  errno = EIO;
  return -1;
}

void loop(void) {
#ifdef USE_TEST
  {
    static testRes_T tr;
    if (tr == TR_RUNNING) {
      tr = test_loop();
      if (tr == TR_FAILED) {
        while(1) { }
      }
    }
  }
#endif
#ifdef USE_DLB8
  dlb8_loop();
#endif // USE_DLB8
  lf_loop();
}
