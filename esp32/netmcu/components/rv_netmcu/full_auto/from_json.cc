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

  using tok_processObj_funT = bool (*)(self_type &self, JsmnBase::Iterator &it);
  static const tok_processObj_funT tok_processRootChilds_funs[] = { //

      [](self_type &self, JsmnBase::Iterator &it) -> bool { // Process object: flags
        if (it.keyIsEqual("flags", JSMN_OBJECT)) {
          auto count = it[1].size;
          for (it += 2; count > 0 && it; --count) {
            if (it.getValue(self.flags.exists, "exists")) {
              it += 2;
            } else {
              JsmnBase::skip_key_and_value(it);
            }
          }
          return true;
        }
        return false;

      },

      [](self_type &self, JsmnBase::Iterator &it) -> bool {
        if (it.getValue(self.name, "name") || it.getValue(self.d_temp, "temp") || it.getValue(self.d_wind, "wind") || it.getValue(self.d_humi, "humi")
            || it.getValue(self.d_clouds, "clouds")) {
          it += 2;
          return true;
        }
        return false;
      },

      [](self_type &self, JsmnBase::Iterator &it) -> bool { // Throw away unwanted objects
        return JsmnBase::skip_key_and_value(it);
      } };

  if (it->type == JSMN_OBJECT) { // root object
    auto count = it->size;
    for (++it; count > 0 && it; --count) {
      assert(it->type == JSMN_STRING);
      for (auto fun : tok_processRootChilds_funs) {
        if (fun(*this, it))
          break;

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

  using tok_processObj_funT = bool (*)(SingleValve &self, JsmnBase::Iterator &it);
  static const tok_processObj_funT tok_processRootChilds_funs[] = { //

      [](SingleValve &self, JsmnBase::Iterator &it) -> bool { // Process object: flags
        if (it.keyIsEqual("flags", JSMN_OBJECT)) {
          auto count = it[1].size;
          for (it += 2; count > 0 && it; --count) {
            if (it.getValue(self.flags.active, "active") || it.getValue(self.flags.exists, "exists") || it.getValue(self.flags.has_adapter, "has_adapter")) {
              it += 2;
            } else {
              JsmnBase::skip_key_and_value(it);
            }
          }
          return true;
        }
        return false;

      },

      [](self_type &self, JsmnBase::Iterator &it) -> bool {
        if (it.getValue(self.name, "name") || it.getValue(self.dry_days, "dry_days") || it.getValue(self.duration_s, "duration_s")
            || it.getValue(self.flow_lph, "flow_lph") || it.getValue(self.adapter, "adapter")) {
          it += 2;
          return true;
        }
        return false;
      },

      [](SingleValve &self, JsmnBase::Iterator &it) -> bool { // Throw away unwanted objects
        return JsmnBase::skip_key_and_value(it);
      } };

  if (it->type == JSMN_OBJECT) { // root object
    auto count = it->size;
    for (++it; count > 0 && it; --count) {
      assert(it->type == JSMN_STRING);
      for (auto fun : tok_processRootChilds_funs) {
        if (fun(*this, it))
          break;
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

  using tok_processObj_funT = bool (*)(self_type &self, JsmnBase::Iterator &it);
  static const tok_processObj_funT tok_processRootChilds_funs[] = { //

      [](self_type &self, JsmnBase::Iterator &it) -> bool { // Process object: flags
        if (it.keyIsEqual("flags", JSMN_OBJECT)) {
          auto count = it[1].size;
          for (it += 2; count > 0 && it; --count) {
            if (it.getValue(self.flags.active, "active") || it.getValue(self.flags.exists, "exists") || it.getValue(self.flags.has_adapter, "has_adapter")) {
              it += 2;
            } else {
              JsmnBase::skip_key_and_value(it);
            }
          }
          return true;
        }
        return false;

      },

      [](self_type &self, JsmnBase::Iterator &it) -> bool {
        if (it.getValue(self.name, "name") || it.getValue(self.valve_bits, "valve_bits") || it.getValue(self.interval_days, "interval_days")
            || it.getValue(self.adapter, "adapter")) {
          it += 2;
          return true;
        }
        return false;
      },

      [](self_type &self, JsmnBase::Iterator &it) -> bool { // Throw away unwanted objects
        return JsmnBase::skip_key_and_value(it);
      } };

  if (it->type == JSMN_OBJECT) { // root object
    auto count = it->size;
    for (++it; count > 0 && it; --count) {
      assert(it->type == JSMN_STRING);

      for (auto fun : tok_processRootChilds_funs) {
        if (fun(*this, it))
          break;

      }
    }
    return true;
  }

  return false;
}

bool AutoTimer::from_json(const char *json) {
  auto jsmn = JsmnBase(json, 1024);

  if (!jsmn)
    return false;

  auto it = jsmn.begin();
  return from_json(it);
}

bool AutoTimer::from_json(JsmnBase::Iterator &it) {
  using tok_processObj_funT = bool (*)(self_type &self, JsmnBase::Iterator &it);
  static const tok_processObj_funT tok_processRootChilds_funs[] = { //

      [](self_type &self, JsmnBase::Iterator &it) -> bool {
        if (it.keyIsEqual("valves", JSMN_ARRAY)) {
          ++it; // skip key token
          auto count = it->size; // get array size
          ++it; // skip array token
          for (int i = 0; i < count; ++i) {
            if (it->type == JSMN_OBJECT) {
              self.m_valves[i].from_json(it);
            } else if (it->type == JSMN_PRIMITIVE) {
              self.m_valves[i] = SingleValve();
              ++it;
            } else {
              JsmnBase::skip_key_and_value(it);
            }

          }
          return true;
        }
        return false;
      },

      [](self_type &self, JsmnBase::Iterator &it) -> bool {
        if (it.keyIsEqual("valve_groups", JSMN_ARRAY)) {
          ++it; // skip key token
          auto count = it->size; // get array size
          ++it; // skip array token
          for (int i = 0; i < count; ++i) {
            if (it->type == JSMN_OBJECT) {
              self.m_valveGroups[i].from_json(it);
            } else if (it->type == JSMN_PRIMITIVE) {
              self.m_valveGroups[i] = ValveGroup();
              ++it;
            } else {
              JsmnBase::skip_key_and_value(it);
            }

          }
          return true;
        }
        return false;
      },

      [](self_type &self, JsmnBase::Iterator &it) -> bool {
        if (it.keyIsEqual("adapters", JSMN_ARRAY)) {
          ++it; // skip key token
          auto count = it->size; // get array size
          ++it; // skip array token
          for (int i = 0; i < count; ++i) {
            if (it->type == JSMN_OBJECT) {
              self.m_adapters[i].from_json(it);
            } else if (it->type == JSMN_PRIMITIVE) {
              self.m_adapters[i] = WeatherAdapter();
              ++it;
            } else {
              JsmnBase::skip_key_and_value(it);
            }

          }
          return true;
        }
        return false;
      },

      [](self_type &self, JsmnBase::Iterator &it) -> bool {
        if (it.getValue(self.name, "name")) {
          it += 2;
          return true;
        }
        return false;
      },

      [](self_type &self, JsmnBase::Iterator &it) -> bool { // Throw away unwanted objects
        return JsmnBase::skip_key_and_value(it);
      } };

  if (it->type == JSMN_OBJECT) { // root object
    auto count = it->size;
    for (++it; count > 0 && it; --count) {
      assert(it->type == JSMN_STRING);
      for (auto fun : tok_processRootChilds_funs) {
        if (fun(*this, it))
          break;
      }
    }
    return true;
  }

  return false;
}
