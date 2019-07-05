/*
 * systick_1ms.h
 *
 *  Created on: 19.06.2019
 *      Author: bertw
 */

#ifndef MAIN_SYSTICK_1MS_H_
#define MAIN_SYSTICK_1MS_H_



#ifdef __cplusplus
extern "C" {
#endif

extern volatile uint64_t run_time_ms;


uint64_t ms_runTime(void);
bool ms_timeElapsed(uint64_t *last, int diff);



#define ms_timeMask(mask) ((run_time_ms & (mask)))
#define ms_timePulse(exp) (!ms_timeMask((1<<(exp))-1))

void systick_setup(void);

#ifdef __cplusplus
}
#endif



#endif /* MAIN_SYSTICK_1MS_H_ */
