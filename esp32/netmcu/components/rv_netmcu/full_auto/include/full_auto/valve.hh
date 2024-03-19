#pragma once

#include "adapter.hh"
#include <cstring>
#include <cstdio>
#include <ctime>
#include <jsmn/jsmn_iterate.hh>
#include <utils_time/ut_constants.hh>

struct MagValve {
  using self_type = MagValve;

  char name[CONFIG_APP_FA_NAMES_MAX_LEN] = "";

  struct {
    bool active = false;  ///< to mark as active (or not disabled temporarily)
    bool exists = false;   ///< to mark as non existent flat value in array
    bool is_due = false;
  } flags;

  struct {
    unsigned duration_s = 0;
    int8_t adapter = 0; ///< adapt duration (0 should always be a neutral adapter)
    unsigned flow_lph = 0;
    int priority = 0;
    unsigned interval_s = SECS_PER_DAY;
  } attr;

  struct {
    time_t last_time_wet = 0;
    time_t next_time_scheduled = 0;
  } state;


  int to_json(char *dst, size_t dst_size) const {
    auto n = snprintf(dst, dst_size, //
        R"({"name":"%s","flags":{"active":%d,"exists":%d,"is_due":%d},"attr":{"duration_s":%d,"adapter":%d,"flow_lph":%d,"priority":%d,"interval_s":%d},"state":{"last_time_wet":%llu,"next_time_scheduled":%llu}})", //
        name, //
        flags.active, flags.exists, flags.is_due, //
        attr.duration_s, attr.adapter, attr.flow_lph, attr.priority, attr.interval_s, //
        (long long unsigned)state.last_time_wet, (long long unsigned)state.next_time_scheduled //
    );

    return n < dst_size ? n : 0;
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
