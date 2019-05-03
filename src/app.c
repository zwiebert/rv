/*
 * app.c
 *
 *  Created on: 29.04.2019
 *      Author: bertw
 */


#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/f1/memorymap.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "mcp23017.h"
#include "rtc.h"
#include "int_macros.h"

#include "../Libraries/tm1638/include/boards/dlb8.h"

 Mcp23017 relay_16;
#define RELAY_ON MCP23017_PORT_PINS_LOW
#define RELAY_OFF MCP23017_PORT_PINS_HIGH

 Tm1638 input1;


 static void input_setup(void) {
	 rcc_periph_clock_enable(RCC_GPIOB);
	 Tm1638_setup(GPIOB, GPIO13, GPIOB, GPIO14);

	 Tm1638_construct(&input1,GPIOB,GPIO15);
 }

 static void i2c2_setup(void)
 {
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

 static void clock_setup(void) {
 	rcc_clock_setup_in_hse_8mhz_out_72mhz();
 }

 static void serialPort_setup(void) {
 	rcc_periph_clock_enable(RCC_GPIOA);
 	rcc_periph_clock_enable(RCC_USART1);
 	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);

 	/* Setup UART parameters. */
 	usart_set_baudrate(USART1, 115200);
 	usart_set_databits(USART1, 8);
 	usart_set_stopbits(USART1, USART_STOPBITS_1);
 	usart_set_mode(USART1, USART_MODE_TX);
 	usart_set_parity(USART1, USART_PARITY_NONE);
 	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

 	/* Finally enable the USART. */
 	usart_enable(USART1);
 }


 static void led_setup(void) {
 	rcc_periph_clock_enable(RCC_GPIOC);
 	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
 }

 static void timer_set(int8_t channel) {
	 static uint16_t set_mask;

	 if (channel >= 0) {
		 set_mask |= 1 << channel;
		 return;
	 }

	 Mcp23017_putBits(&relay_16, set_mask, RELAY_ON);
	 dlb8_put_leds(&input1, GET_LOW_BYTE(set_mask), true);
	 set_mask = 0;
 }

 static void timer_alarm(int8_t channel) {
	 static uint16_t alarm_mask;

	 if (channel >= 0) {
		 alarm_mask |= 1 << channel;
		 return;
	 }

	 Mcp23017_putBits(&relay_16, alarm_mask, RELAY_OFF);
	 dlb8_put_leds(&input1, GET_LOW_BYTE(alarm_mask), false);
	 dlb8_put_chars(&input1, GET_LOW_BYTE(alarm_mask), 'F', true);

	 alarm_mask = 0;
 }

 void setup() {
		clock_setup();
		serialPort_setup();
		i2c2_setup();
		led_setup();
		timer_alarm_cb = timer_alarm;
		timer_set_cb = timer_set;
		rtc_setup();
		input_setup();
 }

 typedef struct {
	 uint8_t channel;
	 uint8_t minutes;
 } timer_args_T;

 timer_args_T ta_buf[16];


void set_timers(timer_args_T *ta, uint8_t ta_len) {
	for (int i=0; i < ta_len; ++i) {
		rtc_set_timer_duration_by_minutes(ta[i].channel, ta[i].minutes);
		if (ta[i].channel < 8) {
			dlb8_put_chars(&input1, (1 << ta[i].channel), '0'+ta[i].minutes, true);
		}
	}
	timer_set(-1);
}

char tas[] = "0:1 3:5";
uint8_t parse_timer_string(char *s, timer_args_T *result) {
	uint8_t i = 0;

	for (char *token = strtok(s, " "); token; token = strtok(NULL, " ")) {
		char *dp = index(token, ':');
		if (dp) {
			*dp = '\0';
			result[i].channel = atoi(token);
			result[i].minutes = atoi(dp + 1);
			*dp = ':';
			++i;
		}
	}
	return i;
}


void app() {
	setup();
	Mcp23017_construct_out(&relay_16, I2C2, Mcp23017_slave_address(0, 0, 0), RELAY_OFF);

	puts("hello");

	rtc_set_timer_duration_by_minutes(9, 1);
	timer_set(-1);
	uint8_t ta_len = parse_timer_string(tas, ta_buf);
	set_timers(ta_buf, ta_len);

	while (1) {
		for (unsigned long i = 0; i < 450000; ++i) {
			__asm__("nop");
		}

		rtc_timer_loop();

		/* Using API function gpio_toggle(): */
		//gpio_toggle(GPIOC, GPIO13); /* LED on/off */
		//Mcp23017_putBit(&relay_16, 1, toggle);
		//Tm1638_put_char(&input1, c++, LED_KEY_POS_TO_REG(5));


		uint8_t button = dlb8_get_changed_buttons(&input1);

		if (button) {
			printf("button: %d\n", button);
			rtc_set_timer_duration_by_minutes(button, 1);
			Tm1638_put_char(&input1, '1', LED_KEY_ADDR_DIGIT(button));
			timer_set(-1);
		} else {

		}
	}
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
