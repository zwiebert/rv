#pragma once

#include "user_config.h"
#include "water_pump.h"
#include <time/real_time_clock.h>

#define WPL_MAX_OFF_TIME (ONE_DAY * 2)
#define WPL_MAX_ON_TIME_SHORT (ONE_MINUTE * 2)
#define WPL_MAX_ON_TIME_LONG (ONE_HOUR * 2)

void wpl_loop(void);

void wpl_increaseMaxOnTime(void);
time_t wpl_getMaxOnTime(void);


