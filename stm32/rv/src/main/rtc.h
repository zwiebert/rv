/*
 * rtc.h
 *
 *  Created on: 01.05.2019
 *      Author: bertw
 */

#ifndef RTC_H_
#define RTC_H_

#include <stdint.h>

#define ONE_MINUTE (60)
#define ONE_HOUR (ONE_MINUTE * 60)
#define ONE_DAY (ONE_HOUR * 24)

void rtc_setup(void);

extern volatile uint32_t curr_time;

#endif /* RTC_H_ */
