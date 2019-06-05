/*
 * rtc.h
 *
 *  Created on: 01.05.2019
 *      Author: bertw
 */

#ifndef RTC_H_
#define RTC_H_

#include <stdint.h>
#include <time.h>

#define ONE_MINUTE (60)
#define ONE_HOUR (ONE_MINUTE * 60)
#define ONE_DAY (ONE_HOUR * 24)

#ifdef __cplusplus
extern "C" {
#endif

void rtc_setup(void);

extern volatile time_t curr_time;
extern volatile unsigned run_time;

#define runTime() (run_time + 0)

#ifdef __cplusplus
}
#endif

#endif /* RTC_H_ */
