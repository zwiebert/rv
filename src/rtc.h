/*
 * rtc.h
 *
 *  Created on: 01.05.2019
 *      Author: bertw
 */

#ifndef RTC_H_
#define RTC_H_


void rtc_setup(void);

extern void (*timer_alarm_cb)(int8_t channel);
extern void (*timer_set_cb)(int8_t channel);
void rtc_timer_loop(void);
uint32_t rtc_set_timer_duration_by_minutes(uint8_t channel, uint8_t minutes);

#endif /* RTC_H_ */
