#pragma once

#include "user_config.h"
#include <mcp23017.h>
#include <stdbool.h>


extern Mcp23017 relay_16;

#define RELAY_ON MCP23017_PORT_PINS_LOW
#define RELAY_OFF MCP23017_PORT_PINS_HIGH

void relay16_doReset();
void relay16_atFault();
void relay16_setup(bool re_init);
