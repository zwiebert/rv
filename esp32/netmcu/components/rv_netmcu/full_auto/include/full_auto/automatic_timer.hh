#pragma once

#include "adapter.hh"
#include "irrigation_zone.hh"
#include "weather/weather_irrigation.hh"
#include "jsmn/jsmn_iterate.hh"
#include "jsoneat/from_to_json_jsmn_cbuf.hh"
#include <debug/log.h>
#include <uout/uout_writer.hh>
#include <cstdint>
#include <string>
#include <list>
#include <iterator>
#include <algorithm>
#include <functional>
using namespace std::placeholders;

namespace app::fa {

class AutoTimer {
public:
  constexpr static const char *our_logtag = "auto_timer";
  constexpr static const char *default_save_key = "at.default";
  using self_type = AutoTimer;
  inline void set_default_adapter() {
    m_adapters[0].flags.neutral = true;
    m_adapters[0].flags.exists = true;
    m_adapters[0].flags.read_only = true;
    strcpy(m_adapters[0].name, "Neutral");
  }

public:
  AutoTimer(Weather_Irrigation *wi = nullptr) :
      m_wi(wi) {
    set_default_adapter();
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
    return std::begin(m_adapters);
  }
  auto adapters_end() {
    return std::end(m_adapters);
  }
  auto zones_all_begin() {
    return std::begin(m_zones);
  }
  auto zonees_all_end() {
    return std::end(m_zones);
  }

private:
  auto get_zone_json(char *dst, size_t dst_size, int idx) {
    return jsoneat::to_json::cbuf::to_json_val(dst, dst_size, m_zones[idx]);
  }
  auto get_adapter_json(char *dst, size_t dst_size, int idx) {
    return jsoneat::to_json::cbuf::to_json_val(dst, dst_size, m_adapters[idx]);
  }

public:
  bool update(int idx, const WeatherAdapter &adapter) {
    if (!(0 <= idx && idx < CONFIG_APP_FA_MAX_WEATHER_ADAPTERS))
      return false;
    if (adapter.flags.read_only)
      return false;
    m_adapters[idx] = adapter;
    return true;
  }
  /**
   * \brief  Update or Set a single adapter from JSON object
   * \tparam jsmn_iterator  type of argument "it"
   * \param idx    number of adapter
   * \param it     iterator pointing to JSON zone object (without its key)
   * \param update If false, the adapter is initialized first with default values
   * \return       success. Failures: idx out of range. read_only flag set. Wrong JSON content.
   */
  template<typename jsmn_iterator>
  bool update_adapter_from_json(int idx, jsmn_iterator &it, bool update = false) {
    if (!(0 <= idx && idx < CONFIG_APP_FA_MAX_WEATHER_ADAPTERS))
      return false;
    auto &el = m_adapters[idx];
    if (el.flags.read_only)
      return false;
    if (!update)
      el = WeatherAdapter();
    return el._from_json(it);
  }
  /**
   * \brief      Write content of a single adapter to JSON stream
   * \param td   output JSON stream
   * \param idx  number of adapter
   * \param key  JSON key for the JSON object written
   * \return     success
   */
  bool write_adapter_to_json(UoutWriter &td, int idx, const char *key) {
    if (!(0 <= idx && idx < CONFIG_APP_FA_MAX_WEATHER_ADAPTERS))
      return false;
    if (td.sj().add_key(key)) {
      return td.sj().read_json_from_function(std::bind(&WeatherAdapter::to_json, &m_adapters[idx], _1, _2));
    }
    return false;
  }
  /**
   * \brief     Write array of all adapter objects to JSON stream
   * \param td  output JSON stream
   * \param key JSON key for the JSON array written
   * \return    success
   */
  bool write_adapters_to_json(UoutWriter &td, const char *key) {
    if (td.sj().add_key(key)) {
      return td.sj().read_json_arr_from_function(std::bind(&AutoTimer::get_adapter_json, this, _1, _2, _3), CONFIG_APP_FA_MAX_WEATHER_ADAPTERS);
    }
    return false;
  }

