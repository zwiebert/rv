/*
 * valve_relay.h
 *
 *  Created on: 14.05.2019
 *      Author: bertw
 */

#ifndef VALVE_RELAYS_H_
#define VALVE_RELAYS_H_

#include <stdint.h>
#include "mcp23017.h"
#include "valve_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

extern Mcp23017 relay_16;

 // mask of relays used for valves
#define VALVE_RELAY_MASK (0x0f >> (16 - VALVE_TIMER_COUNT))

 uint16_t valveRelay_getActiveValves();

#ifdef __cplusplus
}
#endif

#endif /* VALVE_RELAYS_H_ */
