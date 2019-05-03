/*
 * timer.h
 *
 *  Created on: 03.05.2019
 *      Author: bertw
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

#define TIMER_SET_DONE -1

typedef struct {
	uint32_t start_time;
	uint32_t target_time;
	uint8_t programmed_minutes;
	uint32_t active_time_today;
} valve_timer_T;

extern void (*valve_timer_alarm_cb)(int8_t channel);
extern void (*valve_timer_set_cb)(int8_t channel);

void valve_timer_loop(void);
uint32_t valve_timer_set_timer_duration_by_minutes(uint8_t channel, uint8_t minutes);
uint32_t valve_timer_increment_timer_duration(uint8_t channel);
uint32_t valve_timer_finish_timer(uint8_t channel);

uint8_t valve_timer_get_programmed_minutes(uint8_t channel);

void valve_timer_tick(void);
void valve_timer_setup(void);

#endif /* TIMER_H_ */
