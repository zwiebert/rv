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


#define NO_TIMER_VALUE ~0UL

#define IS_TIMER_RUNNING(t) (t->target_time != NO_TIMER_VALUE)


valveTimer_T timers[VALVE_TIMER_COUNT];

void (*valveTimer_setCb)(int8_t channel);
void (*valveTimer_alarmCb)(int8_t channel);

volatile uint32_t next_timer_value;

volatile bool timer_rang, timer_noticed;
bool timer_finished;

static void rtc_update_timer_table(void);

uint8_t valveTimer_getProgrammedMinutes(uint8_t channel) {
	return timers[channel].programmed_minutes;
}

uint32_t valveTimer_setTimerDurationByMinutes(uint8_t channel, uint8_t minutes) {
	if (!(channel < VALVE_TIMER_COUNT && minutes <= MAX_TIMER_MINUTES && curr_time > 0))
		return 0;


	timers[channel].programmed_minutes = minutes;
	timers[channel].start_time = curr_time;
	uint32_t timer = timers[channel].target_time = minutes * 60 + curr_time;

	if (valveTimer_setCb)
		valveTimer_setCb(channel);

	if (timer < next_timer_value) {
		next_timer_value = timer;
	}
	D(printf("timer set: ch=%u, end=%u, curr=%u, next:%u\n",
			(unsigned)channel, (unsigned)timer, (unsigned)curr_time, (unsigned)next_timer_value));
	return timer;
}

int8_t valve_timer_increments[] = { 0, 5, 7, 10, 20, 30, 40, -1 };
uint32_t valveTimer_incrementTimerDuration(uint8_t channel) {
	valveTimer_T *t = &timers[channel];
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

	if (!IS_TIMER_RUNNING(t))
		t->start_time = curr_time;

	uint32_t timer = !IS_TIMER_RUNNING(t) ? curr_time : t->target_time;
	timer += minutes_to_add * 60;
	t->target_time = timer;
	rtc_update_timer_table();

	if (valveTimer_setCb)
		valveTimer_setCb(channel);


	return timer;
}


void reset_active_times() {  // FIXME: should do  reset at midnight
	static uint32_t day_start_time;

	if (day_start_time + ONE_DAY < curr_time) {
		for (int i = 0; i < VALVE_TIMER_COUNT; ++i) {
			timers[i].active_time_today = 0;
		}
		day_start_time = curr_time;
	}
}

uint32_t valveTimer_finishTimer(uint8_t channel) {
	if (timers[channel].target_time != NO_TIMER_VALUE) {
		timers[channel].target_time = curr_time - 1;
		timer_finished = true;
		return timers[channel].target_time;
	}
	return 0;
}

bool rtc_timer_test() {
	return next_timer_value < curr_time;
}

static void rtc_update_timer_table(void) {
	next_timer_value = NO_TIMER_VALUE;
	bool callback_called = false;

	for (uint8_t i = 0; i < VALVE_TIMER_COUNT; ++i) {
		valveTimer_T *t = &timers[i];

		if (!IS_TIMER_RUNNING(t))
			continue;

		uint32_t run_time = curr_time - t->start_time;

		if (t->target_time < curr_time) {
			valveTimer_alarmCb(i);
			callback_called = true;
			t->target_time = NO_TIMER_VALUE;
			t->programmed_minutes = 0;
			t->active_time_today += run_time;
			D(printf("timer alarm: ch=%u, end=%u, curr=%u\n", (unsigned) i,	(unsigned) t->target_time, (unsigned) curr_time));
		} else if (t->target_time < next_timer_value) {
			next_timer_value = t->target_time;
		}

		if (IS_TIMER_RUNNING(t) && t->active_time_today + run_time > MAX_TIME_PER_DAY) {
			valveTimer_finishTimer(i);
		}
	}
	if (callback_called) {
		valveTimer_alarmCb(-1);
	}
}

void valveTimer_loop(void) {
	if (timer_rang && !timer_noticed) {
		rtc_update_timer_table();
		timer_noticed = true;
	} else if (timer_finished) {
		rtc_update_timer_table();
		timer_finished = false;
	}
	reset_active_times();
}

static void timer_table_setup(void) {
	next_timer_value = NO_TIMER_VALUE;
	for (int i=0; i < VALVE_TIMER_COUNT; ++i) {
	   timers[i].target_time = NO_TIMER_VALUE;
	   timers[i].programmed_minutes = 0;
	   timers[i].active_time_today = 0;
	}
}

/* IRQ */
void valveTimer_tick()
{
	if (timer_rang && timer_noticed) {
		timer_rang = false;
		timer_noticed = false;
	}

	if (next_timer_value < curr_time) {
		timer_rang = true;
	}
}

void valveTimer_setup() {
	timer_table_setup();

}
