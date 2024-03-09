/**
 * \file    rv/rain_sensor.hh
 * \brief   interface to rain sensor device (cork contact)
 * \author  bertw
 */

#pragma once

#include "user_config.h"
#include <time.h>


/**
 * \brief Rain sensor cork contact
 */
class RainSensor {
	time_t mOffTime = 0;
public:
	RainSensor();

	void loop(); ///< Do work

	/// \brief Get state of the contact.
	/// \return true if rain sensor is wet
	bool getState(time_t delay=0);

	/// \brief Get off duration
	/// \return How long the rain sensor was now dry, which is the time since last beging we.)
	time_t getOffTime();

};



