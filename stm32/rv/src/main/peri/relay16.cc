/**
 * \file   peri/relay16.cc
 * \brief  Control the 16 relay-board (connected via IIC-Expander MCP23017)
 *
 */

#include "user_config.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/f1/memorymap.h>
#include "relay16.h"


Mcp23017 relay_16;

#define DISABLE_ALL_PIN GPIO5       ///< GPIO to enforce switching off all relay in case of a atFault
#define DISABLE_ALL_PIN_PORT GPIOB  ///< GPIO-port for \ref DISABLE_ALL_PIN

void relay16_atFault() {
  gpio_set(DISABLE_ALL_PIN_PORT, DISABLE_ALL_PIN);
}

void relay16_doReset() {
  gpio_set(DISABLE_ALL_PIN_PORT, DISABLE_ALL_PIN);
  for (int i = 0; i < 4500; ++i) {
    __asm__("nop");
  }
  gpio_clear(DISABLE_ALL_PIN_PORT, DISABLE_ALL_PIN);
}

void relay16_setup(bool re_init) {
    if (!re_init) {
        rcc_periph_clock_enable(RCC_GPIOB);
        gpio_clear(DISABLE_ALL_PIN_PORT, DISABLE_ALL_PIN);
        gpio_set_mode(DISABLE_ALL_PIN_PORT, GPIO_MODE_OUTPUT_2_MHZ,
                GPIO_CNF_OUTPUT_PUSHPULL, DISABLE_ALL_PIN);
    }

    relay16_doReset();
    Mcp23017_construct_out(&relay_16, I2C2, Mcp23017_slave_address(0, 0, 0),
            RELAY_OFF);
}




