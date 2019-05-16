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
#include <time.h>
#include "mcp23017.h"
#include "rtc.h"
#include "valve_timer.h"
#include "misc/int_macros.h"
#include "water_pump_logic.h"
#include "peri/uart.h"
#include "cli/cli.h"


#include "../Libraries/tm1638/include/boards/dlb8.h"

 Mcp23017 relay_16;
#define RELAY_ON MCP23017_PORT_PINS_LOW
#define RELAY_OFF MCP23017_PORT_PINS_HIGH

 Dlb8 input[2];
 Dlb8 *dlb8_obj[2];

 void loop(void);

#if 1
void display_print_timer(uint8_t n) {
	char buf[8] = " ";
	uint8_t minutes = valveTimer_getProgrammedMinutes(n);
	if (minutes)
		itoa(minutes, buf, 10);
	dlb8_put_chars(dlb8_obj[n < 8 ? 0 : 1], 1 << n, buf[0], minutes >= 10);
}

void display_print_timers() {
	for (int i = 0; i < 16; ++i)
		display_print_timer(i);
}

void dlb8_print_date(Dlb8 *obj, struct tm *tm) {
	uint8_t digit = 0;
	uint8_t mask = 0x01;
	int n;

	n = tm->tm_year + 1900;

	digit = n / 1000;
	n %= 1000;
	dlb8_put_chars(obj, mask, '0'+digit, false);
	mask <<=1;
	digit = n / 100;
	n %= 100;
	dlb8_put_chars(obj, mask, '0'+digit, false);
	mask <<=1;
	digit = n / 10;
	n %= 10;
	dlb8_put_chars(obj, mask, '0'+digit, false);
	mask <<=1;
	digit = n;
	dlb8_put_chars(obj, mask, '0'+digit, false);
	mask <<=1;

	n = tm->tm_mon;
	digit = n > 9 ? n / 10 : 0;
	dlb8_put_chars(obj, mask, '0'+digit, false);
	mask <<=1;
	digit = digit ? n % 10 : n;
	dlb8_put_chars(obj, mask, '0'+digit, false);
	mask <<=1;

	n = tm->tm_mday;
	digit = n > 9 ? n / 10 : 0;
	dlb8_put_chars(obj, mask, '0'+digit, false);
	mask <<=1;
	digit = digit ? n % 10 : n;
	dlb8_put_chars(obj, mask, '0'+digit, false);
	mask <<=1;

}

