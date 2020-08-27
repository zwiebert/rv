/*
 * rain_sensor.c
 *
 *  Created on: 07.06.2019
 *      Author: bertw
 */

#include <rv/rain_sensor.hh>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "peri/uart.h"

#define RAIN_SENSOR_RCC RCC_GPIOB
#define RAIN_SENSOR_PORT GPIOB
#define RAIN_SENSOR_PIN GPIO8

#ifdef USE_TEST
bool rs_forceRain;
#endif

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

bool RainSensor::getState(time_t delay) {
#ifdef USE_TEST
  if (rs_forceRain)
    return true;
#endif

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

