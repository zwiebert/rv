#pragma once

#include "adapter.hh"
#include "valve.hh"
#include "weather/weather_irrigation.hh"
#include "jsmn/jsmn_iterate.hh"

#include <string>
#include <list>
#include <iterator>
#include <algorithm>

class AutoTimer {
  using self_type = AutoTimer;

public:
  AutoTimer(Weather_Irrigation *wi = nullptr): m_wi(wi) {
    m_adapters[0].flags.neutral = true;
    m_adapters[0].flags.exists = true;
    m_adapters[0].flags.read_only = true;
    strcpy(m_adapters[0].name, "Neutral");
  }

public:
  bool save_this(const char *key);
  bool restore_this(const char *key);

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

  bool from_json(const char *json);
  bool from_json(JsmnBase::Iterator &it);

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
    bool operator<(const sorted_index & other) const { return other.prio < this->prio;}
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
  static constexpr int TOTAL_OBJS = CONFIG_APP_NUMBER_OF_VALVES +  CONFIG_APP_FA_MAX_WEATHER_ADAPTERS;

};
