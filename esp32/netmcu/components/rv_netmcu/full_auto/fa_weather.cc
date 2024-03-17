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
#include <full_auto/automatic_timer.hh>


static AutoTimer at;

int FaContentReader::open(const char *name, const char *query) {
  int fd = 0;
  auto &fda = m_file_data[fd];
  if (fda.is_open)
    return -1;

  fda.is_open = true;

  return fd;
}
int FaContentReader::read(int fd, char *buf, unsigned buf_size) {
  if (fd != 0)
    return -1;
  auto &fda = m_file_data[fd];

  if (!fda.is_open)
    return -1;

  return at.to_json(buf, buf_size, fda.objects_read);
}
int FaContentReader::close(int fd) {
  if (fd != 0)
    return -1;
  auto &fda = m_file_data[fd];

  if (!fda.is_open)
    return -1;

  fda = FileData();
  return 0;
}

