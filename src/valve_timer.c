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

#define D(x) x
#define OUTPUT_PIN_COUNT 16
#define MAX_TIMER_MINUTES 60
#define NO_TIMER_VALUE ~0UL



valve_timer_T timers[OUTPUT_PIN_COUNT];

void (*valve_timer_set_cb)(int8_t channel);
void (*valve_timer_alarm_cb)(int8_t channel);

volatile uint32_t next_timer_value;

volatile bool timer_rang, timer_noticed;

static void rtc_update_timer_table(void);

uint8_t valve_timer_get_programmed_minutes(uint8_t channel) {
	return timers[channel].programmed_minutes;
}

uint32_t valve_timer_set_timer_duration_by_minutes(uint8_t channel, uint8_t minutes) {
	if (!(channel < OUTPUT_PIN_COUNT && minutes <= MAX_TIMER_MINUTES && curr_time > 0))
		return 0;


	timers[channel].programmed_minutes = minutes;
	uint32_t timer = timers[channel].target_time = minutes * 60 + curr_time;

	if (valve_timer_set_cb)
		valve_timer_set_cb(channel);

	if (timer < next_timer_value) {
		next_timer_value = timer;
	}
	D(printf("timer set: ch=%u, end=%u, curr=%u, next:%u\n",
			(unsigned)channel, (unsigned)timer, (unsigned)curr_time, (unsigned)next_timer_value));
	return timer;
}

int8_t valve_timer_increments[] = { 0, 5, 7, 10, 20, 30, 40, -1 };
uint32_t valve_timer_increment_timer_duration(uint8_t channel) {
	valve_timer_T *t = &timers[channel];
	int minutes_to_add = -1;

	for (int i = 0; valve_timer_increments[i] >= 0; ++i) {
		int8_t vti = valve_timer_increments[i];
		if (vti <= t->programmed_minutes)
			continue;

		minutes_to_add = vti - t->programmed_minutes;
		t->programmed_minutes = vti;
		break;
	}

	if (minutes_to_add < 0)
		return 0;


	uint32_t timer = t->target_time == NO_TIMER_VALUE ? curr_time : t->target_time;
	timer += minutes_to_add * 60;
	t->target_time = timer;
	rtc_update_timer_table();

	if (valve_timer_set_cb)
		valve_timer_set_cb(channel);


	return timer;
}

bool rtc_timer_test() {
	return next_timer_value < curr_time;
}

static void rtc_update_timer_table(void) {
	next_timer_value = NO_TIMER_VALUE;
	bool callback_called = false;

	for (uint8_t i = 0; i < OUTPUT_PIN_COUNT; ++i) {
		uint32_t timer = timers[i].target_time;
		if (timer < curr_time) {
			valve_timer_alarm_cb(i);
			callback_called = true;
			timers[i].target_time = NO_TIMER_VALUE;
			timers[i].programmed_minutes = 0;
			D(printf("timer alarm: ch=%u, end=%u, curr=%u\n", (unsigned) i,
					(unsigned) timer, (unsigned) curr_time));


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
	   timers[i].target_time = NO_TIMER_VALUE;
	   timers[i].programmed_minutes = 0;
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
