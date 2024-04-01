#pragma once

#include "adapter.hh"
#include "valve.hh"
#include "weather/weather_irrigation.hh"
#include "jsmn/jsmn_iterate.hh"
#include <debug/log.h>
#include <cstdint>
#include <string>
#include <list>
#include <iterator>
#include <algorithm>

class AutoTimer {
  constexpr static const char *our_logtag = "auto_timer";
  constexpr static const char *default_save_key = "at.default";
  using self_type = AutoTimer;

public:
  AutoTimer(Weather_Irrigation *wi = nullptr) :
      m_wi(wi) {
    m_adapters[0].flags.neutral = true;
    m_adapters[0].flags.exists = true;
    m_adapters[0].flags.read_only = true;
    strcpy(m_adapters[0].name, "Neutral");
  }

public:
  /**
   * \brief Save whole object as binary.
   * \param key  optional file name, must begin with "at."
   */
  bool save_this(const char *key = default_save_key);
  /**
   * \brief Restore whole object from binary.
   * \param key  optional file name, must begin with "at."
   */
  bool restore_this(const char *key = default_save_key);

public:
  void todo_loop();

public:
  auto adapters_begin() {
    return std::begin(m_adapters);
  }
  auto adapters_end() {
    return std::end(m_adapters);
  }

  auto valves_all_begin() {
    return std::begin(m_magval);
  }
  auto valves_all_end() {
    return std::end(m_magval);
  }

public:

  /**
   * \brief  reads member objects in JSON format
   *
   * This is called from a read function which will call it multiple times until all data was read
   *
   * \param buf       buffer.
   * \param buf_size  should at least 250 bytes
   * \param obj_ct    holds the state (where we were at return). Needs to passed unchanged to next call.
   *                  obj_ct holds the number of objects
   *                  if obj_ct is -1 (EOF), the function does nothing
   *
   * \return          returns the number of bytes written to buf
   *                  returns 0 for EOF or if buffer was not large enough
   * .
   */
  int to_json(char *buf, size_t buf_size, int &obj_ct);

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
    m_adapters[idx] = adapter;
    return true;
  }
  template<typename jsmn_iterator = Jsmn_String::Iterator, typename std::enable_if<std::is_class<jsmn_iterator> { }, bool>::type = true>
  bool update_adapter(int idx, jsmn_iterator &it) {
    if (!(0 <= idx && idx < CONFIG_APP_FA_MAX_WEATHER_ADAPTERS))
      return false;
    if (m_adapters[idx].flags.read_only)
      return false;
    return m_adapters[idx].from_json(it);
  }

  bool update(int idx, const MagValve &zone) {
    if (!(0 <= idx && idx < CONFIG_APP_NUMBER_OF_VALVES))
      return false;
    m_magval[idx] = zone;
    return true;
  }

  template<typename jsmn_iterator = Jsmn_String::Iterator, typename std::enable_if<std::is_class<jsmn_iterator> { }, bool>::type = true>
  bool update_zone(int idx, jsmn_iterator &it) {
    if (!(0 <= idx && idx < CONFIG_APP_NUMBER_OF_VALVES))
      return false;
    return m_magval[idx].from_json(it);
  }

  template<typename jsmn_iterator = Jsmn_String::Iterator>
  bool cmd_obj_json(jsmn_iterator &it) {
    int err = 0;
    assert(it->type == JSMN_OBJECT);
    auto count = it->size;
    for (it += 1; count > 0 && it; --count) {
      int cmdn = -1;
      char cmds[32] = "";

      if (it.takeValue(cmds, "save")) {
        auto key = *cmds ? cmds : default_save_key;
        if (save_this(key))
        continue;
        db_loge(our_logtag, "save with key <%s> failed", key);
      }
      if (it.takeValue(cmds, "restore")) {
        auto key = *cmds ? cmds : default_save_key;
        if (restore_this(key))
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
  bool handle_json(jsmn_iterator &it) {
    int err = 0;
    assert(it->type == JSMN_OBJECT);
    auto count = it->size;
    for (it += 1; count > 0 && it; --count) {

      if (it.keyIsEqual("command", JSMN_OBJECT)) {
        if (cmd_obj_json(++it))
          continue;

        db_loge(our_logtag, "Command failed");
        ++err;
        continue;
      }

      const auto zone_prefix = "zone.";

      if (it.keyStartsWith(zone_prefix, JSMN_OBJECT)) {
        MagValve zone;
        int zone_idx = -1;
        char key[16];
        if (it.getValue(key)) {
          zone_idx = atoi(key + strlen(zone_prefix));
          if (update_zone(zone_idx, ++it)) {
            continue; // update succeeded
          }
        }
        db_loge(our_logtag, "Could not update zone %d", zone_idx);
        ++err;
        continue;
      }

      const auto adapter_prefix = "adapter.";
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
        db_loge(our_logtag, "Could not update adapter %d", adapter_idx);
        ++err;
        continue;
      }

      db_loge(our_logtag, "unknown key found in json.auto");
      ++err;
      Jsmn_String::skip_key_and_value(it);

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
                self.m_magval[i].from_json(it);
              } else if (it.value_equals_null()) {
                // use {} for unchanged
                self.m_magval[i] = MagValve();
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
                self.m_adapters[i].from_json(it);
              } else if (it.value_equals_null()) {
                self.m_adapters[i] = WeatherAdapter();
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
    const auto &adapter = m_adapters[v.attr.adapter];
    int dry_hours = 24 * 7;
    float f = 1.0;

    if (tlast) {
      dry_hours = (twhen - tlast) / SECS_PER_HOUR;
    }
    if (m_wi) {
      f = m_wi->get_simple_irrigation_factor(dry_hours, adapter);
    }

    interval_s = (0 < f) ? interval_s * f : 0;

    return twhen < (tlast + interval_s);
  }

private:
  void sort_magval_idxs() {

    m_used_valves_count = m_due_valves_count = 0;
    for (int i = 0; i < CONFIG_APP_NUMBER_OF_VALVES; ++i) {
      auto &dst_due = m_magval_due_idxs[i];
      auto &dst_exists = m_magval_prio_idxs[i];
      auto &src = m_magval[i];

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
  char name[CONFIG_APP_FA_NAMES_MAX_LEN] = "";
  MagValve m_magval[CONFIG_APP_NUMBER_OF_VALVES];
  WeatherAdapter m_adapters[CONFIG_APP_FA_MAX_WEATHER_ADAPTERS];
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
