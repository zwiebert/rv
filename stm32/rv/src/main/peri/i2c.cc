#include "user_config.h"
#include "i2c.h"
#include <peri/relay16.h>

#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/f1/gpio.h>
#include <libopencm3/stm32/f1/rcc.h>

bool i2c2_check() {
    if (gpio_get(GPIOB, GPIO_I2C2_SDA))
        return true;
    relay16_setup(true);
    return false;
}

void i2c2_setup(void) {
    /* Enable clocks for I2C2 and AFIO. */
    rcc_periph_clock_enable(RCC_I2C2);
    rcc_periph_clock_enable(RCC_AFIO);
    rcc_periph_clock_enable(RCC_GPIOB);

    /* Set alternate functions for the SCL and SDA pins of I2C2. */
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
    GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
    GPIO_I2C2_SCL | GPIO_I2C2_SDA);

    /* Disable the I2C before changing any configuration. */
    i2c_peripheral_disable(I2C2);

    /* APB1 is running at 36MHz. */
    i2c_set_clock_frequency(I2C2, I2C_CR2_FREQ_36MHZ);

    /* 400KHz - I2C Fast Mode */
    i2c_set_fast_mode(I2C2);

    /*
     * fclock for I2C is 36MHz APB2 -> cycle time 28ns, low time at 400kHz
     * incl trise -> Thigh = 1600ns; CCR = tlow/tcycle = 0x1C,9;
     * Datasheet suggests 0x1e.
     */
    i2c_set_ccr(I2C2, 0x1e);

    /*
     * fclock for I2C is 36MHz -> cycle time 28ns, rise time for
     * 400kHz => 300ns and 100kHz => 1000ns; 300ns/28ns = 10;
     * Incremented by 1 -> 11.
     */
    i2c_set_trise(I2C2, 0x0b);

    /*
     * This is our slave address - needed only if we want to receive from
     * other masters.
     */
    i2c_set_own_7bit_slave_address(I2C2, 0x32);

    /* If everything is configured -> enable the peripheral. */
    i2c_peripheral_enable(I2C2);
}
