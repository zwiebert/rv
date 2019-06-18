/*
 * rain_sensor.c
 *
 *  Created on: 07.06.2019
 *      Author: bertw
 */

#include <rain_sensor.hh>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "peri/uart.h"

#define RAIN_SENSOR_RCC RCC_GPIOB
#define RAIN_SENSOR_PORT GPIOB
#define RAIN_SENSOR_PIN GPIO8



time_t RainSensorOffTime;


RainSensor::RainSensor() {
 	rcc_periph_clock_enable(RAIN_SENSOR_RCC);
#ifndef USE_BB
 	gpio_set_mode(RAIN_SENSOR_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, RAIN_SENSOR_PIN);
#else
 	gpio_set_mode(RAIN_SENSOR_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, RAIN_SENSOR_PIN);
 	gpio_set(RAIN_SENSOR_PORT, RAIN_SENSOR_PIN);
#endif
}


#define WP_RAIN_SENSOR_DEBOUNCE_SECS 15
static bool rainSensor_isRainSensorActive(void) {
    static time_t last_pressed, last_released;
    time_t now = time(0);



    if (last_released <= last_pressed && last_pressed + WP_RAIN_SENSOR_DEBOUNCE_SECS > now)
        return false;

    if (last_pressed <= last_released && last_released + WP_RAIN_SENSOR_DEBOUNCE_SECS > now)
        return true;

    bool result = gpio_get(RAIN_SENSOR_PORT, RAIN_SENSOR_PIN);  // rain-sensor contact is normally closed (dry)
    if (result)
        last_pressed = now;
    else
        last_released = now;

    return result;
}

bool RainSensor::getState(time_t delay) {
	bool result = false;
	if (!delay)
	    result = gpio_get(RAIN_SENSOR_PORT, RAIN_SENSOR_PIN); //rainSensor_isRainSensorActive();
	else
		result = mOffTime + delay < time(0);

	return result;
}

void RainSensor::loop(void) {
  static bool old_state;
  bool state = getState();

  if (old_state != state) {
    esp32_puts(state ? "status rain=1;" : "status rain=0;");
    old_state = state;
  }

	if (state) {
		mOffTime = time(0);
	}
}

time_t RainSensor::getOffTime() {
	return mOffTime;
}

