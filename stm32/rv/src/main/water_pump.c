/*
 * water_pump.c
 *
 *  Created on: 12.05.2019
 *      Author: bertw
 */


#include "water_pump.h"
#include "mcp23017.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

extern Mcp23017 relay_16;
#define WP_RELAY_PIN 15  // on IO expander
#define WP_PCOUT_PIN 14  // on IO expander
#define ON true
#define OFF false

#define WP_PCIN_PORT GPIO_PORT_B_BASE
#define WP_PCIN_PIN  GPIO0
#define WP_UB_PORT GPIO_PORT_B_BASE
#define WP_UB_PIN  GPIO1

#define WP_BUTTON_IGNORE_TIME 10
#define WP_PC_HOLD_TIME 2
#define WP_RUST_PROTECTION_RUN_TIME 2
#define WP_PC_CLEAR_FAILURE_TIME 5

static time_t last_on_time, last_off_time;
static wp_err_T wp_error;

// busy-loop wait for N seconds
static void delay_secs(unsigned secs) {
	while (secs--)
		for (unsigned long i = 0; i < 4500000; ++i) {
			__asm__("nop");
		}
}

// switch on/off main voltage of water pump
static void wp_switchPumpRelay(bool on) {
	Mcp23017_putBit(&relay_16, WP_RELAY_PIN, !on); // mains relay with NO contact
}

// switch on/off main voltage of PressControl
static void wp_switchPcOutRelay(bool on) {
	Mcp23017_putBit(&relay_16, WP_PCOUT_PIN, !on); // mains relay with NC contact
}

// test if PressControl wants to turn on the pump
bool wp_isPressControlOn(void) {
	static time_t last_pressed;
	time_t now = time(0);
	if (last_pressed + WP_PC_HOLD_TIME > now)
		return true;

	bool result = gpio_get(WP_PCIN_PORT, WP_PCIN_PIN) == 0;

	if (result)
		last_pressed = now;

	return result;
}

// test if user has pressed the button to increase max-on-time or clear failure state
bool wp_isUserButtonPressed(void) {
	static time_t last_pressed;
	time_t now = time(0);

	if (last_pressed + WP_BUTTON_IGNORE_TIME > now)
		return false;

	bool result = gpio_get(WP_UB_PORT, WP_UB_PIN) == 0;

	if (result)
		last_pressed = now;

	return result;
}

// test if pump is currently running
bool wp_isPumpOn(void) {
	return !Mcp23017_getBit(&relay_16, WP_RELAY_PIN, true);
}

// get duration in seconds since the pump was last switched off
time_t wp_getPumpOffDuration(void) {
	if (!wp_isPumpOn() && last_off_time) { //XXX: remove test for current state?
		return time(0) - last_off_time;
	}
	return 0;
}

// get duration in seconds since the pump was last switched on
time_t wp_getPumpOnDuration(void) {
	if (wp_isPumpOn() && last_on_time) { //XXX: remove test for current state?
		return time(0) - last_on_time;
	}
	return 0;
}

// switch on/off pump on (and set internal time stamps)
void wp_switchPump(bool on) {
	if (wp_error != WP_ERR_NONE)
		on = OFF; // force pump off if error

	if (on && !wp_isPumpOn())
		last_on_time = time(0);
	else if (!on && wp_isPumpOn())
		last_off_time = time(0);

	wp_switchPumpRelay(on);
}

// set or clear error state and switch off pump if error occurred
void wp_setError(wp_err_T error) {
	if (error == WP_ERR_NONE) {
		wp_clearPcFailure();
	} else {
		wp_switchPump(OFF);
	}
	wp_error = error;
}

wp_err_T wp_getError(void) {
	return wp_error;
}

// switch pump on for a very short time to prevent rust
void wp_shortRunPumpForProtection(void) {
	wp_switchPump(ON);
	delay_secs(WP_RUST_PROTECTION_RUN_TIME);
	wp_switchPump(OFF);
}

// shortly interrupt PressControl main voltage to clear its Failure state
void wp_clearPcFailure(void) {
	wp_switchPcOutRelay(ON); // mains relay NC contacts
	delay_secs(WP_PC_CLEAR_FAILURE_TIME);
	wp_switchPcOutRelay(OFF);
}

void wp_setup(void) {
	wp_switchPump(OFF);
	wp_switchPcOutRelay(OFF);

 	rcc_periph_clock_enable(RCC_GPIOB);
#if 1
 	gpio_set_mode(WP_PCIN_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, WP_PCIN_PIN);
 	gpio_set_mode(WP_UB_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, WP_UB_PIN);
#else
 	gpio_set_mode(WP_PCIN_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, WP_PCIN_PIN);
 	gpio_set(WP_PCIN_PORT, WP_PCIN_PIN); //pull up
 	gpio_set_mode(WP_UB_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, WP_UB_PIN);
 	gpio_set(WP_UB_PORT, WP_UB_PIN); // pull up
#endif
}
