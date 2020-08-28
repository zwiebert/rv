#pragma once

#include <stdbool.h>

void mcp23017_doReset();
void mcp23017_atFault();
void mcp23017_setup(bool re_init);
