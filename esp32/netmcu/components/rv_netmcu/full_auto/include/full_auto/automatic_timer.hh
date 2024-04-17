#pragma once

#include "adapter.hh"
#include "valve.hh"
#include "weather/weather_irrigation.hh"
#include "jsmn/jsmn_iterate.hh"
#include <debug/log.h>
#include <uout/uout_writer.hh>
#include <cstdint>
#include <string>
#include <list>
#include <iterator>
#include <algorithm>
#include <functional>
using namespace std::placeholders;

class AutoTimer {
  constexpr static const char *our_logtag = "auto_timer";
  constexpr static const char *default_save_key = "at.default";
  using self_type = AutoTimer;

public:
  AutoTimer(Weather_Irrigation *wi = nullptr) :
      m_wi(wi) {
    m_s.m_adapters[0].flags.neutral = true;
    m_s.m_adapters[0].flags.exists = true;
    m_s.m_adapters[0].flags.read_only = true;
    strcpy(m_s.m_adapters[0].name, "Neutral");
  }

public:
  /**
   * \brief Save whole settings object as binary.
   * \param key  optional file name, must begin with "at."
   */
  bool save_settings(const char *key = default_save_key);
  /**
   * \brief Restore whole settings object from binary.
   * \param key  optional file name, must begin with "at."
   */
  bool restore_settings(const char *key = default_save_key);

public:
  void todo_loop();

public:
  auto adapters_begin() {
    return std::begin(m_s.m_adapters);
  }
  auto adapters_end() {
    return std::end(m_s.m_adapters);
  }

  auto valves_all_begin() {
    return std::begin(m_s.m_magval);
  }
  auto valves_all_end() {
    return std::end(m_s.m_magval);
  }
  auto get_zone_json(char *dst, size_t dst_size, int idx) {
    return m_s.m_magval[idx].to_json(dst, dst_size);
  }
  auto get_adapter_json(char *dst, size_t dst_size, int idx) {
    return m_s.m_adapters[idx].to_json(dst, dst_size);
  }

public:

  template<typename T, typename std::enable_if<!std::is_class<T> { }, bool>::type = true>
  bool from_json(T json) {
    auto jsmn = Jsmn<1024, T>(json);

    if (!jsmn)
      return false;

    auto it = jsmn.begin();
    return from_json(it);
  }

  bool update(int idx, const WeatherAdapter &adapter) {
    if (!(0 <= idx && idx < CONFIG_APP_FA_MAX_WEATHER_ADAPTERS))
      return false;
    if (adapter.flags.read_only)
      return false;
    m_s.m_adapters[idx] = adapter;
    return true;
  }
  template<typename jsmn_iterator = Jsmn_String::Iterator, typename std::enable_if<std::is_class<jsmn_iterator> { }, bool>::type = true>
  bool update_adapter(int idx, jsmn_iterator &it, bool update = false) {
    if (!(0 <= idx && idx < CONFIG_APP_FA_MAX_WEATHER_ADAPTERS))
      return false;
    auto &el = m_s.m_adapters[idx];
    if (el.flags.read_only)
      return false;
    if (!update)
      el = WeatherAdapter();
    return el.from_json(it);
  }
  bool write_adapter_json(UoutBuilderJson &sj, int idx, const char *key) {
    if (!(0 <= idx && idx < CONFIG_APP_FA_MAX_WEATHER_ADAPTERS))
      return false;
    if (sj.add_key(key)) {
      return sj.read_json_from_function(std::bind(&WeatherAdapter::to_json, &m_s.m_adapters[idx], _1, _2));
    }
    return false;
  }
  bool write_adapters_json(UoutBuilderJson &sj, const char *key) {
    if (sj.add_key(key)) {
      return sj.read_json_arr_from_function(std::bind(&AutoTimer::get_adapter_json, this,  _1, _2, _3), CONFIG_APP_FA_MAX_WEATHER_ADAPTERS);
    }
    return false;
  }

