#pragma once

#include "adapter.hh"
#include "single_valve.hh"
#include "valve_group.hh"
#include "jsmn/jsmn_iterate.hh"

#include <string>
#include <list>
#include <iterator>

class AutoTimer {
public:
  bool save_this(const char *key);
  bool restore_this(const char *key);

  auto valves_begin() {
    return std::begin(m_valves);
  }
  auto valves_end() {
    return std::end(m_valves);
  }
  auto valveGroups_begin() {
    return std::begin(m_valveGroups);
  }
  auto valveGroups_end() {
    return std::end(m_valveGroups);
  }
  auto adapters_begin() {
    return std::begin(m_adapters);
  }
  auto adapters_end() {
    return std::end(m_adapters);
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

private:
  char name[CONFIG_APP_FA_NAMES_MAX_LEN] = "";
  SingleValve m_valves[CONFIG_APP_NUMBER_OF_VALVES];
  ValveGroup m_valveGroups[CONFIG_APP_FA_MAX_VALVE_GROUPS];
  WeatherAdapter m_adapters[CONFIG_APP_FA_MAX_WEATHER_ADAPTERS];
public:
  void dev_random_fill_data();
};
