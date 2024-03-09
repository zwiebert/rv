/**
 * \file   water_pump/water_pump_logic.h
 * \brief  programming the water pump
 */

#pragma once

#include "user_config.h"
#include "water_pump.h"
#include <time/real_time_clock.h>

#define WPL_MAX_OFF_TIME (ONE_DAY * 2)
#define WPL_MAX_ON_TIME_SHORT (ONE_MINUTE * 2)  ///< enforced on pump run times not caused by us (like water taps)
#define WPL_MAX_ON_TIME_LONG (ONE_HOUR * 2)  ///< enforced on pump run times not caused by us (like water taps)


///   \brief   Do work
void wpl_loop(void);


///   \brief   Select the long MaxOnTime
///
///   Will switch back to short MaxOnTime later by itself
void wpl_increaseMaxOnTime(void);

/// \brief Get current selected MaxOnTime.
time_t wpl_getMaxOnTime(void);


