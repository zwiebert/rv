/*
 * timer.c
 *
 *  Created on: 03.05.2019
 *      Author: bertw
 */

#include "valve_timer.h"
#include "rtc.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/cm3/nvic.h>

#include <stdio.h>

#define OUTPUT_PIN_COUNT 16
#define MAX_TIMER_MINUTES 60
#define NO_TIMER_VALUE ~0UL

uint32_t timers[OUTPUT_PIN_COUNT];
void (*valve_timer_set_cb)(int8_t channel);
void (*valve_timer_alarm_cb)(int8_t channel);

volatile uint32_t next_timer_value;

volatile bool timer_rang, timer_noticed;

uint32_t valve_timer_set_timer_duration_by_minutes(uint8_t channel, uint8_t minutes) {
	if (!(channel < OUTPUT_PIN_COUNT && minutes <= MAX_TIMER_MINUTES && curr_time > 0))
		return 0;
	if (valve_timer_set_cb)
		valve_timer_set_cb(channel);

	uint32_t timer = timers[channel] = minutes * 60 + curr_time;

	if (timer < next_timer_value) {
		next_timer_value = timer;
	}
	printf("timer set: ch=%u, end=%u, curr=%u, next:%u\n",
			(unsigned)channel, (unsigned)timer, (unsigned)curr_time, (unsigned)next_timer_value);
	return timer;
}

bool rtc_timer_test() {
	return next_timer_value < curr_time;
}

void rtc_update_timer_table(void) {
	next_timer_value = NO_TIMER_VALUE;
	bool callback_called = false;

	for (uint8_t i = 0; i < OUTPUT_PIN_COUNT; ++i) {
		uint32_t timer = timers[i];
		if (timer < curr_time) {
			timers[i] = NO_TIMER_VALUE;
			printf("timer alarm: ch=%u, end=%u, curr=%u\n", (unsigned) i,
					(unsigned) timer, (unsigned) curr_time);
			valve_timer_alarm_cb(i);
			callback_called = true;

		} else if (timer < next_timer_value) {
			next_timer_value = timer;
		}
	}
	if (callback_called) {
		valve_timer_alarm_cb(-1);
	}
}

void valve_timer_loop(void) {
	if (timer_rang && !timer_noticed) {
		rtc_update_timer_table();
		timer_noticed = true;
	}
}

static void timer_table_setup(void) {
	next_timer_value = NO_TIMER_VALUE;
	for (int i=0; i < OUTPUT_PIN_COUNT; ++i) {
	   timers[i] = NO_TIMER_VALUE;
	}
}

/* IRQ */
void valve_timer_tick()
{
	if (timer_rang && timer_noticed) {
		timer_rang = false;
		timer_noticed = false;
	}

	if (next_timer_value < curr_time) {
		timer_rang = true;
	}
}

void valve_timer_setup() {
	timer_table_setup();

}
