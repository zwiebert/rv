/*
 * water_pump_logic.h
 *
 *  Created on: 12.05.2019
 *      Author: bertw
 */

#ifndef WATER_PUMP_LOGIC_H_
#define WATER_PUMP_LOGIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "user_config.h"
#include "water_pump.h"
#include <real_time_clock.h>

#define WPL_MAX_OFF_TIME (ONE_DAY * 14)
#define WPL_MAX_ON_TIME_SHORT (ONE_MINUTE * 1)
#define WPL_MAX_ON_TIME_LONG (ONE_HOUR * 2)

void wpl_loop(void);

void wpl_increaseMaxOnTime(void);
time_t wpl_getMaxOnTime(void);



#ifdef __cplusplus
}
#endif


#endif /* WATER_PUMP_LOGIC_H_ */
