#pragma once

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



