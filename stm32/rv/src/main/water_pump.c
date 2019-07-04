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
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include "misc/int_macros.h"
#include "report.h"
#include "real_time_clock.h"
#include "systick_1ms.h"
#include "assert.h"
#include "valve_relays.h"

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

static run_time_T last_on_time, last_off_time;
static wp_err_T wp_error;

static bool wp_pc_sample, wp_ub_sample;

// busy-loop wait for N seconds
static void delay_secs(unsigned secs) {
	while (secs--)
		for (unsigned long i = 0; i < 4500000; ++i) {
			__asm__("nop");
		}
}

// switch on/off main voltage of water pump
static void wp_switchPumpRelay(bool on) {
  static bool old_on;
  //bool old_on = !Mcp23017_getBit(&relay_16, WP_RELAY_PIN, true);
  Mcp23017_putBit(&relay_16, WP_RELAY_PIN, !on); // mains relay with NO contact
  if (old_on != on) {
    old_on = on;
    report_pump_status(on);
  }
  assert(on == !Mcp23017_getBit(&relay_16, WP_RELAY_PIN, true));
}

// switch on/off main voltage of PressControl
static void wp_switchPcOutRelay(bool on) {
	Mcp23017_putBit(&relay_16, WP_PCOUT_PIN, !on); // mains relay with NC contact
}

#ifndef USE_PC_POLLING

volatile static int Wp_is_press_control_unchanged;
volatile static bool Wp_is_press_control;

void EXTI0_IRQHandler(void)
{
  Wp_is_press_control_unchanged = 0;
    uint16_t exti_line_state = GPIOB_IDR;

    if (GET_BIT(exti_line_state, 0)) {
      Wp_is_press_control = false;
    } else {
      Wp_is_press_control = true;
    }

    exti_reset_request(EXTI0);
}

static void exti_setup(void)
{


    /* Enable AFIO clock. */
    rcc_periph_clock_enable(RCC_AFIO);

    /* Enable EXTI0 interrupt. */
    nvic_enable_irq(NVIC_EXTI0_IRQ);

    /* Configure the EXTI subsystem. */
    exti_select_source(EXTI0, GPIOB);
    exti_set_trigger(EXTI0, EXTI_TRIGGER_BOTH);
    exti_enable_request(EXTI0);
}
#endif

// test if PressControl wants to turn on the pump
bool wp_isPressControlOn(bool *has_changed) {
#ifdef FAKE_PC
  {
    static bool is_on;

    if (is_on != (valveRelay_getActiveValves() != 0)) {
      if (has_changed) {
        *has_changed = true;
      }
      is_on = !is_on;
    }
    return is_on;
  }

#endif

#ifndef USE_PC_POLLING
  return Wp_is_press_control;
#elif 0
  static bool state;
  if (Wp_is_press_control_unchanged < 2) {
    state = gpio_get(WP_PCIN_PORT, WP_PCIN_PIN) == 0;
    ++Wp_is_press_control_unchanged;
  }

  return state;

#elif 1
  static uint64_t last_change;
  static bool is_on, want_change;
  bool hasChanged = false;
#define PC_DELAY_MS 250
  bool sample_on = !wp_pc_sample;
  if (is_on == sample_on) {
    want_change = false;
  } else if (!want_change) {
    want_change = true;
    last_change = ms_runTime();
  } else if (ms_timeElapsed(&last_change, PC_DELAY_MS)) {
    is_on = sample_on;
    want_change = false;
    hasChanged = true;
  }

  if (has_changed) {
    *has_changed = hasChanged;
  }
  return is_on;
#endif
}

// test if user has pressed the button to increase max-on-time or clear failure state
bool wp_isUserButtonPressed(bool *has_changed) {
#if 0
	static run_time_T last_pressed;
	run_time_T now = runTime();

	if (last_pressed + WP_BUTTON_IGNORE_TIME > now)
		return false;

	bool result = gpio_get(WP_UB_PORT, WP_UB_PIN) == 0;

	if (result)
		last_pressed = now;

	return result;
#else
	  static uint64_t last_change;
	  static bool is_on, want_change;
	  bool hasChanged = false;
	#define PC_DELAY_MS 250
	  bool sample_on = !wp_ub_sample;
	  if (is_on == sample_on) {
	    want_change = false;
	  } else if (!want_change) {
	    want_change = true;
	    last_change = ms_runTime();
	  } else if (ms_timeElapsed(&last_change, PC_DELAY_MS)) {
	    is_on = sample_on;
	    want_change = false;
	    hasChanged = true;
	  }

	  if (has_changed) {
	    *has_changed = hasChanged;
	  }
	  return is_on;
#endif
}

// test if pump is currently running
bool wp_isPumpOn(void) {
	return !Mcp23017_getBit(&relay_16, WP_RELAY_PIN, true);
}

// get duration in seconds since the pump was last switched off
run_time_T wp_getPumpOffDuration(void) {
	if (!wp_isPumpOn()) {
		return runTime() - last_off_time;
	}
	return 0;
}

// get duration in seconds since the pump was last switched on
run_time_T wp_getPumpOnDuration(void) {
  run_time_T result = 0;
	if (wp_isPumpOn()) {
	  result = runTime() - last_on_time;
	}
	return result;
}

// switch on/off pump on (and set internal time stamps)
void wp_switchPump(bool on) {
	if (wp_error != WP_ERR_NONE)
		on = OFF; // force pump off if error

	if (on && !wp_isPumpOn())
		last_on_time = runTime();
	else if (!on && wp_isPumpOn())
		last_off_time = runTime();

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


void wp_loop(void) {
#if (WP_PCIN_PORT != WP_UB_PORT)
#error "pins needs to be on the same port for this function to work"
#endif
  uint16_t portSample = gpio_get(WP_PCIN_PORT, WP_PCIN_PIN | WP_UB_PIN);
  wp_pc_sample = (portSample & WP_PCIN_PIN) != 0;
  wp_ub_sample = (portSample & WP_UB_PIN) != 0;
}




void wp_setup(void) {
	wp_switchPump(OFF);
	wp_switchPcOutRelay(OFF);

 	rcc_periph_clock_enable(RCC_GPIOB);
#ifndef USE_BB
 	gpio_set_mode(WP_PCIN_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, WP_PCIN_PIN);
 	gpio_set_mode(WP_UB_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, WP_UB_PIN);
#else
 	gpio_set_mode(WP_PCIN_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, WP_PCIN_PIN);
 	gpio_set(WP_PCIN_PORT, WP_PCIN_PIN); //pull up
 	gpio_set_mode(WP_UB_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, WP_UB_PIN);
 	gpio_set(WP_UB_PORT, WP_UB_PIN); // pull up
#endif

#ifndef USE_PC_POLLING
 	Wp_is_press_control =  gpio_get(WP_PCIN_PORT, WP_PCIN_PIN) == 0;
 	exti_setup();
#endif
}
