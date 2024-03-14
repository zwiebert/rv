#include <full_auto/adapter.hh>
#include <full_auto/single_valve.hh>
#include <full_auto/valve_group.hh>

#include "jsmn/jsmn_iterate.hh"
#include <utils_misc/allocate_buffer.hh>

#include <cstring>
#include <cstdlib>
#include <cassert>

using jp = Jsmn<100>;
using jpit = jp::Iterator;

bool WeatherAdapter::from_json(const char *json) {
  auto jsmn = jp(json);

  if (!jsmn)
    return false;

  using tok_processObj_funT = bool (*)(WeatherAdapter &adapter, jpit &it);
  static const tok_processObj_funT tok_processRootChilds_funs[] = { //

      [](WeatherAdapter &adapter, jpit &it) -> bool { // Process object: main
        if (it.keyIsEqual("flags", JSMN_OBJECT)) {
          auto count = it[1].size;
          for (it += 2; count > 0 && it; --count) {
            if (it.getValue(adapter.flags.exists, "exists")) {
              it += 2;
            } else {
              jp::skip_key_and_value(it);
            }
          }
          return true;
        }
        return false;

      },

      [](WeatherAdapter &adapter, jpit &it) -> bool { // Throw away unwanted objects
        return jp::skip_key_and_value(it);
      } };

  auto it = jsmn.begin();
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
  auto jsmn = jp(json);

  if (!jsmn)
    return false;

  using tok_processObj_funT = bool (*)(SingleValve &valve, jpit &it);
  static const tok_processObj_funT tok_processRootChilds_funs[] = { //

      [](SingleValve &valve, jpit &it) -> bool { // Process object: main
        if (it.keyIsEqual("flags", JSMN_OBJECT)) {
          auto count = it[1].size;
          for (it += 2; count > 0 && it; --count) {
            if (it.getValue(valve.flags.active, "active") || it.getValue(valve.flags.exists, "exists") || it.getValue(valve.flags.has_adapter, "has_adapter")) {
              it += 2;
            } else {
              jp::skip_key_and_value(it);
            }
          }
          return true;
        }
        return false;

      },

      [](SingleValve &valve, jpit &it) -> bool { // Throw away unwanted objects
        return jp::skip_key_and_value(it);
      } };

  auto it = jsmn.begin();
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
  auto jsmn = jp(json);

  if (!jsmn)
    return false;

  using tok_processObj_funT = bool (*)(ValveGroup &group, jpit &it);
  static const tok_processObj_funT tok_processRootChilds_funs[] = { //

      [](ValveGroup &group, jpit &it) -> bool { // Process object: main
        if (it.keyIsEqual("flags", JSMN_OBJECT)) {
          auto count = it[1].size;
          for (it += 2; count > 0 && it; --count) {
            if (it.getValue(group.flags.active, "active") || it.getValue(group.flags.exists, "exists") || it.getValue(group.flags.has_adapter, "has_adapter")) {
              it += 2;
            } else {
              jp::skip_key_and_value(it);
            }
          }
          return true;
        }
        return false;

      },

      [](ValveGroup &group, jpit &it) -> bool { // Throw away unwanted objects
        return jp::skip_key_and_value(it);
      } };

  auto it = jsmn.begin();
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

