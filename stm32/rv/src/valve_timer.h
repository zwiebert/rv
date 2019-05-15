/*
 * timer.h
 *
 *  Created on: 03.05.2019
 *      Author: bertw
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

#define VALVE_TIMER_COUNT 14
#define MAX_TIMER_MINUTES 60
#define MAX_TIME_PER_DAY (60 * 90)



#define TIMER_SET_DONE -1

typedef struct {
	uint32_t start_time;
	uint32_t target_time;
	uint8_t programmed_minutes;
	uint32_t active_time_today;
} valveTimer_T;

extern void (*valveTimer_alarmCb)(int8_t channel);
extern void (*valveTimer_setCb)(int8_t channel);

void valveTimer_loop(void);
uint32_t valveTimer_setTimerDurationByMinutes(uint8_t channel, uint8_t minutes);
uint32_t valveTimer_incrementTimerDuration(uint8_t channel);
uint32_t valveTimer_finishTimer(uint8_t channel);

uint8_t valveTimer_getProgrammedMinutes(uint8_t channel);

void valveTimer_tick(void);
void valveTimer_setup(void);


#endif /* TIMER_H_ */
