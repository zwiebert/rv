/*
 * valve_relay.c
 *
 *  Created on: 14.05.2019
 *      Author: bertw
 */


#include "valve_relays.h"

 uint16_t valveRelay_getActiveValves() {
	 uint16_t result = Mcp23017_getBits(&relay_16, VALVE_RELAY_MASK, true);
	 return result;
 }
