#include "../../../Libraries/tm1638/include/tm1638.h"
#include "dlb8.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/f1/memorymap.h>

void tm1638_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOB);
    Tm1638_setup(GPIOB, GPIO13, GPIOB, GPIO14);
    Tm1638_construct(&input[0].tm, GPIOB, GPIO15);
    dlb8_obj[0] = &input[0];
    Tm1638_construct(&input[1].tm, GPIOB, GPIO12);
    dlb8_obj[1] = &input[1];
}



