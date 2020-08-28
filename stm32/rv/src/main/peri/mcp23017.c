#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/f1/memorymap.h>
#include <mcp23017.h>
#include <rv/valve_relays.h>


Mcp23017 relay_16;

#define DISABLE_ALL_PIN GPIO5
#define DISABLE_ALL_PIN_PORT GPIOB

void mcp23017_atFault() {
  gpio_set(DISABLE_ALL_PIN_PORT, DISABLE_ALL_PIN);
}

void mcp23017_doReset() {
  gpio_set(DISABLE_ALL_PIN_PORT, DISABLE_ALL_PIN);
  for (int i = 0; i < 4500; ++i) {
    __asm__("nop");
  }
  gpio_clear(DISABLE_ALL_PIN_PORT, DISABLE_ALL_PIN);
}

void mcp23017_setup(bool re_init) {
    if (!re_init) {
        rcc_periph_clock_enable(RCC_GPIOB);
        gpio_clear(DISABLE_ALL_PIN_PORT, DISABLE_ALL_PIN);
        gpio_set_mode(DISABLE_ALL_PIN_PORT, GPIO_MODE_OUTPUT_2_MHZ,
                GPIO_CNF_OUTPUT_PUSHPULL, DISABLE_ALL_PIN);
    }

    mcp23017_doReset();
    Mcp23017_construct_out(&relay_16, I2C2, Mcp23017_slave_address(0, 0, 0),
            RELAY_OFF);
}




