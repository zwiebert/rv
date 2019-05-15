/*
 * water_pump.h
 *
 *  Created on: 12.05.2019
 *      Author: bertw
 */

#ifndef WATER_PUMP_H_
#define WATER_PUMP_H_

#include <stdbool.h>
#include <time.h>

#define WP_MCP_MASK 0x0c

// test if PressControl wants to turn on the pump
bool wp_isPressControlOn(void);

// test if user has pressed the button to increase max-on-time or clear failure state
bool wp_isUserButtonPressed(void);

// test if pump is currently running
bool wp_isPumpOn(void);

// get duration in seconds since the pump was last switched off
time_t wp_getPumpOffDuration(void);

// get duration in seconds since the pump was last switched on
time_t wp_getPumpOnDuration(void);

// switch on/off pump on (and set internal time stamps)
void wp_switchPump(bool on);

// switch pump on for a very short time to prevent rust
void wp_shortRunPumpForProtection(void);

// shortly interrupt PressControl main voltage to clear its Failure state
void wp_clearPcFailure(void);

// error states
typedef enum  { WP_ERR_NONE, WP_ERR_MAX_ON_TIME } wp_err_T;

// set or clear error state and switch off pump if error occurred
void wp_setError(wp_err_T error);

wp_err_T wp_getError(void);


void wp_setup(void);

#endif /* WATER_PUMP_H_ */
