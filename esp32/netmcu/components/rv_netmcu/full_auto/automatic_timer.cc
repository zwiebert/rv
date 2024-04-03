#include <full_auto/automatic_timer.hh>
#include <key_value_store/kvs_wrapper.h>
#include <cstdio>
#include <cstring>

#include <debug/log.h>
#ifdef CONFIG_RV_NETMCU_DEBUG
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "rv.full_auto"

static constexpr char kvs_name[] = "full_auto";

bool AutoTimer::save_settings(const char *key) {
  bool result = false;
  if (strstr(key, "at.") != key)
    return false;
  if (auto h = kvs_open(kvs_name, kvs_WRITE)) {
    if (kvs_set_blob(h, key, &m_s, sizeof m_s)) {
      result = true;
    }
    kvs_commit(h);
    kvs_close(h);
  }

  return result;

}
bool AutoTimer::restore_settings(const char *key) {
  bool result = false;
  if (strstr(key, "at.") != key)
    return false;

  if (auto h = kvs_open(kvs_name, kvs_READ)) {
    if (kvs_get_blob(h, key, &m_s, sizeof m_s)) {
      result = true;
    }
    kvs_close(h);
  }
  return result;
}

#define TO_JSON_FLAG_EXISTS v.flags.exists
#include <weather/to_json.hh>
int AutoTimer::to_json(char *buf, size_t buf_size, int &obj_ct, int &state, int start_ct) {
  int bi = 0;
  int arr_idx = obj_ct - start_ct;
  const auto total_objs = TOTAL_OBJS + (m_wi ? m_wi->TOTAL_OBJS:0);
  if (!(0 <= arr_idx && arr_idx < total_objs))
    return 0; // out of our range, maybe interpreted as EOF by caller

  bi += array_to_json_tmpl(buf + bi, buf_size - bi, obj_ct, &m_s.m_magval[0], CONFIG_APP_NUMBER_OF_VALVES, "valves", start_ct);
  start_ct += CONFIG_APP_NUMBER_OF_VALVES;

  bi += array_to_json_tmpl(buf + bi, buf_size - bi, obj_ct, &m_s.m_adapters[0], CONFIG_APP_FA_MAX_WEATHER_ADAPTERS, "adapters", start_ct);
  start_ct += CONFIG_APP_FA_MAX_WEATHER_ADAPTERS;

  if (m_wi) {
    int state;
    bi += m_wi->to_json(buf + bi, buf_size - bi, obj_ct, state, start_ct);
  }


  state = arr_idx+1 == total_objs;
  return bi;
}

#include <debug/dbg.h>

void AutoTimer::dev_random_fill_data() {
  auto tnow = time(0);
  for (auto &o : m_s.m_magval) {
    snprintf(o.name, sizeof o.name, "ObjectName-%d", rando(100, 1000));
    o.flags.exists = rando(0, 2);
    o.flags.active = rando(0, 2);
    o.state.last_time_wet = rando(tnow - SECS_PER_DAY * 7, tnow - SECS_PER_DAY);
    //o.state.next_time_scheduled = rando(tnow + SECS_PER_HOUR, tnow + SECS_PER_HOUR * 2);
  }
  for (auto &o : m_s.m_adapters) {
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
  for (MagValve &mv : m_s.m_magval) {
    mv.flags.is_due = should_valve_be_due(mv, time(0));
  }
  sort_magval_idxs();
  D(db_logi(logtag, "used_valves_count=%d, due_valves_count=%u", m_used_valves_count, m_due_valves_count));

  for (auto ip : m_magval_due_idxs) {
    auto &v = m_s.m_magval[ip.idx];
    if (!v.flags.exists || !v.flags.is_due)
      break;
    D(db_logi(logtag, "Schedule valve number %d (%s). prio=%d", ip.idx, v.name, ip.prio));
  }
}
