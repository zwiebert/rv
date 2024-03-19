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
  assert(it->type == JSMN_OBJECT);

  using token_handler_fun_type = bool (*)(self_type &self, JsmnBase::Iterator &it, int &err);
  static const token_handler_fun_type tok_processRootChilds_funs[] = { //

      [](self_type &self, JsmnBase::Iterator &it, int &err) -> bool {
        if (it.keyIsEqual("flags", JSMN_OBJECT)) {
          auto count = it[1].size;
          for (it += 2; count > 0 && it; --count) {
            if (it.getValue(self.flags.exists, "exists") //
                || it.getValue(self.flags.neutral, "neutral") //
                 || it.getValue(self.flags.read_only, "read_only") //
                 ) {
              it += 2;
            } else {
              ++err;
              JsmnBase::skip_key_and_value(it);
            }
          }
          return true;
        }
        return false;

      },

      [](self_type &self, JsmnBase::Iterator &it, int &err) -> bool {
        if (it.getValue(self.name, "name") || it.getValue(self.d_temp, "temp") || it.getValue(self.d_wind, "wind") || it.getValue(self.d_humi, "humi")
            || it.getValue(self.d_clouds, "clouds")) {
          it += 2;
          return true;
        }
        return false;
      },

      [](self_type &self, JsmnBase::Iterator &it, int &err) -> bool { // Throw away unwanted objects
        return JsmnBase::skip_key_and_value(it);
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

bool MagValve::from_json(const char *json) {
  auto jsmn = Jsmn<32>(json);

  if (!jsmn)
    return false;

  auto it = jsmn.begin();
  return from_json(it);
}

bool MagValve::from_json(JsmnBase::Iterator &it) {
  assert(it->type == JSMN_OBJECT);

  using token_handler_fun_type = bool (*)(MagValve &self, JsmnBase::Iterator &it, int &err);
  static const token_handler_fun_type tok_processRootChilds_funs[] = { //

      [](MagValve &self, JsmnBase::Iterator &it, int &err) -> bool {
        if (it.keyIsEqual("flags", JSMN_OBJECT)) {
          auto count = it[1].size;
          for (it += 2; count > 0 && it; --count) {
            if (it.getValue(self.flags.active, "active") //
                || it.getValue(self.flags.exists, "exists") //
                || it.getValue(self.flags.is_due, "is_due")) {
              it += 2;
            } else {
              ++err;
              JsmnBase::skip_key_and_value(it);
            }
          }
          return true;
        }
        return false;
      },

      [](MagValve &self, JsmnBase::Iterator &it, int &err) -> bool {
        if (it.keyIsEqual("attr", JSMN_OBJECT)) {
          auto count = it[1].size;
          for (it += 2; count > 0 && it; --count) {
            if (it.getValue(self.attr.duration_s, "duration_s") //
            || it.getValue(self.attr.adapter, "adapter") //
                || it.getValue(self.attr.flow_lph, "flow_lph") //
                || it.getValue(self.attr.priority, "priority") //
                || it.getValue(self.attr.interval_s, "interval_s") //
                    ) {
              it += 2;
            } else {
              ++err;
              JsmnBase::skip_key_and_value(it);
            }
          }
          return true;
        }
        return false;
      },

      [](MagValve &self, JsmnBase::Iterator &it, int &err) -> bool {
        if (it.keyIsEqual("state", JSMN_OBJECT)) {
          auto count = it[1].size;
          for (it += 2; count > 0 && it; --count) {
            if (it.getValue(self.state.last_time_wet, "last_time_wet") //
            || it.getValue(self.state.next_time_scheduled, "next_time_scheduled") //
                ) {
              it += 2;
            } else {
              ++err;
              JsmnBase::skip_key_and_value(it);
            }
          }
          return true;
        }
        return false;
      },

      [](self_type &self, JsmnBase::Iterator &it, int &err) -> bool {
        if (it.getValue(self.name, "name")) {
          it += 2;
          return true;
        }
        return false;
      },

      [](self_type &self, JsmnBase::Iterator &it, int &err) -> bool {
        return JsmnBase::skip_key_and_value(it);
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

bool AutoTimer::from_json(const char *json) {
  auto jsmn = JsmnBase(json, 1024);

  if (!jsmn)
    return false;

  auto it = jsmn.begin();
  return from_json(it);
}

bool AutoTimer::from_json(JsmnBase::Iterator &it) {
  assert(it->type == JSMN_OBJECT);

  using token_handler_fun_type = bool (*)(self_type &self, JsmnBase::Iterator &it, int &err);

  static const token_handler_fun_type tok_processRootChilds_funs[] = { //

      [](self_type &self, JsmnBase::Iterator &it, int &err) -> bool {
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
              JsmnBase::skip_key_and_value(it);
            }

          }
          return true;
        }
        return false;
      },

      [](self_type &self, JsmnBase::Iterator &it, int &err) -> bool {
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
              JsmnBase::skip_key_and_value(it);
            }

          }
          return true;
        }
        return false;
      },

      [](self_type &self, JsmnBase::Iterator &it, int &err) -> bool {
        if (it.getValue(self.name, "name")) {
          it += 2;
          return true;
        }
        return false;
      },

      // any consume not handled tokens
          [](self_type &self, JsmnBase::Iterator &it, int &err) -> bool {
            return JsmnBase::skip_key_and_value(it);
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
