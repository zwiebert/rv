#pragma once

#include "adapter.hh"
#include <cstring>
#include <cstdio>

const int MAX_DRY_DAYS = 30;

struct SingleValve {
  char name[CONFIG_APP_FA_NAMES_MAX_LEN] = "";

  struct {
    bool active = false;  ///< to mark as active (or not disabled temporarily)
    bool exists = false;   ///< to mark as non existent flat value in array
    bool has_adapter = false; ///< to mark the usage of an adapter
  } flags;

  unsigned dry_days = MAX_DRY_DAYS; // days since last rain or irrigation

  unsigned duration_s = 0;
  unsigned flow_lph = 0;
  int8_t adapter = -1; ///< adapt duration

  int to_json(char *dst, size_t dst_size) const {
    auto n = snprintf(dst, dst_size, //
        R"({"name":"%s","flags":{"active":%d,"exists":%d,"has_adapter":%d},"dry_days":%d,"duration_s":%d,"flow_lph":%d,"adapter":%d})", //
        name, //
        flags.active, flags.exists, flags.has_adapter, //
        dry_days, duration_s, flow_lph, adapter);

    return n < dst_size ? n : 0;
  }
  bool from_json(const char *json);
};
