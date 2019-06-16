/*
 * water_pump_logic.c
 *
 *  Created on: 12.05.2019
 *      Author: bertw
 */

#include <valve_relays.h>
#include "water_pump_logic.h"
#include "report.h"

static time_t wpl_max_on_time = WPL_MAX_ON_TIME_SHORT;
#define WPL_RESET_MAX_ON_TIME_AFTER (ONE_HOUR)

static bool hasPumpRunTooLong() {
  if (valveRelay_getActiveValves()) {
    if (wp_getPumpOnDuration() > WPL_MAX_ON_TIME_LONG) {
      return true;
    }
  } else if (wp_getPumpOnDuration() > wpl_max_on_time) {
    return true;
  }
  return false;
}

static void checkUserButton() {
  if (wp_isUserButtonPressed()) {
    wpl_increaseMaxOnTime();
  }
}

static bool checkForError() {
  if (wp_getError() != WP_ERR_NONE) {
    if (wp_isUserButtonPressed()) {
      if (wp_getError() == WP_ERR_MAX_ON_TIME)
        wpl_increaseMaxOnTime();

      report_event("wp:error:cleared");
      wp_setError(WP_ERR_NONE);
      return false;
    } else {
      return true;
    }
  }
  return false;
}

static void ifPumpOn() {
  if (!wp_isPressControlOn()) {
    wp_switchPump(false);
  } else if (hasPumpRunTooLong()) {
    wp_setError(WP_ERR_MAX_ON_TIME);
    report_event("wp:error:max_on_time");
  }
}

static void checkResetMaxOnTime() {
  if (wpl_max_on_time != WPL_MAX_ON_TIME_SHORT && wp_getPumpOffDuration() > WPL_RESET_MAX_ON_TIME_AFTER) {
    wpl_max_on_time = WPL_MAX_ON_TIME_SHORT;
  }
}

static void checkRustProtection() {
  if (wp_getPumpOffDuration() > WPL_MAX_OFF_TIME) {
      wp_shortRunPumpForProtection();
  }
}

static void ifPumpOff() {
  checkResetMaxOnTime();

  if (wp_isPressControlOn()) {
    wp_switchPump(true);
  } else {
    checkRustProtection();
  }
}

void wpl_loop(void) {

  if (checkForError())
    return;

  checkUserButton();

  if (wp_isPumpOn()) {
    ifPumpOn();
  } else {
    ifPumpOff();
  }

}

time_t wpl_getMaxOnTime(void) {
  return wpl_max_on_time;
}

void wpl_increaseMaxOnTime(void) {
  wpl_max_on_time = WPL_MAX_ON_TIME_LONG;
}
