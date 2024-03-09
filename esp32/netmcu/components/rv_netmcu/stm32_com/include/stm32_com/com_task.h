#pragma once


#include <stdbool.h>

struct cfg_stm32com { bool enable; };

void stm32com_setup_task(const struct cfg_stm32com *cfg_stm32com);


