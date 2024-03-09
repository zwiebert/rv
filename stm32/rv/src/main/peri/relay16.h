/**
 * \file   peri/relay16.h
 * \brief  Control the 16 Relais Board (connected via IIC-Expander MCP23017)
 *
 */

#pragma once

#include "user_config.h"
#include <mcp23017.h>
#include <stdbool.h>


extern Mcp23017 relay_16;

#define RELAY_ON MCP23017_PORT_PINS_LOW   ///< map pin level to relay current
#define RELAY_OFF MCP23017_PORT_PINS_HIGH ///< map pin level to relay current

/// \brief  Clear and disable all GPIO pin of MCP23017
void relay16_doReset();

/// \brief  Should be called by AtFault()-Handler. Will turn off all relays.
void relay16_atFault();

///  \brief Set up or re-init this module at startup.
void relay16_setup(bool re_init);
