#include <full_auto/automatic_timer.hh>
#include <app_settings/config.h>
#include <stm32_com/stm32_commands.hh>
#include <kvs/kvs_wrapper.h>
#include <utils_misc/sun.h>
#include <utils_time/ut_constants.hh>
#include <time.h>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <debug/dbg.h>
#include <debug/log.h>
#ifdef CONFIG_RV_NETMCU_DEBUG
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "rv.full_auto"

namespace app::fa {
static constexpr char kvs_name[] = "full_auto";



AutoTimerData::AutoTimerData(Weather_Irrigation *wi) :
    m_wi(wi), m_longitude(config_read_longitude()), m_latitude(config_read_latitude()) {
  set_default_adapter();
}

bool AutoTimerData::update_sunrise_time() {

  auto now_time = time(0);

  if (m_sunrise_time > now_time) // we already calculated a valid sunrise time for today
    return true;

  struct tm now_tm;
  if (!gmtime_r(&now_time, &now_tm))
    return false;

  double sunrise;
  sun_calculateDuskDawn(&sunrise, nullptr, 0, now_tm.tm_yday, m_longitude, m_latitude, CIVIL_TWILIGHT_RAD);

  unsigned sr_dtime_s = unsigned(sunrise * SECS_PER_HOUR);
  unsigned now_dtime_s = now_tm.tm_hour * SECS_PER_HOUR + now_tm.tm_min * SECS_PER_MINT + now_tm.tm_sec;

  const time_t midnight_time = now_time - now_dtime_s;

  // make sure m_sunrise_time is in the future
  m_sunrise_time = (sr_dtime_s > now_dtime_s) ? midnight_time + sr_dtime_s : midnight_time + SECS_PER_DAY + sr_dtime_s;

  return true;
}

struct AutoTimerSaveData {
  std::array<IrrigationZone, CONFIG_APP_MAX_ZONES> zones;
  std::array<WeatherAdapter, CONFIG_APP_FA_MAX_WEATHER_ADAPTERS> adapters; //
  //JSONEAT_SER_FROM_TO(JSONEAT_KvPairs(zones, adapters));
};

static AutoTimerSaveData m_s = { };

#if 0
bool AutoTimerData::save_settings(const char *key) {
  bool result = false;
  if (strstr(key, "at.") != key)
    return false;

  set_default_adapter();
  m_s.zones = m_zones;
  m_s.adapters = m_adapters;

  char dummy;
  const auto json_size = m_s.to_json(&dummy, 0) + 1;

  if (auto json = (char*) malloc(json_size)) {
    if (m_s.to_json(json, json_size) < json_size) {
      if (auto h = kvs_open(kvs_name, kvs_WRITE)) {
        if (kvs_set_str(h, key, json)) {
          result = true;
        }
        kvs_commit(h);
        kvs_close(h);
      }
    }
    free(json);
  }

  return result;

}
bool AutoTimerData::restore_settings(const char *key) {
  bool result = false;
  if (strstr(key, "at.") != key)
    return false;

  if (auto h = kvs_open("full_auto_json", kvs_READ)) {
    if (const auto json_size = kvs_get_strlen(h, key) + 1) {
      if (auto json = (char*) malloc(json_size)) {
        if (kvs_get_str(h, key, json, json_size)) {
          result = true;
          if (m_s.from_json(json)) {
            m_zones = m_s.zones;
            m_adapters = m_s.adapters;
            set_default_adapter();
          }
        }
        free(json);
      }
    }
    kvs_close(h);
  }
  return result;
}
#else
bool AutoTimerData::save_settings(const char *key) {
  bool result = false;
  if (strstr(key, "at.") != key)
    return false;
  if (auto h = kvs_open(kvs_name, kvs_WRITE)) {
    set_default_adapter();
    m_s.zones = m_zones;
    m_s.adapters = m_adapters;

    if (kvs_set_blob(h, key, &m_s, sizeof m_s)) {
      result = true;
    }
    kvs_commit(h);
    kvs_close(h);
  }
  return result;
}

bool AutoTimerData::restore_settings(const char *key) {
  bool result = false;
  if (strstr(key, "at.") != key)
    return false;
  if (auto h = kvs_open(kvs_name, kvs_READ)) {
    if (kvs_get_blob(h, key, &m_s, sizeof m_s)) {
      result = true;
      m_zones = m_s.zones;
      m_adapters = m_s.adapters;
      set_default_adapter();
    }
    kvs_close(h);
  }
  return result;
}
#endif
#include <debug/dbg.h>

void AutoTimerData::dev_random_fill_data() {
  auto tnow = time(0);
  for (auto &o : m_zones) {
    snprintf(o.name, sizeof o.name, "ObjectName-%d", rando(100, 1000));
    o.flags.exists = rando(0, 2);
    o.flags.ignore_rain = rando(0, 2);
    o.state.last_time_wet = rando(tnow - SECS_PER_DAY * 7, tnow - SECS_PER_DAY);
    //o.state.next_time_scheduled = rando(tnow + SECS_PER_HOUR, tnow + SECS_PER_HOUR * 2);
  }
  for (auto &o : m_adapters) {
    if (o.flags.read_only)
      continue;
    snprintf(o.name, sizeof o.name, "ObjectName-%d", rando(100, 1000));
    o.flags.exists = rando(0, 2);
  }
}

bool AutoTimer::should_zone_be_due(const IrrigationZone &z, const time_t twhen) const {
  if (!z.flags.exists) // there is no auto-timer for this zone
    return false;
  if (z.state.next_time_scheduled) // already scheduled
    return false;
  if (z.attr.before_sunrise_s && twhen < (m_sunrise_time - z.attr.before_sunrise_s))  // not in specified time window relative to sunrise
    return false;
  if (m_stm32_state.rain_sensor && !z.flags.ignore_rain) // XXX: It may be cleaner to wait for RV cancel the timer ???
    return false;

  const time_t tlast = z.state.last_time_wet;
  if (!tlast)
    return true;

  auto interval_s = z.attr.interval_s;
  const auto &adapter = m_adapters[z.attr.adapter];
  int dry_hours = 24 * 7;
  float f = 1.0;

  if (tlast) {
    dry_hours = (twhen - tlast) / SECS_PER_HOUR;
  }
  if (m_wi) {
    f = m_wi->get_simple_irrigation_factor(dry_hours, adapter);
  }

  interval_s = (0 < f) ? interval_s * f : 0;
  bool result = (tlast + interval_s) < twhen;
  db_logw("full_auto", "%s() => %u -- name=%s, dry_hours=%d, f=%f, ival=%u, tlast=%lld, twhen=%lld", __func__, result, z.name, dry_hours, f, interval_s, tlast, twhen);
  return result;
}
void AutoTimer::todo_loop() {
  const auto now_time = time(0);
  // TODO: the factor should be valve dependent (dry_time as parameter)
  m_f = m_wi ? m_wi->get_simple_irrigation_factor(36) : 1.0;
  update_sunrise_time();

  // first pass: mark all due valves with flag.is_due
  for (IrrigationZone &z : m_zones) {
    if (m_stm32_state.rain_sensor && !z.flags.ignore_rain) {
      z.state.last_time_wet = now_time; // all zones are supposed to be covered by rain_sensor
    }
    z.flags.is_due = should_zone_be_due(z, now_time);
  }

  sort_zone_idxs();
  D(db_logi(logtag, "used_valves_count=%d, due_zones_count=%u", m_used_valves_count, m_due_zones_count));

  for (int i=0; i < m_due_zones_count; ++i) {
    const auto &ip = m_zone_due_idxs[i];
    IrrigationZone &z = m_zones[ip.idx];

    D(db_logi(logtag, "Schedule valve number %d (%s). prio=%d", ip.idx, z.name, ip.prio));
    SetArgs args;
    args.valve_number = ip.idx;
    args.on_duration = z.attr.duration_s;
    args.ignoreRainSensor = z.flags.ignore_rain;
    z.state.last_time_wet = now_time; // XXX: maybe a bit early. the zone irrigation may need some minutes to hours to start for real.
#ifndef TEST_HOST
    app::stm32::stm32com_set_timer(args);
#endif
  }
}
} // namespace
