#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/f1/gpio.h>
#include <libopencm3/stm32/f1/rcc.h>
#include <peri/i2c.h>
#include <peri/tm1638.h>
#include <peri/uart.h>
#include <peri/mcp23017.h>
#include <water_pump/water_pump.h>
#include <setup/app_cxx.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time/real_time_clock.h>
#include <time/systick_1ms.h>




static void clock_setup(void) {
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
}

static void led_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
            GPIO13);
}

void setup() {
  setenv("TZ", "CET-2", 1); //XXX
  clock_setup();
  systick_setup();
  uart_setup();
  i2c2_setup();
  led_setup();
  rtc_setup();
  tm1638_setup();
  mcp23017_setup(false);
  wp_setup();
  cxx_setup();
#ifdef USE_TEST
  test_setup();
#endif
}
