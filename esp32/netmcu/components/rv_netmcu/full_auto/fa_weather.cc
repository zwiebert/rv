#pragma once

#include "full_auto/weather.hh"

#include "weather/weather_irrigation.hh"
#include "weather/weather_provider_owm.hh"

static Weather_Provider_Owm weather_provider;
Weather_Irrigation weather_irrigation(&weather_provider);

static auto &wi = weather_irrigation;

bool fa_poll_weather_full_hour() {
  return wi.fetch_and_store_weather_data();
}
