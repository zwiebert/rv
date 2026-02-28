#include <full_auto/automatic_timer.hh>
#include <stm32_com/stm32_commands.hh>
#include <kvs/kvs_wrapper.h>
#include <cstdio>
#include <cstring>
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

bool AutoTimerData::save_settings(const char *key) {
  bool result = false;
  if (strstr(key, "at.") != key)
    return false;
  if (auto h = kvs_open(kvs_name, kvs_WRITE)) {
    struct {
      std::array<IrrigationZone, CONFIG_APP_MAX_ZONES> zones;
      std::array<WeatherAdapter, CONFIG_APP_FA_MAX_WEATHER_ADAPTERS> adapters;
    } m_s = { m_zones,  m_adapters };
      set_default_adapter();
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
    struct {
      std::array<IrrigationZone, CONFIG_APP_MAX_ZONES> zones;
      std::array<WeatherAdapter, CONFIG_APP_FA_MAX_WEATHER_ADAPTERS> adapters;
    } m_s = { };
    if (kvs_get_blob(h, key, &m_s, sizeof m_s)) {
      result = true;
      m_zones = m_s.zones;
      m_adapters =  m_s.adapters;
      set_default_adapter();
    }
    kvs_close(h);
  }
  return result;
}

#include <debug/dbg.h>

void AutoTimerData::dev_random_fill_data() {
  auto tnow = time(0);
  for (auto &o : m_zones) {
    snprintf(o.name, sizeof o.name, "ObjectName-%d", rando(100, 1000));
    o.flags.exists = rando(0, 2);
    o.flags.active = rando(0, 2);
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

void AutoTimer::todo_loop() {
  // TODO: the factor should be valve dependent (dry_time as parameter)
  m_f = m_wi ? m_wi->get_simple_irrigation_factor(36) : 1.0;

  // first pass: mark all due valves with flag.is_due
  for (IrrigationZone &mv : m_zones) {
    mv.flags.is_due = should_valve_be_due(mv, time(0));
  }
  sort_zone_idxs();
  D(db_logi(logtag, "used_valves_count=%d, due_valves_count=%u", m_used_valves_count, m_due_valves_count));

  for (auto ip : m_zone_due_idxs) {
    IrrigationZone &v = m_zones[ip.idx];
    if (!v.flags.exists || !v.flags.is_due)
      break; // not needed to look at more elements in sorted list
    D(db_logi(logtag, "Schedule valve number %d (%s). prio=%d", ip.idx, v.name, ip.prio));
    SetArgs args;
    args.valve_number = ip.idx;
    args.on_duration = v.attr.duration_s;
    v.state.last_time_wet = time(0);
#ifndef TEST_HOST
    app::stm32::stm32com_set_timer(args);
#endif
  }
}
} // namespace