  template<typename jsmn_iterator = Jsmn_String::Iterator, typename std::enable_if<std::is_class<jsmn_iterator> { }, bool>::type = true>
  bool update_zone(int idx, jsmn_iterator &it, bool update = false) {
    if (!(0 <= idx && idx < CONFIG_APP_NUMBER_OF_VALVES))
      return false;
    auto &el = m_s.m_magval[idx];
    if (!update)
      el = MagValve();
    return el.from_json(it);
  }
  bool write_zone_json(UoutBuilderJson &sj, int idx, const char *key) {
    if (!(0 <= idx && idx < CONFIG_APP_NUMBER_OF_VALVES))
      return false;
    if (sj.add_key(key)) {
      return sj.read_json_from_function(std::bind(&MagValve::to_json, &m_s.m_magval[idx], _1, _2));
    }
    return false;
  }
  bool write_zones_json(UoutBuilderJson &sj, const char *key) {
    if (sj.add_key(key)) {
      return sj.read_json_arr_from_function(std::bind(&AutoTimer::get_zone_json, this,  _1, _2, _3), CONFIG_APP_NUMBER_OF_VALVES);
    }
    return false;
  }

  bool write_past_weather_data_json(UoutBuilderJson &sj, const char *key) {
    if (!m_wi)
      return false;

    if (sj.add_key(key)) {
      return m_wi->to_json(sj);
    }
    return false;
  }

