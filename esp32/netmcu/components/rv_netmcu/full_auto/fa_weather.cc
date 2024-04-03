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

int FaContentReader::open(const char *name, const char *query) {
  int fd = 0;
  auto &fda = m_file_data[fd];
  if (fda.is_open)
    return -1;

  fda.is_open = true;
  weather_irrigation.load_past_weather_data();
//  DT(at.dev_random_fill_data()); // XXX

  return fd;
}
int FaContentReader::read(int fd, char *buf, unsigned buf_size) {
  int n = 0;
  int state = 0;
  if (fd != 0)
    return -1;
  auto &fda = m_file_data[fd];

  if (!fda.is_open)
    return -1;
  if (fda.objects_read == -1)
    return 0; //EOF

  if (fda.objects_read == 0) {
    buf[n++] = '{';
  }

  n += at.to_json(buf + n, buf_size - n, fda.objects_read, state);

  if (state) {
    if (buf[n - 1] == ',')
      --n;
    buf[n++] = '}';
    fda.objects_read = -1;
  }
  assert(n >= 0);
  return n;
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

