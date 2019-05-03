/*
 * timer.h
 *
 *  Created on: 03.05.2019
 *      Author: bertw
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

extern void (*valve_timer_alarm_cb)(int8_t channel);
extern void (*valve_timer_set_cb)(int8_t channel);

void valve_timer_loop(void);
uint32_t valve_timer_set_timer_duration_by_minutes(uint8_t channel, uint8_t minutes);

void valve_timer_tick(void);
void valve_timer_setup(void);

#endif /* TIMER_H_ */
