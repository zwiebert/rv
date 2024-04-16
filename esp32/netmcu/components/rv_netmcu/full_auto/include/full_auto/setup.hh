#pragma once
#include <weather/weather_irrigation.hh>
#include <full_auto/automatic_timer.hh>
#include <full_auto/weather.hh>

extern AutoTimer *auto_timer;
extern Weather_Irrigation *weather_irrigation;

extern FullAuto *full_auto;


struct fa_cfg {

};

void fa_setup(fa_cfg *cfg);
