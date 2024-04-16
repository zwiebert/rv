#include "full_auto/weather.hh"
#include <full_auto/setup.hh>
#include "weather/weather_irrigation.hh"
#include "weather/weather_provider_owm.hh"
#include <full_auto/automatic_timer.hh>

#include <cstring>

#define DT(x) x

bool fa_poll_weather_full_hour() {
  return full_auto->weather_irrigation().fetch_and_store_weather_data();
}

void fa_loop() {
  full_auto->auto_timer().todo_loop();
}

