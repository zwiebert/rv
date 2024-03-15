#pragma once

#include "adapter.hh"
#include <string>
#include <list>

struct ValveGroup {
  char name[CONFIG_APP_FA_NAMES_MAX_LEN] = "";  // Rasen/Blumen/Töpfe/Hecke/etc

  struct {
    bool active;  ///< to mark as active (or not disabled temporarily)
    bool exists;   ///< to mark as non existent flat value in array
    bool has_adapter; ///< to mark the usage of an adapter
  } flags;

  uint32_t valve_bits = 0;
  int interval_days;  // wie oft pro Woche bewässert werden soll
  int8_t adapter = -1; ///< adapt day interval

  bool to_json(char *dst, size_t dst_size) {
    auto n = snprintf(dst, dst_size, //
        R"({"name":"%s","flags":{"active":%d,"exists":%d,"has_adapter":%d},"valve_bits":%lu,"interval_days":%d,"adapter":%d})", //
        name, //
        flags.active, flags.exists, flags.has_adapter, //
        valve_bits, interval_days, adapter);

    return n < dst_size;
  }
  bool from_json(const char *json);
};
