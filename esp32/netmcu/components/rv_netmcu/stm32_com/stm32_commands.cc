#include <stm32_com/stm32_commands.hh>

#include <stm32/stm32.h>

#include <cstdio>
#include <ctime>
#include <cassert>


void stm32com_ask_version() {
  const char json[] = "\r\n{\"cmd\":{\"version\":\"?\"}};\r\n";
  stm32_write(json, sizeof json);
}

void stm32com_ask_value(const char *key) {
  char buf[128];
  int n = snprintf(buf, sizeof buf, "\r\n{\"cmd\":{\"%s\":\"?\"}};\r\n", key);
  assert (0 < n && n < sizeof buf);
  stm32_write(buf, n + 1);
}

void stm32com_duration(unsigned zone, unsigned timer_number, unsigned on_duration_secs) {
  char buf[128];
  int n = snprintf(buf, sizeof buf, "\r\n{\"cmd\":{\"dur%u.%u\":\"%u\"}};\r\n",
      zone, timer_number,
      on_duration_secs);
  assert (0 < n && n < sizeof buf);
  stm32_write(buf, n + 1);
}

void stm32com_duration(unsigned zone, unsigned timer_number, unsigned on_duration_secs, const stm32com_timer_args &args) {
  char buf[128];
  int n = snprintf(buf, sizeof buf, "\r\n{\"cmd\":{\"dur%u.%u\":\"%u,%u,%u,%u,%u,%d,%d,%d\"}};\r\n",
      zone, timer_number,
      on_duration_secs,
      args.ignoreRainSensor,
      args.off_duration_secs,
      args.repeats,
      args.period_secs,
      args.dInterval,
      args.dhBegin,
      args.dhEnd);
  assert (0 < n && n < sizeof buf);
  stm32_write(buf, n + 1);
}

void stm32com_send_time(void) {
  char buf[80];
  int n = snprintf(buf, sizeof buf, "\r\n{\"config\":{\"time\":%lld}};\r\n", (long long)time(0));
  assert (0 < n && n < sizeof buf);
  stm32_write(buf, n + 1);
}
