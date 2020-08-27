/*
 * water_pump_logic.c
 *
 *  Created on: 12.05.2019
 *      Author: bertw
 */

#include <rv/valve_relays.h>
#include "rv/water_pump_logic.h"
#include "time/real_time_clock.h"
#include "rv/report.h"

static time_t wpl_max_on_time = WPL_MAX_ON_TIME_SHORT;
#define WPL_RESET_MAX_ON_TIME_AFTER (ONE_HOUR)
static uint32_t wpl_increaseMaxOnTimeTime;
static bool maybePcFailure;

#define VALVE_ACTIVE_DELAY 20
static bool areValvesActive() {
  static time_t last_time;
  time_t now = runTime();

  if (valveRelay_getActiveValves()) {
    last_time = now;
    return true;
  }

  if (last_time + VALVE_ACTIVE_DELAY > now) {
    return true;
  }

  return false;

}

static bool hasPumpRunTooLong() {
  if (areValvesActive()) {
    if (wp_getPumpOnDuration() > WPL_MAX_ON_TIME_LONG) {
      return true;
    }
  } else if (wp_getPumpOnDuration() > wpl_max_on_time) {
    return true;
  }
  return false;
}

static void checkUserButton() {
  bool hasChanged = false;
  if (wp_isUserButtonPressed(&hasChanged) && hasChanged) {
    wpl_increaseMaxOnTime();
  }
  if (maybePcFailure) {
    wp_clearPcFailure();
    maybePcFailure = false;
  }
}

static bool checkForError() {
  if (wp_getError() != WP_ERR_NONE) {
    if (wp_isUserButtonPressed(0)) {
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
  if (!wp_isPressControlOn(0)) {
    wp_switchPump(false);
  } else if (hasPumpRunTooLong()) {
    wp_setError(WP_ERR_MAX_ON_TIME);
    report_event("wp:error:max_on_time");
  }
}

static void checkResetMaxOnTime() {
  if (wpl_max_on_time != WPL_MAX_ON_TIME_SHORT
      && wpl_increaseMaxOnTimeTime + runTime() > WPL_RESET_MAX_ON_TIME_AFTER
      && wp_getPumpOffDuration() > WPL_RESET_MAX_ON_TIME_AFTER) {
    report_event("wp:max_on_time:change_to_short");
    wpl_max_on_time = WPL_MAX_ON_TIME_SHORT;
  }
}

static void checkRustProtection() {
  if (wp_getPumpOffDuration() > WPL_MAX_OFF_TIME) {
      report_event("wp:rust_protection:run");
      wp_shortRunPumpForProtection();
  }
}

static void ifPumpOff() {
  checkResetMaxOnTime();
  bool hasChanged;

  if (wp_isPressControlOn(&hasChanged)) {
    wp_switchPump(true);
  } else {
    checkRustProtection();
    if (hasChanged && areValvesActive())
      maybePcFailure = true;
  }
}

void wpl_loop(void) {
  wp_loop();

  if (checkForError())
    return;

  checkUserButton();

  if (wp_isPumpOn()) {
    maybePcFailure = false;
    ifPumpOn();
  } else {
    ifPumpOff();
  }

}

time_t wpl_getMaxOnTime(void) {
  return wpl_max_on_time;
}

void wpl_increaseMaxOnTime(void) {
  if (wpl_max_on_time != WPL_MAX_ON_TIME_LONG) {
    report_event("wp:max_on_time:change_to_long");
  }
  wpl_max_on_time = WPL_MAX_ON_TIME_LONG;
  wpl_increaseMaxOnTimeTime = runTime();
}
