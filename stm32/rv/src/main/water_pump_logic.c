/*
 * water_pump_logic.c
 *
 *  Created on: 12.05.2019
 *      Author: bertw
 */


#include "water_pump_logic.h"
#include "valve_relays.h"

static time_t wpl_max_on_time = WPL_MAX_ON_TIME_SHORT;
#define WPL_RESET_MAX_ON_TIME_AFTER (ONE_HOUR)

void wpl_loop(void) {

	if (wp_getError() != WP_ERR_NONE) {
		if (wp_isUserButtonPressed()) {
			if (wp_getError() == WP_ERR_MAX_ON_TIME)
				wpl_increaseMaxOnTime();
			wp_setError(WP_ERR_NONE);

		}
		return;
	}

	if (wp_isUserButtonPressed()) {
		wpl_increaseMaxOnTime();
	}

	if (wp_isPumpOn()) {
		if (!wp_isPressControlOn()) {
			wp_switchPump(false);
		} else if (valveRelay_getActiveValves()) {
			if (wp_getPumpOnDuration() > WPL_MAX_ON_TIME_LONG) {
				wp_setError(WP_ERR_MAX_ON_TIME);
			}
		} else if (wp_getPumpOnDuration() > wpl_max_on_time) {
			wp_setError(WP_ERR_MAX_ON_TIME);
		}
	} else { // pump is off
		if (wpl_max_on_time != WPL_MAX_ON_TIME_SHORT
				&& wp_getPumpOffDuration() > WPL_RESET_MAX_ON_TIME_AFTER) {
			wpl_max_on_time = WPL_MAX_ON_TIME_SHORT;
		}
		if (wp_isPressControlOn()) {
			wp_switchPump(true);
		} else if (wp_getPumpOffDuration() > WPL_MAX_OFF_TIME) {
			wp_shortRunPumpForProtection();
		}
	}

}



time_t wpl_getMaxOnTime(void) {
	return wpl_max_on_time;
}

void wpl_increaseMaxOnTime(void) {
	wpl_max_on_time = WPL_MAX_ON_TIME_LONG;
}
