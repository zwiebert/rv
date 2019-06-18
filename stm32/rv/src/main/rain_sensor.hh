/*
 * rain_sensor.h
 *
 *  Created on: 07.06.2019
 *      Author: bertw
 */

#ifndef MAIN_RAIN_SENSOR_HH_
#define MAIN_RAIN_SENSOR_HH_

#include "user_config.h"
#include <time.h>



class RainSensor {
	time_t mOffTime = 0;
public:
	RainSensor();
	void loop();
	bool getState(time_t delay=0);
	time_t getOffTime();



};


#endif /* MAIN_RAIN_SENSOR_HH_ */
