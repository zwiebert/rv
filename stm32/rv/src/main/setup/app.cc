/*
 * app.c
 *
 *  Created on: 29.04.2019
 *      Author: bertw
 */

#include "user_config.h"

#include <loop/loop.hh>
#include <peri/relay16.h>
#include <rv/report.h>
#include <water_pump/water_pump.h>
#include <sys/_timeval.h>
#include <sys/errno.h>
#include <time/real_time_clock.h>
#include <peri/uart.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/f1/memorymap.h>

void loop(void);
void app_setup();

extern "C" void atFault() {
  relay16_atFault();
}

void app() {
  app_setup();
  report_event("mcu:started");
  wp_clearPcFailure(); //

  for (;;) {
    loop();
  }
}

time_t time(time_t *p) {
  if (p)
    *p = curr_time;
  return curr_time;
}

// redirect standard output (fd 1) and stderr (fd 2)
extern "C" int _write(int fd, char *ptr, int len) {
  if (fd != 1 && fd != 2) {
    errno = EIO;
    return -1;
  }

  return esp32_write(ptr, len);
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
