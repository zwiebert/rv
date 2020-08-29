#pragma once

#include "../../../Libraries/tm1638/include/boards/dlb8.h"

extern Dlb8 input[2];
extern Dlb8 *dlb8_obj[2];

struct tm;

void dlb8_print_time(Dlb8 *obj, struct tm *tm);
void dlb8_print_date(Dlb8 *obj, struct tm *tm);
void dlb8_loop();
void dlb8_setup(void);
