#pragma once

#include <mcp23017.h>
#include <stdbool.h>


extern Mcp23017 relay_16;

#define RELAY_ON MCP23017_PORT_PINS_LOW
#define RELAY_OFF MCP23017_PORT_PINS_HIGH

void mcp23017_doReset();
void mcp23017_atFault();
void mcp23017_setup(bool re_init);
