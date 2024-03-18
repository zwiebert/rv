#include <full_auto/adapter.hh>
#include <full_auto/single_valve.hh>
#include <full_auto/valve_group.hh>
#include <full_auto/automatic_timer.hh>

#include "jsmn/jsmn_iterate.hh"
#include <utils_misc/allocate_buffer.hh>

#include <cstring>
#include <cstdlib>
#include <cassert>

bool WeatherAdapter::from_json(const char *json) {
  auto jsmn = Jsmn<32>(json);

  if (!jsmn)
    return false;

  auto it = jsmn.begin();
  return from_json(it);
}

bool WeatherAdapter::from_json(JsmnBase::Iterator &it) {

  using tok_processObj_funT = bool (*)(WeatherAdapter &adapter, JsmnBase::Iterator &it);
  static const tok_processObj_funT tok_processRootChilds_funs[] = { //

      [](WeatherAdapter &adapter, JsmnBase::Iterator &it) -> bool { // Process object: flags
        if (it.keyIsEqual("flags", JSMN_OBJECT)) {
          auto count = it[1].size;
          for (it += 2; count > 0 && it; --count) {
            if (it.getValue(adapter.flags.exists, "exists")) {
              it += 2;
            } else {
              JsmnBase::skip_key_and_value(it);
            }
          }
          return true;
        }
        return false;

      },

      [](WeatherAdapter &adapter, JsmnBase::Iterator &it) -> bool { // Throw away unwanted objects
        return JsmnBase::skip_key_and_value(it);
      } };

  if (it->type == JSMN_OBJECT) { // root object
    auto count = it->size;
    for (++it; count > 0 && it; --count) {
      assert(it->type == JSMN_STRING);
      if (it.getValue(name, "name") || it.getValue(d_temp, "temp") || it.getValue(d_wind, "wind") || it.getValue(d_humi, "humi")
          || it.getValue(d_clouds, "clouds")) {
        it += 2;
      } else {
        for (auto fun : tok_processRootChilds_funs) {
          if (fun(*this, it))
            break;
        }
      }
    }
    return true;
  }

  return false;
}

bool SingleValve::from_json(const char *json) {
  auto jsmn = Jsmn<32>(json);

  if (!jsmn)
    return false;

  auto it = jsmn.begin();
  return from_json(it);
}

bool SingleValve::from_json(JsmnBase::Iterator &it) {

  using tok_processObj_funT = bool (*)(SingleValve &valve, JsmnBase::Iterator &it);
  static const tok_processObj_funT tok_processRootChilds_funs[] = { //

      [](SingleValve &valve, JsmnBase::Iterator &it) -> bool { // Process object: flags
        if (it.keyIsEqual("flags", JSMN_OBJECT)) {
          auto count = it[1].size;
          for (it += 2; count > 0 && it; --count) {
            if (it.getValue(valve.flags.active, "active") || it.getValue(valve.flags.exists, "exists") || it.getValue(valve.flags.has_adapter, "has_adapter")) {
              it += 2;
            } else {
              JsmnBase::skip_key_and_value(it);
            }
          }
          return true;
        }
        return false;

      },

      [](SingleValve &valve, JsmnBase::Iterator &it) -> bool { // Throw away unwanted objects
        return JsmnBase::skip_key_and_value(it);
      } };

  if (it->type == JSMN_OBJECT) { // root object
    auto count = it->size;
    for (++it; count > 0 && it; --count) {
      assert(it->type == JSMN_STRING);
      if (it.getValue(name, "name") || it.getValue(dry_days, "dry_days") || it.getValue(duration_s, "duration_s") || it.getValue(flow_lph, "flow_lph")
          || it.getValue(adapter, "adapter")) {
        it += 2;
      } else {
        for (auto fun : tok_processRootChilds_funs) {
          if (fun(*this, it))
            break;
        }
      }
    }
    return true;
  }

  return false;
}

bool ValveGroup::from_json(const char *json) {
  auto jsmn = Jsmn<32>(json);

  if (!jsmn)
    return false;

  auto it = jsmn.begin();
  return from_json(it);
}

bool ValveGroup::from_json(JsmnBase::Iterator &it) {

  using tok_processObj_funT = bool (*)(ValveGroup &group, JsmnBase::Iterator &it);
  static const tok_processObj_funT tok_processRootChilds_funs[] = { //

      [](ValveGroup &group, JsmnBase::Iterator &it) -> bool { // Process object: flags
        if (it.keyIsEqual("flags", JSMN_OBJECT)) {
          auto count = it[1].size;
          for (it += 2; count > 0 && it; --count) {
            if (it.getValue(group.flags.active, "active") || it.getValue(group.flags.exists, "exists") || it.getValue(group.flags.has_adapter, "has_adapter")) {
              it += 2;
            } else {
              JsmnBase::skip_key_and_value(it);
            }
          }
          return true;
        }
        return false;

      },

      [](ValveGroup &group, JsmnBase::Iterator &it) -> bool { // Throw away unwanted objects
        return JsmnBase::skip_key_and_value(it);
      } };

  if (it->type == JSMN_OBJECT) { // root object
    auto count = it->size;
    for (++it; count > 0 && it; --count) {
      assert(it->type == JSMN_STRING);
      if (it.getValue(name, "name") || it.getValue(valve_bits, "valve_bits") || it.getValue(interval_days, "interval_days")
          || it.getValue(adapter, "adapter")) {
        it += 2;
      } else {
        for (auto fun : tok_processRootChilds_funs) {
          if (fun(*this, it))
            break;
        }
      }
    }
    return true;
  }

  return false;
}

bool AutoTimer::from_json(const char *json) {
  auto jsmn = Jsmn<32>(json);

  if (!jsmn)
    return false;

  auto it = jsmn.begin();
  return from_json(it);
}

bool AutoTimer::from_json(JsmnBase::Iterator &it) {

  using tok_processObj_funT = bool (*)(AutoTimer &self, JsmnBase::Iterator &it);
  static const tok_processObj_funT tok_processRootChilds_funs[] = { //

      [](AutoTimer &self, JsmnBase::Iterator &it) -> bool { // Process object: flags
        if (it.keyIsEqual("valves", JSMN_ARRAY)) {
          auto count = it[1].size;
          for (int i = 0; i < count; ++i) {
            if (it->type == JSMN_OBJECT) {
              self.m_valves[i].from_json(it);
            } else {
              self.m_valves[i] = SingleValve();
            }
          }
          return true;
        }
        return false;
      },

      [](AutoTimer &self, JsmnBase::Iterator &it) -> bool { // Throw away unwanted objects
        return JsmnBase::skip_key_and_value(it);
      } };

  if (it->type == JSMN_OBJECT) { // root object
    auto count = it->size;
    for (++it; count > 0 && it; --count) {
      assert(it->type == JSMN_STRING);
      if (it.getValue(name, "name")) {
        it += 2;
      } else {
        for (auto fun : tok_processRootChilds_funs) {
          if (fun(*this, it))
            break;
        }
      }
    }
    return true;
  }

  return false;
}
