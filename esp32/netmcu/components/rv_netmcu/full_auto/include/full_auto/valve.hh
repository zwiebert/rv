#pragma once

#include "adapter.hh"
#include <cstring>
#include <cstdio>
#include <ctime>
#include "jsmn/jsmn_iterate.hh"

const int MAX_DRY_DAYS = 30;

struct MagValve {
  using self_type = MagValve;

  char name[CONFIG_APP_FA_NAMES_MAX_LEN] = "";

  struct {
    bool active = false;  ///< to mark as active (or not disabled temporarily)
    bool exists = false;   ///< to mark as non existent flat value in array
    bool has_adapter = false; ///< to mark the usage of an adapter
  } flags;

  struct {
    unsigned duration_s = 0;
    int8_t adapter = -1; ///< adapt duration
    unsigned flow_lph = 0;
    int priority = 0;
  } attr;

  struct {
    time_t last_time_wet = 0;
    time_t next_time_scheduled = 0;
  } state;


  int to_json(char *dst, size_t dst_size) const {
#if 0
    auto n = snprintf(dst, dst_size, //
        R"({"name":"%s","flags":{"active":%d,"exists":%d,"has_adapter":%d},"dry_days":%d,"duration_s":%d,"flow_lph":%d,"adapter":%d})", //
        name, //
        flags.active, flags.exists, flags.has_adapter, //
        dry_days, duration_s, flow_lph, adapter);

    return n < dst_size ? n : 0;
#else
    return 0;
  }


  /*
   * \brief parse JSON and then calls this->from_json(JsmnBase::Iterator &it)
   */
  bool from_json(const char *json);

  /*
   * \brief     initialize *this by default ctor and then get any values from parsed JSON object
   * \param it  Iterator pointing to the object token (JSMN_OBJECT)
   * \return
   */
  bool from_json(JsmnBase::Iterator &it);
};
