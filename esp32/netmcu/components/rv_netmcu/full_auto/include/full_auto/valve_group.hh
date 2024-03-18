#pragma once

#include "adapter.hh"
#include "jsmn/jsmn_iterate.hh"
#include <string>
#include <list>

struct ValveGroup {
  using self_type = ValveGroup;

  char name[CONFIG_APP_FA_NAMES_MAX_LEN] = "";  // Rasen/Blumen/Töpfe/Hecke/etc

  struct {
    bool active = false;  ///< to mark as active (or not disabled temporarily)
    bool exists = false;   ///< to mark as non existent flat value in array
    bool has_adapter = false; ///< to mark the usage of an adapter
  } flags;

  uint32_t valve_bits = 0;
  int interval_days = 1;  // wie oft pro Woche bewässert werden soll
  int8_t adapter = -1; ///< adapt day interval

  int to_json(char *dst, size_t dst_size) const {
    auto n = snprintf(dst, dst_size, //
        R"({"name":"%s","flags":{"active":%d,"exists":%d,"has_adapter":%d},"valve_bits":%lu,"interval_days":%d,"adapter":%d})", //
        name, //
        flags.active, flags.exists, flags.has_adapter, //
        (long unsigned)valve_bits, interval_days, adapter);

    return n < dst_size ? n : 0;
  }
  bool from_json(const char *json);
  bool from_json(JsmnBase::Iterator &it);
};
