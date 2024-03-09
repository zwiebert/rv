/**
 * \file   real_time_clock.h
 * \brief  Provide real time (POSIX stamp) and seconds since startup
 * \author bertw
 */

#ifndef RTC_H_
#define RTC_H_

#include "user_config.h"
#include <stdint.h>
#include <time.h>

#define ONE_MINUTE (60)
#define ONE_HOUR (ONE_MINUTE * 60)
#define ONE_DAY (ONE_HOUR * 24)

#ifdef __cplusplus
extern "C" {
#endif

void rtc_setup(void);

typedef uint32_t run_time_T;


run_time_T runTime();
time_t currentTime();
void rtc_setCurrentTime(time_t t);



#ifndef MCU_HOST
inline run_time_T runTime() {
  extern volatile run_time_T run_time;
  return run_time;
}

inline time_t currentTime() {
  extern volatile time_t curr_time;
  return curr_time;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* RTC_H_ */
