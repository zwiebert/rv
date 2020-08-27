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

#define VALVE_TIMER_COUNT 14

#ifdef __cplusplus
extern "C" {
#endif

void app_switch_valve(int valve_number, bool state);
void app_switch_valves(uint16_t valve_bits, uint16_t valve_mask);

#define RELAY_ON MCP23017_PORT_PINS_LOW
#define RELAY_OFF MCP23017_PORT_PINS_HIGH

extern Mcp23017 relay_16;


 // mask of relays used for valves
#define VALVE_RELAY_MASK (0xffff >> (16 - VALVE_TIMER_COUNT))

 uint16_t valveRelay_getActiveValves();

#ifdef __cplusplus
}
#endif

#endif /* VALVE_RELAYS_H_ */