  /**
   * \brief  Update or Set a single zone from JSON object
   * \tparam jsmn_iterator  type of argument "it"
   * \param idx    number of zone
   * \param it     iterator pointing to JSON zone object (without its key)
   * \param update If false, the zone is initialized first with default values
   * \return       success. Failures: idx out of range.  Wrong JSON content.
   */
  template<typename jsmn_iterator>
  bool update_zone_from_json(int idx, jsmn_iterator &it, bool update = false) {
    if (!(0 <= idx && idx < CONFIG_APP_MAX_ZONES))
      return false;
    auto &el = m_zones[idx];
    if (!update)
      el = IrrigationZone();
    return el._from_json(it);
  }
  /**
   * \brief      Write content of a single zoneV to JSON stream
   * \param td   output JSON stream
   * \param idx  number of zone
   * \param key  JSON key for the JSON object written
   * \return     success
   */
  bool write_zone_to_json(UoutWriter &td, int idx, const char *key) {
    if (!(0 <= idx && idx < CONFIG_APP_MAX_ZONES))
      return false;
    if (td.sj().add_key(key)) {
      return td.sj().read_json_from_function(std::bind(&IrrigationZone::to_json, &m_zones[idx], _1, _2));
    }
    return false;
  }
  /**
   * \brief     Write array of all zone objects to JSON stream
   * \param td  output JSON stream
   * \param key JSON key for the JSON array written
   * \return    success
   */
  bool write_zones_to_json(UoutWriter &td, const char *key) {
    if (td.sj().add_key(key)) {
      return td.sj().read_json_arr_from_function(std::bind(&AutoTimer::get_zone_json, this, _1, _2, _3), CONFIG_APP_MAX_ZONES);
    }
    return false;
  }
  /**
   * \brief     Write past weather data array to JSON stream
   * \param td  output JSON stream
   * \param key JSON key for the JSON array written
   * \return    success
   */
  bool write_past_weather_data_to_json(UoutWriter &td, const char *key) {
    if (!m_wi)
      return false;

    if (td.sj().add_key(key)) {
      return m_wi->to_json(td);
    }
    return false;
  }

public:
  /**
   * \brief       Test if the given valve is scheduled for now or a given future time
   * \param v     the valve in question
   * \param twhen the time for which we ask. Should be now or in the future.
   * \return      true, if valve is due
   */
  bool should_valve_be_due(const IrrigationZone &v, const time_t twhen = time(0)) const {
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
    bool result = (tlast + interval_s) < twhen;
//db_logi("full_auto", "%s() => %u -- name=%s, dry_hours=%d, f=%f, ival=%u, tlast=%lld, twhen=%lld", __func__, result, v.name, dry_hours, f, interval_s, tlast, twhen);
    return result;
  }

private:
  /**
   * \brief  sort valve indexes according to priority and schedule (due)
   *
   * This allows looking up the order in which the zones (valves) need to be irrigated.
   *
   */
  void sort_zone_idxs() {

    m_used_valves_count = m_due_valves_count = 0;
    for (int i = 0; i < CONFIG_APP_MAX_ZONES; ++i) {
      auto &dst_due = m_zone_due_idxs[i];
      auto &dst_exists = m_zone_prio_idxs[i];
      auto &src = m_zones[i];

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
    std::sort(std::begin(m_zone_prio_idxs), std::end(m_zone_prio_idxs));
    std::sort(std::begin(m_zone_due_idxs), std::end(m_zone_due_idxs));
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
  IrrigationZone m_zones[CONFIG_APP_MAX_ZONES];
  WeatherAdapter m_adapters[CONFIG_APP_FA_MAX_WEATHER_ADAPTERS];
  sorted_index m_zone_prio_idxs[CONFIG_APP_MAX_ZONES];
  sorted_index m_zone_due_idxs[CONFIG_APP_MAX_ZONES];
  uint8_t m_used_valves_count = 0, m_due_valves_count = 0;
private:
  Weather_Irrigation *m_wi = nullptr;
  float m_f = 1.0;
private:
  struct {
    bool rain_sensor;
  } m_stm32_state;
public:
  void dev_random_fill_data(); //
  JSONEAT_SER_FROM_TO(JSONEAT_KvPairs(name), jsoneat::KvPair("valves", m_zones), //
      jsoneat::KvPair("adapters", m_adapters))
  ;
};

} // namespace
