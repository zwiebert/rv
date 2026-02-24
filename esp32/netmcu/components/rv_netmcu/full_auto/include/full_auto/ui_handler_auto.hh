#pragma once
#include "automatic_timer.hh"

namespace ui {

constexpr const char *our_logtag = "ui.auto";
constexpr auto zone_prefix = "zone.";
constexpr auto adapter_prefix = "adapter.";

namespace details {

  ///////////////////////////////////////////////////////////////////////////////
  ////////////   Handlers for external commands via JSON auto object ////////////
  ///////////////////////////////////////////////////////////////////////////////
  /**
   * \brief      Handles auto.command object
   * \tparam jsmn_iterator
   * \param it
   * \return
   */
  template<typename jsmn_iterator>
  bool handler__command(AutoTimer &at, jsmn_iterator &it) {
    int err = 0;
    assert(it->type == JSMN_OBJECT);
    auto count = it->size;
    for (it += 1; count > 0 && it; --count) {
      char cmds[32] = "";

      if (it.takeValue(cmds, "save")) {
        auto key = *cmds ? cmds : AutoTimer::default_save_key;
        if (at.save_settings(key))
          continue;
      db_loge(our_logtag, "save with key <%s> failed", key);
    }
    if (it.takeValue(cmds, "restore")) {
      auto key = *cmds ? cmds : AutoTimer::default_save_key;
      if (at.restore_settings(key))
        continue;
    db_loge(our_logtag, "restore from key <%s> failed", key);
  }

  ++err;
  it.skip_key_and_value();
  continue;
}
return !err;
}
/**
 * \brief                 handles auto.set command object
 * \tparam jsmn_iterator
 * \param sj
 * \param it
 * \param update
 * \return
 */
template<typename jsmn_iterator>
bool handler__set(AutoTimer &at, UoutBuilderJson &sj, jsmn_iterator &it, bool update = false) {
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
      if (at.update_zone(zone_idx, ++it) && at.write_zone_json(sj, zone_idx, key)) {
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
      if (at.update_adapter(adapter_idx, ++it)) {
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
/**
 * \brief   handles auto.get command object
 * \tparam jsmn_iterator
 * \param sj
 * \param it
 * \return
 */
template<typename jsmn_iterator>
bool handler__get(AutoTimer &at, UoutBuilderJson &sj, jsmn_iterator &it) {
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
      if (at.write_zone_json(sj, zone_idx, key)) {
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
      if (at.write_adapter_json(sj, adapter_idx, key)) {
        continue; // update succeeded
      }
    }
    db_loge(our_logtag, "Could not get adapter %d", adapter_idx);
    ++err;
    continue;
  }
  if (it.keyIsEqual("zones", JSMN_ARRAY)) {
    it.skip_key_and_value();
    if (at.write_zones_json(sj, "zones")) {
      continue;
    }
    db_loge(our_logtag, "Could not get zones");
    ++err;
    continue;
  }
  if (it.keyIsEqual("adapters", JSMN_ARRAY)) {
    it.skip_key_and_value();
    if (at.write_adapters_json(sj, "adapters")) {
      continue;
    }
    db_loge(our_logtag, "Could not get adapters");
    ++err;
    continue;
  }

  if (it.keyIsEqual("past_wd", JSMN_ARRAY)) {
    it.skip_key_and_value();
    if (at.write_past_weather_data_json(sj, "past_wd")) {
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

}

/**
 * \brief    ???  Command interface taking JSON (taking "auto" object which contains commands)
 * \tparam        Iterator type
 * \param sj      JSON builder for creating output as response
 * \param it      JSON JSMN iterator for input
 * \return        true for no errors
 */
template<typename jsmn_iterator>
bool handler__auto(AutoTimer &at, UoutBuilderJson &sj, jsmn_iterator &it) {
int err = 0;
assert(it->type == JSMN_OBJECT);
auto count = it->size;

if (sj.add_object("auto")) {
  for (it += 1; count > 0 && it; --count) {

    if (it.keyIsEqual("command", JSMN_OBJECT)) {
      if (details::handler__command(at, ++it))
        continue;

      db_loge(our_logtag, "Command failed");
      ++err;
      continue;
    }

    if (it.keyIsEqual("get", JSMN_OBJECT)) {
      if (details::handler__get (at, sj, ++it))
        continue;

      db_loge(our_logtag, "get failed");
      ++err;
      continue;
    }

    if (it.keyIsEqual("set", JSMN_OBJECT)) {
      if (details::handler__set(at, sj, ++it))
        continue;

      db_loge(our_logtag, "set failed");
      ++err;
      continue;
    }

    if (it.keyIsEqual("update", JSMN_OBJECT)) {
      if (details::handler__set (at, sj, ++it), true)
        continue;

      db_loge(our_logtag, "update failed");
      ++err;
      continue;
    }

    db_loge(our_logtag, "unknown key found in json.auto");
    ++err;
    it.skip_key_and_value();

  }
  sj.close_object();
}
return !err;
}


}
