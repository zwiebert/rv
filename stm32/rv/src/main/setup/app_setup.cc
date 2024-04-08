#include "user_config.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/f1/gpio.h>
#include <libopencm3/stm32/f1/rcc.h>
#include <peri/i2c.h>
#include <peri/relay16.h>
#include <peri/uart.h>
#include <water_pump/water_pump.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time/real_time_clock.h>
#include <time/systick_1ms.h>
#ifdef USE_DLB8
#include "peri/dlb8.h"
#endif

#include <rv/rv.hh>
#include <rv/rv_timers.hh>
#include <rv/report.h>
#include <watch_dog/watch_dog.hh>

void* __dso_handle = (void*)NULL;
extern "C" void _fini() {
  while(true);
}

void timers_was_modified(int vn, int tn, bool removed);


static void clock_setup(void) {
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
}

static void led_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
            GPIO13);
}

void app_setup_cxx();

#include <txtio/inout.h>
void cliApp_setup();

void app_setup() {
  setenv("TZ", "CET-2", 1); //XXX
  clock_setup();
  systick_setup();
  uart_setup();

  cliApp_setup();
  io_getc_fun = esp32_getc;
  io_putc_fun = esp32_putc;

  i2c2_setup();
  led_setup();
  rtc_setup();
#ifdef USE_DLB8
  dlb8_setup();
#endif
  relay16_setup(false);
  wp_setup();
#ifdef USE_WDG
    watchDog_setup();
#endif
    rvt.register_callback(timers_was_modified);
#ifdef USE_TEST
  test_setup();
#endif
}
