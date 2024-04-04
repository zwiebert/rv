#include "full_auto/weather.hh"

#include "weather/weather_irrigation.hh"
#include "weather/weather_provider_owm.hh"
#include <full_auto/automatic_timer.hh>

#include <cstring>

#define DT(x) x

static Weather_Provider_Owm weather_provider;
static Weather_Irrigation weather_irrigation(&weather_provider);
AutoTimer at(&weather_irrigation);
static auto &wi = weather_irrigation;

bool fa_poll_weather_full_hour() {
  return wi.fetch_and_store_weather_data();
}

void fa_loop() {
  at.todo_loop();
}