  template<typename jsmn_iterator = Jsmn_String::Iterator>
  bool cmd_obj_json(jsmn_iterator &it) {
    int err = 0;
    assert(it->type == JSMN_OBJECT);
    auto count = it->size;
    for (it += 1; count > 0 && it; --count) {
      char cmds[32] = "";

      if (it.takeValue(cmds, "save")) {
        auto key = *cmds ? cmds : default_save_key;
        if (save_settings(key))
          continue;
      db_loge(our_logtag, "save with key <%s> failed", key);
    }
    if (it.takeValue(cmds, "restore")) {
      auto key = *cmds ? cmds : default_save_key;
      if (restore_settings(key))
        continue;
    db_loge(our_logtag, "restore from key <%s> failed", key);
  }

  ++err;
  it.skip_key_and_value();
  continue;
}
return !err;
}
template<typename jsmn_iterator = Jsmn_String::Iterator>
bool set_obj_json(UoutBuilderJson &sj, jsmn_iterator &it, bool update = false) {
int err = 0;
assert(it->type == JSMN_OBJECT);
auto count = it->size;
for (it += 1; count > 0 && it; --count) {

  if (it.keyStartsWith(zone_prefix, JSMN_OBJECT)) {
    MagValve zone;
    int zone_idx = -1;
    char key[16];
    if (it.getValue(key)) {
      zone_idx = atoi(key + strlen(zone_prefix));
      if (update_zone(zone_idx, ++it) && write_zone_json(sj, zone_idx, key)) {
        continue; // update succeeded
      }
    }
    db_loge(our_logtag, "Could not %s zone %d", update ? "update":"set", zone_idx);
    ++err;
    continue;
  }

  if (it.keyStartsWith(adapter_prefix, JSMN_OBJECT)) {
    WeatherAdapter adapter;
    int adapter_idx = -1;
    char key[16];
    if (it.getValue(key)) {
      adapter_idx = atoi(key + strlen(adapter_prefix));
      if (update_adapter(adapter_idx, ++it)) {
        continue; // update succeeded
      }
    }
    db_loge(our_logtag, "Could not %s adapter %d", update ? "update":"set", adapter_idx);
    ++err;
    continue;
  }

  ++err;
  it.skip_key_and_value();
  continue;
}
return !err;
}
template<typename jsmn_iterator = Jsmn_String::Iterator>
bool get_obj_json(UoutBuilderJson &sj, jsmn_iterator &it) {
int err = 0;
assert(it->type == JSMN_OBJECT);
auto count = it->size;
for (it += 1; count > 0 && it; --count) {

  if (it.keyStartsWith(zone_prefix, JSMN_OBJECT)) {
    MagValve zone;
    int zone_idx = -1;
    char key[16];
    if (it.getValue(key)) {
      it.skip_key_and_value();
      zone_idx = atoi(key + strlen(zone_prefix));
      if (write_zone_json(sj, zone_idx, key)) {
        continue; // update succeeded
      }
    }
    db_loge(our_logtag, "Could not get zone %d", zone_idx);
    ++err;
    continue;
  }

  if (it.keyStartsWith(adapter_prefix, JSMN_OBJECT)) {
    WeatherAdapter adapter;
    int adapter_idx = -1;
    char key[16];
    if (it.getValue(key)) {
      it.skip_key_and_value();
      adapter_idx = atoi(key + strlen(adapter_prefix));
      if (write_adapter_json(sj, adapter_idx, key)) {
        continue; // update succeeded
      }
    }
    db_loge(our_logtag, "Could not get adapter %d", adapter_idx);
    ++err;
    continue;
  }
  if (it.keyIsEqual("zones", JSMN_ARRAY)) {
    it.skip_key_and_value();
    if (write_zones_json(sj, "zones")) {
      continue;
    }
    db_loge(our_logtag, "Could not get zones");
    ++err;
    continue;
  }
  if (it.keyIsEqual("adapters", JSMN_ARRAY)) {
    it.skip_key_and_value();
    if (write_adapters_json(sj, "adapters")) {
      continue;
    }
    db_loge(our_logtag, "Could not get adapters");
    ++err;
    continue;
  }

  if (it.keyIsEqual("past_wd", JSMN_ARRAY)) {
    it.skip_key_and_value();
    if (write_past_weather_data_json(sj, "past_wd")) {
      continue;
    }
    db_loge(our_logtag, "Could not get adapters");
    ++err;
    continue;
  }

  ++err;
  it.skip_key_and_value();
  continue;
}
return !err;
}
template<typename jsmn_iterator = Jsmn_String::Iterator>
bool handle_json(UoutBuilderJson &sj, jsmn_iterator &it) {
int err = 0;
assert(it->type == JSMN_OBJECT);
auto count = it->size;

if (sj.add_object("auto")) {
  for (it += 1; count > 0 && it; --count) {

    if (it.keyIsEqual("command", JSMN_OBJECT)) {
      if (cmd_obj_json(++it))
        continue;

      db_loge(our_logtag, "Command failed");
      ++err;
      continue;
    }

    if (it.keyIsEqual("get", JSMN_OBJECT)) {
      if (get_obj_json(sj, ++it))
        continue;

      db_loge(our_logtag, "get failed");
      ++err;
      continue;
    }

    if (it.keyIsEqual("set", JSMN_OBJECT)) {
      if (set_obj_json(sj, ++it))
        continue;

      db_loge(our_logtag, "set failed");
      ++err;
      continue;
    }

    if (it.keyIsEqual("update", JSMN_OBJECT)) {
      if (set_obj_json(sj, ++it), true)
        continue;

      db_loge(our_logtag, "update failed");
      ++err;
      continue;
    }

    db_loge(our_logtag, "unknown key found in json.auto");
    ++err;
    Jsmn_String::skip_key_and_value(it);

  }
  sj.close_object();
}
return !err;
}
template<typename jsmn_iterator = Jsmn_String::Iterator, typename std::enable_if<std::is_class<jsmn_iterator> { }, bool>::type = true>
bool from_json(jsmn_iterator &it) {
assert(it->type == JSMN_OBJECT);

using token_handler_fun_type = bool (*)(self_type &self, jsmn_iterator &it, int &err);

static const token_handler_fun_type tok_processRootChilds_funs[] = { //

    [](self_type &self, jsmn_iterator &it, int &err) -> bool {
      if (it.keyIsEqual("valves", JSMN_ARRAY)) {
        ++it; // skip key token
        auto count = it->size; // get array size
        ++it; // skip array token
        for (int i = 0; i < count; ++i) {
          if (it->type == JSMN_OBJECT) {
            self.m_s.m_magval[i].from_json(it);
          } else if (it.value_equals_null()) {
            // use {} for unchanged
            self.m_s.m_magval[i] = MagValve();
            ++it;
          } else {
            ++err;
            it.skip_key_and_value();
          }

        }
        return true;
      }
      return false;
    },

    [](self_type &self, jsmn_iterator &it, int &err) -> bool {
      if (it.keyIsEqual("adapters", JSMN_ARRAY)) {
        ++it;
        auto count = it->size;
        ++it;
        for (int i = 0; i < count; ++i) {
          if (it->type == JSMN_OBJECT) {
            self.m_s.m_adapters[i].from_json(it);
          } else if (it.value_equals_null()) {
            self.m_s.m_adapters[i] = WeatherAdapter();
            ++it;
          } else {
            ++err;
            it.skip_key_and_value();
          }

        }
        return true;
      }
      return false;
    },

    [](self_type &self, jsmn_iterator &it, int &err) -> bool {
      if (it.getValue(self.name, "name")) {
        it += 2;
        return true;
      }
      return false;
    },

    // any consume not handled tokens
        [](self_type &self, jsmn_iterator &it, int &err) -> bool {
          return it.skip_key_and_value();
        } };

int err = 0;
auto count = it->size;
for (++it; count > 0 && it; --count) {
  for (auto fun : tok_processRootChilds_funs) {
    if (fun(*this, it, err))
      break;
  }
}

return !err;
}

public:
bool should_valve_be_due(const MagValve &v, const time_t twhen = time(0)) const {
if (!v.flags.exists || v.state.next_time_scheduled || m_stm32_state.rain_sensor)
  return false;

const time_t tlast = v.state.last_time_wet;
if (!tlast)
  return true;

auto interval_s = v.attr.interval_s;
const auto &adapter = m_s.m_adapters[v.attr.adapter];
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
//db_logi("full_auto", "%s() => %u -- name=%s, dry_hours=%d, f=%f, ival=%u, tlast=%lld, twhen=%lld", __func__, result, v.name, dry_hours, f, interval_s, tlast, twhen);
return result;
}

private:
void sort_magval_idxs() {

m_used_valves_count = m_due_valves_count = 0;
for (int i = 0; i < CONFIG_APP_NUMBER_OF_VALVES; ++i) {
  auto &dst_due = m_magval_due_idxs[i];
  auto &dst_exists = m_magval_prio_idxs[i];
  auto &src = m_s.m_magval[i];

  dst_exists.idx = dst_due.idx = i;

  if (src.flags.exists) {
    ++m_used_valves_count;
    dst_exists.prio = src.attr.priority;
  } else {
    dst_exists.prio = -100;
  }
  if (src.flags.exists && src.flags.is_due) {
    ++m_due_valves_count;
    dst_due.prio = src.attr.priority;
  } else {
    dst_due.prio = -100;
  }
}
std::sort(std::begin(m_magval_prio_idxs), std::end(m_magval_prio_idxs));
std::sort(std::begin(m_magval_due_idxs), std::end(m_magval_due_idxs));
}

private:
struct sorted_index {
int8_t idx, prio;
bool operator<(const sorted_index &other) const {
  return other.prio < this->prio;
}
};

private:
static constexpr auto zone_prefix = "zone.";
static constexpr auto adapter_prefix = "adapter.";
private:
char name[CONFIG_APP_FA_NAMES_MAX_LEN] = "";
struct {
MagValve m_magval[CONFIG_APP_NUMBER_OF_VALVES];
WeatherAdapter m_adapters[CONFIG_APP_FA_MAX_WEATHER_ADAPTERS];
} m_s;
sorted_index m_magval_prio_idxs[CONFIG_APP_NUMBER_OF_VALVES];
sorted_index m_magval_due_idxs[CONFIG_APP_NUMBER_OF_VALVES];
uint8_t m_used_valves_count = 0, m_due_valves_count = 0;
private:
Weather_Irrigation *m_wi = nullptr;
float m_f = 1.0;
private:
struct {
bool rain_sensor;
} m_stm32_state;
public:
void dev_random_fill_data();
static constexpr int TOTAL_OBJS = CONFIG_APP_NUMBER_OF_VALVES + CONFIG_APP_FA_MAX_WEATHER_ADAPTERS;

};
