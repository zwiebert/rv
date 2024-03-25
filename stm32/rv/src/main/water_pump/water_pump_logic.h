/**
 * \file   water_pump/water_pump_logic.h
 * \brief  programming the water pump
 */

#pragma once

#include "user_config.h"
#include "water_pump.h"
#include <time/real_time_clock.h>

#define WPL_MAX_OFF_TIME (ONE_DAY * 2) ///< for rust protection run pump a few seconds if off for this secs
#define WPL_MAX_ON_TIME_SHORT (ONE_MINUTE * 2)  ///< limit pump run time for water taps not controlled by us
#define WPL_MAX_ON_TIME_LONG (ONE_HOUR * 2)  ///< extended limit pump run time for water taps after user button was pressed


/**
 * \brief   Do work. Should be called periodically.
 *
 *    - calls \ref wp_loop
 *    - switches punmp on or off according to related timer and other data
 */
void wpl_loop(void);


///   \brief   Select the long MaxOnTime \ref WPL_MAX_ON_TIME_LONG
///
///   Will switch back to short MaxOnTime \ref WPL_MAX_ON_TIME_SHORT later by itself
void wpl_increaseMaxOnTime(void);

/// \brief Get current selected MaxOnTime.
/// \returns on of \ref WPL_MAX_ON_TIME_LONG or \ref WPL_MAX_ON_TIME_SHORT
time_t wpl_getMaxOnTime(void);


