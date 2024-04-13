#pragma once

struct stm32com_timer_args {
    unsigned off_duration_secs = 0;
    unsigned repeats = 0;
    unsigned period_secs = 0;
    int dInterval = 0;
    int dhBegin = 0;
    int dhEnd = 0;
    bool ignoreRainSensor = false;
};
void stm32com_ask_version();
void stm32com_ask_value(const char *key);
void stm32com_duration(unsigned zone, unsigned timer_number, unsigned on_duration_secs);
void stm32com_duration(unsigned zone, unsigned timer_number, unsigned on_duration_secs, const stm32com_timer_args &args);
