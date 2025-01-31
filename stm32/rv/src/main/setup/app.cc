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
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/f1/memorymap.h>
#include <peri/uart.h>
#include <cstdio>

void loop(void);
void app_setup();

extern "C" void hard_fault_handler() {
  relay16_atFault();
  fputs("\n\nerror: program stopped in hard fault handler ###", stderr);
  while(1);
}

#include <cstdio>

int main() {
  app_setup();
  report_event("mcu:started");
  fputs("mcu:started\n", stderr);
  wp_clearPcFailure(); //

  for (;;) {
    loop();
  }
}

#ifndef MCU_HOST
time_t time(time_t *p) {
  if (p)
    *p = currentTime();
  return currentTime();
}


#define CALL_FUNC(usartn, func, args) usart ##usartn ## _ ## func args
#define CALL_FUNC2(usartn, func, args) CALL_FUNC(usartn, func, args)

// redirect standard output (fd 1) and stderr (fd 2)
extern "C" int _write(int fd, char *ptr, int len) {
  switch(fd){
  case 1:
     return CALL_FUNC2(CONFIG_USART_STDOUT, write, (ptr, len));

#if defined CONFIG_USART_STDERR && CONFIG_USART_STDERR != 0
  case 2:
     return CALL_FUNC2(CONFIG_USART_STDERR, write, (ptr, len));
#endif

  default:
    errno = EIO;
    return -1;
  }
}
// redirect standard output (fd 1) and stderr (fd 2)
extern "C" int _read(int fd, char *ptr, int len) {
  switch(fd){
  case 0:
     return CALL_FUNC2(CONFIG_USART_STDIN, read, (ptr, len));
  default:
    errno = EIO;
    return -1;
  }
}
#endif

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
