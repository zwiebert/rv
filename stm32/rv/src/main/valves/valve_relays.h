/**
 * \file valves/valve_relays.h
 * \brief Set and get valve-relay states.
 * \author: bertw
 */

#ifndef VALVE_RELAYS_H_
#define VALVE_RELAYS_H_

#include <stdint.h>
#include "peri/relay16.h"

#define VALVE_TIMER_COUNT 14

/// \brief  Open/close single valve
void app_switch_valve(int valve_number, bool state);

/// \brief  Open/close multiple valves
void app_switch_valves(uint16_t valve_bits, uint16_t valve_mask);

/// \brief  mask of relays used for valves
#define VALVE_RELAY_MASK (0xffff >> (16 - VALVE_TIMER_COUNT))


/// \brief   Get currently open valves.
/// \return  bit-mask. each open valve is indicated by its related bit set to 1
uint16_t valveRelay_getActiveValves();


#endif /* VALVE_RELAYS_H_ */
