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
	time_t mOffTime = 0;  ///< the last time the rain sensor turned from on to off
public:
	RainSensor();

	void loop(); ///< Do work

	/// \brief Get current state of the contact.
	/// \return true if rain sensor is wet
	bool getState(time_t delay=0);

	/// \brief Get the last time the rain sensor turned from on to off
	/// \return POSIX time stamp
	time_t getOffTime();

};



