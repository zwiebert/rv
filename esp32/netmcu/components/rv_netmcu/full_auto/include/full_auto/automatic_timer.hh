#pragma once

#include "adapter.hh"
#include "single_valve.hh"
#include "valve_group.hh"

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
  int to_json(char *buf, size_t buf_size, int &obj_ct);
private:
  SingleValve m_valves[CONFIG_APP_NUMBER_OF_VALVES];
  ValveGroup m_valveGroups[CONFIG_APP_FA_MAX_VALVE_GROUPS];
  WeatherAdapter m_adapters[CONFIG_APP_FA_MAX_WEATHER_ADAPTERS];
};
