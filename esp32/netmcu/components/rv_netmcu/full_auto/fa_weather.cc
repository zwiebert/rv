#include "full_auto/weather.hh"

#include "weather/weather_irrigation.hh"
#include "weather/weather_provider_owm.hh"
#include <cstring>

static Weather_Provider_Owm weather_provider;
Weather_Irrigation weather_irrigation(&weather_provider);

static auto &wi = weather_irrigation;

bool fa_poll_weather_full_hour() {
  return wi.fetch_and_store_weather_data();
}

#include <full_auto/single_valve.hh>

int FaContentReader::open(const char *name, const char *query) {
  return 1;
}
int FaContentReader::read(int fd, char *buf, unsigned buf_size) {
  SingleValve sv;
  if (sv.to_json(buf, buf_size))
    return strlen(buf);

  return -1;
}
int FaContentReader::close(int fd) {
  return 0;

}