void dlb8_print_time(Dlb8 *obj, struct tm *tm) {
	uint8_t digit = 0;
	uint8_t mask = 0x01;
	int n;

	n = tm->tm_hour;
	digit = n > 9 ? n / 10 : 0;
	dlb8_put_chars(obj, mask, '0'+digit, false);
	mask <<=1;
	digit = digit ? n % 10 : n;
	dlb8_put_chars(obj, mask, '0'+digit, false);
	mask <<=1;

	n = tm->tm_min;
	digit = n > 9 ? n / 10 : 0;
	dlb8_put_chars(obj, mask, '0'+digit, false);
	mask <<=1;
	digit = digit ? n % 10 : n;
	dlb8_put_chars(obj, mask, '0'+digit, false);
	mask <<=1;

	for (int i=0; i < 4; ++i) {
		dlb8_put_chars(obj, mask, ' ', false);
		mask <<=1;
	}
}
#endif


 static void input_setup(void) {
	 rcc_periph_clock_enable(RCC_GPIOB);
	 Tm1638_setup(GPIOB, GPIO13, GPIOB, GPIO14);
	 Tm1638_construct(&input[0].tm,GPIOB,GPIO15);
	 dlb8_obj[0] = &input[0];
	 Tm1638_construct(&input[1].tm,GPIOB,GPIO12);
	 dlb8_obj[1] = &input[1];
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

 static void led_setup(void) {
 	rcc_periph_clock_enable(RCC_GPIOC);
 	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
 }

void timer_set(int8_t channel) {
	static uint16_t set_mask;

	if (channel >= 0) {
		set_mask |= 1 << channel;
		return;
	}

	Mcp23017_putBits(&relay_16, set_mask, RELAY_ON);
	dlb8_put_leds(dlb8_obj[0], GET_LOW_BYTE(set_mask), true);

	for (int i = 0; i < VALVE_TIMER_COUNT; ++i) {
		if (GET_BIT(set_mask, i)) {
			display_print_timer(i);
		}
	}
	set_mask = 0;
}

 static void timer_alarm(int8_t channel) {
	 static uint16_t alarm_mask;

	 if (channel >= 0) {
		 alarm_mask |= (1 << channel);
		 return;
	 }

	 Mcp23017_putBits(&relay_16, alarm_mask, RELAY_OFF);
	 dlb8_put_leds(dlb8_obj[0], GET_LOW_BYTE(alarm_mask), false);
	 dlb8_put_chars(dlb8_obj[0], GET_LOW_BYTE(alarm_mask), 'F', true);

	 alarm_mask = 0;
 }

 void setup() {
		clock_setup();
		uart_setup();
		i2c2_setup();
		led_setup();
		valveTimer_alarmCb = timer_alarm;
		valveTimer_setCb = timer_set;
		valveTimer_setup();
		rtc_setup();
		input_setup();
		Mcp23017_construct_out(&relay_16, I2C2, Mcp23017_slave_address(0, 0, 0), RELAY_OFF);
		wp_setup();

 }

 typedef struct {
	 uint8_t channel;
	 uint8_t minutes;
 } timer_args_T;

 timer_args_T ta_buf[16];


void set_timers(timer_args_T *ta, uint8_t ta_len) {
	for (int i=0; i < ta_len; ++i) {
		valveTimer_setTimerDurationByMinutes(ta[i].channel, ta[i].minutes);
	}
	timer_set(TIMER_SET_DONE);
}

char tas[] = "3:5";
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
puts("Hello");


	while (1) {
		for (unsigned long i = 0; i < 4500; ++i) {
			__asm__("nop");
		}

		uint8_t button = dlb8_get_changed_buttons(dlb8_obj[0]);

		if (button) {
			printf("pressed button: %x\n", button);
			for (int i = 0; i < 8; ++i) {
				if (GET_BIT(button, i))
					valveTimer_incrementTimerDuration(i);
			}
			timer_set(TIMER_SET_DONE);
		}
		button = dlb8_calculate_hold_buttons(dlb8_obj[0], 200);

		if (button) {
			printf("hold button: %x\n", button);
			for (int i = 0; i < 8; ++i) {
				if (GET_BIT(button, i))
					valveTimer_finishTimer(i);
			}
		}

		if ((button & 0x81) == 0x81) {
			time_t t = time(0);
			struct tm *tm = gmtime(&t);
			printf("%d-%d-%dT%d:%d:%02d\n",tm->tm_year+1900, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
			dlb8_print_date(dlb8_obj[0], tm);
		}

		if ((button & 0x41) == 0x41) {
			time_t t = time(0);
			struct tm *tm = gmtime(&t);
			printf("%d-%d-%dT%d:%d:%02d\n",tm->tm_year+1900, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
			dlb8_print_time(dlb8_obj[0], tm);
		}
		if ((button & 0x21) == 0x21) {
			display_print_timers();

		}
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

void XXuart_loop() {
#if 0
	uint8_t ta_len = parse_timer_string(tas, ta_buf);
	set_timers(ta_buf, ta_len);
#endif
}

void loop(void) {
	valveTimer_loop();
	wpl_loop();
#if 1
	cli_loop();
#else
	char *line = get_commandline();
	if (line) {
		uint8_t ta_len = parse_timer_string(line, ta_buf);
		set_timers(ta_buf, ta_len);
	}
#endif
}

